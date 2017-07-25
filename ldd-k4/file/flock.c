#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
	int fd = open( "data", O_CREAT | O_TRUNC);
	if( -1 == flock( fd, LOCK_NB | LOCK_EX)){// lock_non_block lock_exclusive
		printf( "Oops\n");// file locked by others
	}else{
		printf( "Wait\n");// got file lock
		sleep( 10);
	}
	close( fd);
	return 0;
}
