/*
Linux提供兩種非同步的hotplug機制通知user-space裝置狀態的改變，
一是usermode helper，另一個則是透過netlink。

usermode helper
每當kernel收到hotplug event便會執行"CONFIG_UEVENT_HELPER_PATH"
（預設值是/sbin/hotplug，可以透過修改/proc/sys/kernel/hotplug修改預設值），
在embedded system中，常用的是busybox，所以，
通常會把UEVENT HELPER換成/sbin/mdev（找時間來寫寫busybox），
執行UEVENT HELPER會攜帶一些環境變數，如ACTION/DEVPATH/SUBSYSTEM/HOME/PATH等等，
透過這些環境變數，可以取得Device Name，MAJOR/MINOR number等等

我利用一個shell script來取代UEVENT HELPER，主要目的是要印出有哪些環境變數，
和傳遞哪些參數給UEVENT HELPER。

#!/bin/sh
env >> /helper.log
echo "CMD:" $@ >> /helper.log
echo "----------     end     ----------" >> /helper.log
netlink
另外一條路徑就是netlink了，基本上也是傳遞相同的資訊。

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