#define NUM  1000
double A[1000][NUM];
double B[1000][NUM];
double C[1000][NUM];

void Muti_ijk(double A[][NUM], double B[][NUM], double C[][NUM], int n)
{
	int i, j, k;
	double sum = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {
			sum = 0.0;
			for (k = 0; k < n; k++)
				sum += A[i][k] * B[k][j];
			C[i][j] += sum;
		}
}

void Muti_jki(double A[][NUM], double B[][NUM], double C[][NUM], int n)
{
	int i, j, k;
	double sum = 0;
	for (j = 0; j < n; j++)
		for (k = 0; k < n; k++) {
			sum = B[k][j];
			for (i = 0; i < n; i++)
				C[i][j] += A[i][k] * sum;
		}
}

void Muti_kij(double A[][NUM], double B[][NUM], double C[][NUM], int n)
{
	int i, j, k;
	double sum = 0;
	for (k = 0; k < n; k++)
		for (i = 0; i < n; i++) {
			sum = A[i][k];
			for (j = 0; j < n; j++)
				C[i][j] += B[k][j] * sum;
		}
}

main()
{
	int i;
	volatile char a[1024*1024];

	for(i=0;i<1024*1024;i++) a[i]='a';
	(void)Muti_jki(A, B, C, 1000);

	for(i=0;i<1024*1024;i++) a[i]='a';
	(void)Muti_kij(A, B, C, 1000);
	
	for(i=0;i<1024*1024;i++) a[i]='a';
	(void)Muti_ijk(A, B, C, 1000);
}
