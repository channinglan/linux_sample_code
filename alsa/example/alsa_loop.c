/*
 * Copyright (c) 2012 Daniel Mack
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

/*
 * See README
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <alsa/asoundlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE (1024 * 32)
#define BUF_COUNT 10
snd_pcm_t *playback_handle, *capture_handle;
snd_pcm_info_t *playback_info, *capture_info;


static int debug=0;
#define	deb(level,fmt,	arg...)			\
	if(debug > level) {			\
		printf("" fmt, ##arg);		\
	}


static unsigned int format = SND_PCM_FORMAT_S16_LE;
static int audio_sample_rate=48000;
static char capture_device_str[32]="default";
static char play_device_str[32]="default";
#define VERSION_STR "1.0"

static void version(void)
{
	printf("\nversion " VERSION_STR "\n");
}


static int open_stream(snd_pcm_t **handle,snd_pcm_info_t *info, const char *name, int dir,int format)
{
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;

	const char *dirname = (dir == SND_PCM_STREAM_PLAYBACK) ? "PLAYBACK" : "CAPTURE";
	int err;

	if ((err = snd_pcm_open(handle, name, dir, 0)) < 0) {
		fprintf(stderr, "%s (%s): cannot open audio device (%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
#if 1
	snd_pcm_info_alloca(&info);
	if ((err = snd_pcm_info(*handle, info)) < 0) {
		fprintf(stderr, ("info error: %s"), snd_strerror(err));
		return err;
	}
#endif

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot allocate hardware parameter structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params_any(*handle, hw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot initialize hardware parameter structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params_set_access(*handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "%s (%s): cannot set access type(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params_set_format(*handle, hw_params, format)) < 0) {
		fprintf(stderr, "%s (%s): cannot set sample format(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params_set_rate_near(*handle, hw_params, &audio_sample_rate, NULL)) < 0) {
		fprintf(stderr, "%s (%s): cannot set sample rate(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params_set_channels(*handle, hw_params, 2)) < 0) {
		fprintf(stderr, "%s (%s): cannot set channel count(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	if ((err = snd_pcm_hw_params(*handle, hw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot set parameters(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot allocate software parameters structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_sw_params_current(*handle, sw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot initialize software parameters structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_sw_params_set_avail_min(*handle, sw_params, BUFSIZE)) < 0) {
		fprintf(stderr, "%s (%s): cannot set minimum available count(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_sw_params_set_start_threshold(*handle, sw_params, 0U)) < 0) {
		fprintf(stderr, "%s (%s): cannot set start mode(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_sw_params(*handle, sw_params)) < 0) {
		fprintf(stderr, "%s (%s): cannot set software parameters(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}


	printf("\n snd_pcm_name=%s",snd_pcm_name(*handle));

	return 0;
}


#define FIFO_MAX_SIZE (BUFSIZE * BUF_COUNT)
char *fifo_buf;
#define PCM_SIZE	4	//2ch 16bit
int w_ptr=0,r_ptr=0,fifo_size=0;
int buf_fifo(int mode,char *buf,unsigned int len)
{
	unsigned int tmp_len;

	if(mode == 0) { //w
		if(w_ptr+len > FIFO_MAX_SIZE) {
			deb(1,"\n over max szie");
			tmp_len=FIFO_MAX_SIZE-w_ptr;
			memcpy((void *)&fifo_buf[w_ptr],(void *)buf,tmp_len);
			memcpy((void *)&fifo_buf[0],(void *)&buf[tmp_len],(len-tmp_len));
			w_ptr=len-tmp_len;
		} else {
			memcpy((void *)&fifo_buf[w_ptr],(void *)buf,len);
			w_ptr+=len;
		}
		fifo_size+=len;
		deb(1,"\n w %d  len %d fifo %d",w_ptr,len,fifo_size);

		return len;
	} else {	//r
		if(fifo_size < len) {
			len = fifo_size;
		}

		deb(1,"\n r %d  len %d fif0 %d",r_ptr,len,fifo_size);
		if(r_ptr+len > FIFO_MAX_SIZE) {
			deb(1,"\n over max szie");
			tmp_len=FIFO_MAX_SIZE-r_ptr;
			memcpy((void *)buf,(void *)&fifo_buf[r_ptr],tmp_len);
			memcpy((void *)&buf[tmp_len],(void *)&fifo_buf[0],(len-tmp_len));
			r_ptr=len-tmp_len;
		} else {
			memcpy((void *)buf,(void *)&fifo_buf[r_ptr],len);
			r_ptr+=len;
		}

		fifo_size-=len;


		return len;
	}

}




int main(int argc, char *argv[])
{
	unsigned long play_byte=0,capture_byte=0;
	int err,play_prepare_flag=0;
	int c;

	while((c=getopt(argc, argv, "vhr:c:p:d:")) != -1)
	{
	    switch(c)
	    {
	    case 'h':
		printf("\n -r 48000	  :sample rate");
		printf("\n -c hw:x,x	  :audio CAPTURE device");
		printf("\n -p hw:x,x	  :audio PLAY device");
		printf("\n");
		return 0;
		
	    case 'v':		
		version();
		return 0;		
		
	    case 'r':
		audio_sample_rate = atoi(optarg);
		printf("\n audio_sample_rate=%d",audio_sample_rate);
		break;

	    case 'c':
		sscanf(optarg, "%s", capture_device_str);
		printf("\n capture_device_str=%s",capture_device_str);
		break;

	    case 'p':
		sscanf(optarg, "%s", play_device_str);
		printf("\n play_device_str=%s",play_device_str);
		break;
		
	    case 'd':
		debug = atoi(optarg);
		printf("\n debug=%d",debug);
		break;		
		
	    default:
		puts("wrong command");
		break;
	    }
	}


	fifo_buf = (char*) malloc(sizeof(char)*FIFO_MAX_SIZE);

//	if ((err = open_stream(&playback_handle, "hw:1,0", 		SND_PCM_STREAM_PLAYBACK,format)) < 0)
	if ((err = open_stream(&playback_handle,playback_info, play_device_str, 	SND_PCM_STREAM_PLAYBACK,format)) < 0)
		return err;



	if ((err = open_stream(&capture_handle,capture_info, capture_device_str, 	SND_PCM_STREAM_CAPTURE,format)) < 0)
//	if ((err = open_stream(&capture_handle, "hw:2,0", 		SND_PCM_STREAM_CAPTURE,format)) < 0)
		return err;

	if ((err = snd_pcm_start(capture_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
			 snd_strerror(err));
		return err;
	}



	deb(0,"\n init pcm");
	//memset(buf, 0, sizeof(buf));


	deb(0,"\n start loop play");
	while (1) {
		char capture_buf[BUFSIZE*PCM_SIZE];
		char play_buf[BUFSIZE*PCM_SIZE];
		int capture_avail=0,capture_len=0;
		int play_avail=0,play_len=0;
		//int 	snd_pcm_wait (snd_pcm_t *pcm, int timeout)
 		//Wait for a PCM to become ready.
		//if ((err = snd_pcm_wait(playback_handle, 100)) < 0) {
		//	fprintf(stderr, "poll failed(%s)\n", strerror(errno));
		//	break;
		//}
		if ((err = snd_pcm_wait(capture_handle, 20)) < 0) {
			fprintf(stderr, "capture poll failed(%s)\n", strerror(errno));
			break;
		}


		capture_avail = snd_pcm_avail_update(capture_handle);
		if (capture_avail > 0) {
			deb(1,"\n capture_avail %d",capture_avail);
			if (capture_avail > BUFSIZE)
				capture_avail = BUFSIZE;


			snd_pcm_readi(capture_handle, capture_buf, capture_avail);

			capture_len = buf_fifo(0,capture_buf,capture_avail*PCM_SIZE)/PCM_SIZE;
			capture_byte+=capture_len;

			deb(1,"\n capture 0x%x %d/%d(%ld)",capture_buf[0],capture_len,capture_avail,capture_byte);
		} else if (capture_avail < 0) {
			deb(1,"capture cannot update audio avail for use(%s)\n",
			 snd_strerror(capture_avail));
		}


		if(play_prepare_flag==0) {
			if(capture_byte > BUFSIZE/2) {
				if ((err = snd_pcm_prepare(playback_handle)) < 0) {
					fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
					snd_strerror(err));
					return err;
				}
				play_prepare_flag=1;
				deb(0,"\n playback prepare ok");
			}
		} else {
			play_avail = snd_pcm_avail_update(playback_handle);
			if (play_avail > 0) {
				deb(1,"\n playback avail %d",play_avail);

				if (play_avail > BUFSIZE)
					play_avail = BUFSIZE;

				if (play_avail > 3840)
					play_avail = 3840;
					
				if (play_avail > fifo_size)
					play_avail = fifo_size;					
					

				 play_len = buf_fifo(1,play_buf,play_avail*PCM_SIZE)/PCM_SIZE;
			 	capture_len=0;

				snd_pcm_writei(playback_handle, play_buf, play_avail);

				play_byte+=play_avail;
				deb(1,"\n playback 0x%x %d/%d(%ld)",play_buf[0],play_len,play_avail,play_byte);
			} else if (play_avail < 0){
				fprintf(stderr,"play cannot update audio avail for use(%s)\n",
				 snd_strerror(play_avail));
			}
		}
	}
	free(fifo_buf);
	snd_pcm_close(playback_handle);
	snd_pcm_close(capture_handle);
	return 0;
}