volatile unsigned char * const UART0_PTR = (unsigned char *)0x0101f1000;

void display(const char *string)
{
	while (*string != '\0') {
		*UART0_PTR = *string;
		string++;
	}
}

void my_init()
{
	while(1)
	display("Hello World\r\n");
}
