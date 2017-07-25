#include<iostream>
#include<cmath>
#include<vector>
#include<iomanip>
using namespace std;
#define rep(i,n) for(int i=1;i<=n;i++) //宏定义for循环，精简代码
class Matrix         //矩阵（或向量）类
{
	public:
		vector< vector<double> > a; //二维向量写法
		//double **b;    //二维指针写法，只要将后文a改为b即可
		int row;   //行数
		int col;   //列数
		Matrix(int n,int m) //通过构造初始化
		{
			row=n;
			col=m;
			//b=new double *[row+1];   这三行为二维指针的申请空间，与二维向量差不多
			//rep(i,row)
			//b[i]=new double[col+1];
			a.resize(row+1); //申请行数为row+1，0号位不用
			rep(i,row)
				a[i].resize(col+1); //申请各行的列数
			rep(i,n)
				rep(j,m)
				a[i][j]=0; //每个元素初始化为0，方便后面计算
		}
		void operator=(Matrix &v1) ;   //赋值重载
		friend Matrix operator*(Matrix &v1,Matrix &v2); // '*'号重载
};
void display(Matrix v)   //输出
{
	rep(i,v.row)
	{
		rep(j,v.col)
			cout<<setw(8)<<v.a[i][j]<<' ';
		cout<<'\n';
	}
}
void Matrix::operator=(Matrix &v1) //赋值定义
{
	rep(i,v1.row)
		rep(j,v1.col)
		a[i][j]=v1.a[i][j];
}
Matrix operator*(Matrix &v1,Matrix &v2) //乘法定义
{
	Matrix temp(v1.row,v2.col);
	rep(i,temp.row)
	{
		rep(j,temp.col)
		{
			double sum=0;
			rep(k,v1.col)
				sum+=v1.a[i][k]*v2.a[k][j];
			temp.a[i][j]=sum;
		}
	}
	return temp;
}
int main()
{
	int row1,row2,col1,col2;
	cout<<"请输入第一个矩阵的行和列:\n";
	cin>>row1>>col1;
	Matrix m1(row1,col1);
	cout<<"请输入"<<row1<<'*'<<col1<<"个数:\n";
	rep(i,row1)
		rep(j,col1)
		cin>>m1.a[i][j];
	cout<<"请输入第二个矩阵的行和列:\n";
	cin>>row2>>col2;
	Matrix m2(row2,col2);
	cout<<"请输入"<<row2<<'*'<<col2<<"个数:\n ";
	rep(i,row2)
		rep(j,col2)
		cin>>m2.a[i][j];
	if(col1!=row2)
		cout<<"这两个矩阵无法相乘\n";
	else
	{
		Matrix m3(row1,col2);
		m3=m1*m2;
		display(m3);
	}
	return 0;
}
