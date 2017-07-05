// Scott Kuhl
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char s[100];
	while(1)
	{
		//fclose(stdin); // uncomment this to make fgets fail!
		
		// fgets() reads a line of text from a FILE* stream. It always
		// will null terminate the string.
		char *ret = fgets(s, 100, stdin);
		if(ret == NULL)
		{
			if(feof(stdin))
			{
				// To trigger end of file interactively, try pressing
				// Ctrl+D when the program is running.
				printf("End of file/stream was reached.\n");
			}
			else // some other error
				perror("fgets");

			exit(EXIT_SUCCESS);
		}
		printf("Echo: %s\n", s);
	}
}
