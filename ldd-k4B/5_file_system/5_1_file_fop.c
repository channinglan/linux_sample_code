#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


#define FILE_NAME "/tmp/hello.txt"
#define TEST_STR  "1234567890abcdefgihijklmnopq\0"
#define TEST_STR2  "The full list of file creation flags and file status flags is as"
#define MAX_SIZE	128
#define SEEK_LEN	5

void ex_fseek(void)
{
    FILE* stream;
    long l;
    float fp;
    char s[81];
    char c;

	printf("\n --------- %s start --------- ",__FUNCTION__);
    stream = fopen(FILE_NAME,"w+");
    if(stream == NULL)  
    {
        printf("the file is opeaned error!\n");
    }
    else  
    {
        fprintf(stream,"%s %ld %f %c","a_string",6500,3.1415,'x');
        fseek(stream,0L,SEEK_SET);
        fscanf(stream,"%s",s);
        fscanf(stream,"%ld",&l);
        fscanf(stream,"%f",&fp);
        fscanf(stream," %c",&c);
        printf("%s\n",s);
        printf("%ld\n",l);
        printf("%f\n",fp);
        printf("%c\n",c);
        fclose(stream);  // ??
    }

	printf("\n --------- %s end --------- ",__FUNCTION__);
}

void ex_fwrite_fread( void )
{
    FILE *stream;
    char list[30];
    int i, numread, numwritten;
	printf("\n --------- %s start --------- ",__FUNCTION__);
    if( (stream = fopen(FILE_NAME, "w+t" )) != NULL )  // 如果?取??
    {
        for ( i = 0; i < 25; i++ )
        	list[i] = (char)('z' - i);
        numwritten = fwrite( list, sizeof( char ), 25, stream );
        numwritten = fwrite( TEST_STR, sizeof( TEST_STR ), 1, stream );

        printf( "\n Wrote %d items = %s\n", numwritten ,list);
        fclose( stream );
    }
    else
    {
        printf( "Problem opening the file\n" );
    }
    if( (stream = fopen(FILE_NAME, "r+t" )) != NULL )  // 文件?取
    {
        numread = fread( list, sizeof( char ), 25, stream );
        printf( "Number of items read = %d\n", numread );
        printf( "Contents of buffer = %.25s\n", list );
        fclose( stream );
    }
    else
    {
        printf( "File could not be opened\n" );
    }
	printf("\n --------- %s end --------- ",__FUNCTION__);
}

//char *fgets(char *str, int n, FILE *stream)
int ex_fputs_fgets(void)
{
    char msg[] = "This is a test!\n secend line\n";
    char msgget[MAX_SIZE];
    int i = 0;
    FILE* fstream;

	printf("\n --------- %s start --------- \n",__FUNCTION__);


    fstream = fopen(FILE_NAME,"w+");
    if(fstream==NULL)
    {
        printf("Open file failed!\n");
        exit(1);
    }
    fputs(msg, fstream);
    fflush(fstream);
    close(fileno(fstream));
    fstream=fopen(FILE_NAME,"r");
    i = 0;
    fgets(msgget,3,fstream) ;
    fputs(msgget, stdout);
    printf("\n");
    fgets(msgget,100,fstream) ;
    fputs(msgget, stdout);

    close(fileno(fstream));
	printf("\n --------- %s end --------- ",__FUNCTION__);
    return 0;
}

//int fsetpos(FILE *stream, const fpos_t *pos)
int ex_fsetpos(void)
{
FILE *stream;
   int retcode;
   char ptr[MAX_SIZE];  /* existing file 'mylib/myfile' has 20 byte records */
   int  i,ret;
    fpos_t pos;

	printf("\n --------- %s start --------- \n",__FUNCTION__);


    stream = fopen(FILE_NAME, "w+");
    ret = fwrite(TEST_STR, strlen(TEST_STR), 1, stream);

       printf("fwrite %d byte: %s",ret,TEST_STR);
    fclose(stream);

   printf("\n Open file, get position of file pointer, and read first record" );

   stream = fopen(FILE_NAME, "rb");
   fgetpos(stream,&pos);


   ret = fread(ptr,strlen(TEST_STR)/2,1,stream);

   printf("\n 1st record %d: %s\n", ret,ptr);

   printf("\n Perform another read operation on the second record ");
   /*  - the value of 'pos' changes                             */
	ret = fread(ptr,strlen(TEST_STR)/2,1,stream);

	printf("\n 2nd record %d: %s\n",ret, ptr);

   printf("\n Re-set pointer to start of file and re-read first record  ");
   fsetpos(stream,&pos);

	ret = fread(ptr,strlen(TEST_STR),1,stream);

	printf("\n 1st record again: %s\n", ptr);

   fclose(stream);
}





int main(void)
{

    ex_fwrite_fread();
    ex_fseek();
    ex_fputs_fgets();
    ex_fsetpos();



    printf("\n");

    return 0;
}