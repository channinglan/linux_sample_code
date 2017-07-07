/* Scott Kuhl */

/*
 Intermixing reads and writes to FILE* streams is potentially
 problematic. Writes can be buffered and then subsequent reads may
 have undefined results. The solution is to call fseek(stream, 0L,
 SEEK_CUR) every time you switch between writing and reading (and vice
 versa). Even if the program runs as expected without adding this
 additional call, it is important to do to avoid undefined behavior on
 some implementations. NOTE: We aren't checking return values in this
 program, and it is good practice to do so.
*/

/* From the fopen(3) man page (from Ubuntu 13.10):

   Reads  and writes may be intermixed on read/write streams in any order.
   Note that ANSI C requires that a file  positioning  function  intervene
   between  output and input, unless an input operation encounters end-of-
   file.  (If this condition is not met, then a read is allowed to  return
   the result of writes other than the most recent.)  Therefore it is good
   practice (and  indeed  sometimes  necessary  under  Linux)  to  put  an
   fseek(3)  or  fgetpos(3) operation between write and read operations on
   such a stream.   This  operation  may  be  an  apparent  no-op  (as  in
   fseek(..., 0L, SEEK_CUR) called for its synchronizing side effect.
*/

/* The C99 standard, section 7.19.5.3 states:

   When a file is opened with update mode ('+' as the second or third
   character in the above list of mode argument values), both input
   and output may be performed on the associated stream. However,
   output shall not be directly followed by input without an
   intervening call to the fflush function or to a file positioning
   function (fseek, fsetpos, or rewind), and input shall not be
   directly followed by output without an intervening call to a file
   positioning function, unless the input operation encounters
   endof-file.  Opening (or creating) a text file with update mode may
   instead open (or create) a binary stream in some implementations.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FILENAME "fileio-readwrite.temp"

void example(int fixUndefinedBehavior)
{
	FILE *fs = fopen(FILENAME, "w+");

	
	fprintf(fs, "123456789012345678901234567890");
	fseek(fs, 0L, SEEK_SET); /* seek to beginning */
	for(int i=0; i<5; i++)
	{
		fputc('x', fs);
		fputc('x', fs);
		fputc('x', fs);
		if(fixUndefinedBehavior) // between write and read
			// Appears to be a no-op, but has a synchronizing
			// side-effect that will cause us to avoid undefined
			// behavior be switching from write to read.
			fseek(fs, 0L, SEEK_CUR);
		int in = fgetc(fs);
		if(fixUndefinedBehavior) // between read and write
			fseek(fs, 0L, SEEK_CUR);
		printf("read a byte; in = '%c'\n", (char)in);
	}

	fseek(fs, 0L, SEEK_SET); /* seek back to beginning */
	char buf[256] = "";
	fread(buf, 1, 256, fs);
	printf("buf: '%s'\n", buf);

	fclose(fs);
	unlink(FILENAME);
}

int main(void)
{
	// Even if both of these print out the same result on your
	// machine, the possibility for undefined behavior still exists.

	printf("Undefined behavior:\n");
	example(0);

	printf("Correct!:\n");
	example(1);
	
}
