#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#define FILE_NAME "/tmp/hello.txt"
#define MAX_SIZE	128
int main(void)
{
	int fd,fd2,fd3,len;
	char str[MAX_SIZE];

    errno = 0;
    /* Through O_CREAT flag the open() system call
       creates a file if it does not exist at the 
       specified path.The third argument represents 
       the access permissions for the file */
    fd = open(FILE_NAME,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);

    if(-1 == fd)
    {
        printf("\n open() failed with error [%s]\n",strerror(errno));
        return 1;
    }
    else
    {
        printf("\n Open() Successful\n");
        /* open() succeeded, now one can do read operations
           on the file opened since we opened it in read-only
           mode. Also once done with processing, the file needs
           to be closed. Closing a file can be achieved using
           close() function. */
    }
    
    
    write(fd,"1234567890",strlen("1234567890"));
    close(fd);
    
    fd2 = open(FILE_NAME,O_RDWR);    
    len = read(fd2,str,MAX_SIZE);
    str[len] = '0';
    printf("\n %s",str);
       close(fd2); 
    
    fd3 = open(FILE_NAME,O_RDWR);   
    lseek(fd3,5,SEEK_CUR);
    len = read(fd3,str,MAX_SIZE);
    str[len] = '0';
    printf("\n %s",str);
       close(fd3);     
    

    return 0;
}