CC = gcc
STRIP = strip


all:	
	gcc ${CFLAGS} alsa-record-example.c  ${LDFLAGS} -o alsa-record-example -l pthread -lasound -g
	gcc ${CFLAGS} alsa_loop.c  ${LDFLAGS} -o alsa_loop -l pthread -lasound -g	
	gcc ${CFLAGS} pcm_min.c  ${LDFLAGS} -o pcm_min -l pthread -lasound -g
	gcc ${CFLAGS} sound_playback.c  ${LDFLAGS} -o sound_playback -l pthread -lasound -g	
	#gcc ${CFLAGS} pcm.c   ${LDFLAGS} -o pcm -l pthread -lasound -g	
clean:
	rm -rf *.so *.o *.bak *.so *.a
