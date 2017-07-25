void int2hexstr(unsigned int val, unsigned char buf[])
{
	int i;
	for (i = 7; i >= 0; i--) {
		buf[i] = "0123456789ABCDEF"[val & 0x0F];
		val >>= 4; 
	} 
}

main()
{
	unsigned int a=0x12345678;
	char buf[9];
	buf[8] = '\0';
	int2hexstr(a, buf);

	printf("%s\n", buf);
}
