/*
Linux���Ѩ�ثD�P�B��hotplug����q��user-space�˸m���A�����ܡA
�@�Ousermode helper�A�t�@�ӫh�O�z�Lnetlink�C

usermode helper
�C��kernel����hotplug event�K�|����"CONFIG_UEVENT_HELPER_PATH"
�]�w�]�ȬO/sbin/hotplug�A�i�H�z�L�ק�/proc/sys/kernel/hotplug�ק�w�]�ȡ^�A
�bembedded system���A�`�Ϊ��Obusybox�A�ҥH�A
�q�`�|��UEVENT HELPER����/sbin/mdev�]��ɶ��Ӽg�gbusybox�^�A
����UEVENT HELPER�|��a�@�������ܼơA�pACTION/DEVPATH/SUBSYSTEM/HOME/PATH�����A
�z�L�o�������ܼơA�i�H���oDevice Name�AMAJOR/MINOR number����

�ڧQ�Τ@��shell script�Ө��NUEVENT HELPER�A�D�n�ت��O�n�L�X�����������ܼơA
�M�ǻ����ǰѼƵ�UEVENT HELPER�C

#!/bin/sh
env >> /helper.log
echo "CMD:" $@ >> /helper.log
echo "----------     end     ----------" >> /helper.log
netlink
�t�~�@�����|�N�Onetlink�F�A�򥻤W�]�O�ǻ��ۦP����T�C

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/netlink.h>

int main(int argc, char *argv[])
{
    struct sockaddr_nl nls;
    struct pollfd pfd;
    char buf[512];

    memset(&nls, 0, sizeof(nls));
    nls.nl_family = AF_NETLINK;
    nls.nl_pid = getpid();
    nls.nl_groups = -1;

    pfd.events = POLLIN;
    pfd.fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (pfd.fd == -1) {
        printf("Not root\n");
        exit(1);
    }

    if (bind(pfd.fd, (void*)&nls, sizeof(nls))) {
        printf("bind failed\n");
        exit(1);
    }
    while (-1 != poll(&pfd, 1, -1)) {
        int i, len = recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
        if (len == -1) {
            printf("recv\n");
            exit(1);
        }
        i = 0;
        while (i < len) {
            printf("%s\n", buf + i);
            i += strlen(buf+i) + 1;
        }
    }
    printf("\n");
    return 0;
}