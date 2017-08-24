#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <sys/vfs.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define UEVENT_BUFFER_SIZE 1024

static int init_hotplug_sock(void)
{
  struct sockaddr_nl snl;
  const int buffersize = 16 * 1024;
  int retval;

  memset(&snl, 0x00, sizeof(struct sockaddr_nl));
  snl.nl_family = AF_NETLINK;
  snl.nl_pid = getpid();
  snl.nl_groups = 1;

  int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  if (hotplug_sock == -1) {
      printf("error getting socket: %s", strerror(errno));
      return -1;
  }

  /* set receive buffersize */
  setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
  retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));

  if (retval < 0) {
      printf("bind failed: %s", strerror(errno));
      close(hotplug_sock);
      hotplug_sock = -1;
      return -1;
  }
  return hotplug_sock;
}

int parse_usbcam(char *devstr)
{
  char info[256];
  char *p = info, *p2;
  char usb_node[256];
  char action[256];
  char dev_name[256];
  int v4l_id = 0, ret = 0;

  strcpy(info, devstr);
  if((p2 = strstr(p, "video4linux")) == NULL)
	return -1;

  // find v4l id
  p2 += strlen("video4linux") + 1;
  p2 += strlen("video");
  v4l_id = p2[0] - '0';
  // find usb node
  if((p2 = strstr(p, "usb")) == NULL)
	return -1;
  // let's skip two '/'
  if((p = strstr(p2, "/")) == NULL)
	return -1;
  p++;
  if((p2 = strstr(p, "/")) == NULL)
	return -1;
  p2++;
  // in usb node
  if((p = strstr(p2, "/")) == NULL)
	return -1;
  p[0] = 0;
  strcpy(usb_node, p2);
  // action
  p = info;
  if((p2 = strstr(p, "@")) == NULL)
	return -1;
  p2[0] = 0;
  strcpy(action, p);

  //printf("video4linux id=%d\n", v4l_id);
  //printf("usb node:[%s]\n", usb_node);
  //printf("action:[%s]\n", action);

  // xml format
  printf("<usbcam>\n");
  printf("<number>%d</number>\n", v4l_id);
  printf("<usb_node>%s</usb_node>\n", usb_node);
  printf("<action>%s</action>\n", action);
  // try to find web cam model name
  if(!strcmp(action, "add")) {
  	char path[256];

	memset(dev_name, 0, sizeof(dev_name));
  	sprintf(path, "/sys/class/video4linux/video%d/name", v4l_id); 
	FILE *fp = fopen(path, "r");
	if(fp == NULL) {
	    //printf("open:[%s] failed\n", path);
	    goto end_parse;
	}
	fread(dev_name, 1, sizeof(dev_name), fp);
	fclose(fp);
    	if(dev_name[strlen(dev_name) - 1] == '\n')
    	    dev_name[strlen(dev_name) - 1] = 0;
        //printf("model name:[%s]\n", dev_name);
  	printf("<name>%s</name>\n", dev_name);
	// 
	//sprintf(path, "/sbin/write_log \"%s plugin\" 2\n", dev_name);
	//system(path);
  }
end_parse:
  printf("</usbcam>\n");

  return 0;
}

int main(int argc, char* argv[])
{
  int hotplug_sock = init_hotplug_sock();
  int ret = 0;
  char buf_cache[UEVENT_BUFFER_SIZE*2] = {0}; 
       
  while(1) {
    memset(buf_cache, 0 ,sizeof(buf_cache));
    recv(hotplug_sock,buf_cache, sizeof(buf_cache), 0);
    printf("======buf_cache: %s\n", buf_cache);
    ret = parse_usbcam(buf_cache);
  }             
         
 return 0;
}
