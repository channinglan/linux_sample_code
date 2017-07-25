#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////////

#define SHELL_CMD_SPACE	10

#define SHELL_ARGV_SPACE	10

//////////////////////////////////////////////////////////////////////////

typedef int (*CCmd)(int argc, char* argv[]);

typedef struct{
	char*	Name;
	CCmd	CmdTable;
}CCmdTable;

//////////////////////////////////////////////////////////////////////////

int Cmd1(int argc, char* argv[]);
int Cmd2(int argc, char* argv[]);
int InitModel(int argc, char* argv[]);


//////////////////////////////////////////////////////////////////////////

// 命令列表

CCmdTable CmdTable[SHELL_CMD_SPACE]=
{
	{"cmd1", Cmd1},
	{"cmd2", Cmd2},
};

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])

{

while(1)

{

char str[128];

// 处理输入错误

if (cin.fail())

{

cin.clear();

}

// 获得输入命令

cout<<"Shell:>";

cin>>str;

// 如果命令为退出命令，在单片机等程序中，不应退出

if (0==strcmp(str, "quit"))

{

break;

}

// 搜索表中的所有命令，找出与输入命令匹配的并执行

for (int i=0; i<SHELL_CMD_SPACE; i++)

{

// 无命令则提前返回

if (CmdTable[i].Name==NULL) continue;

// 如果命令正确

if (0==strcmp(str, CmdTable[i].Name))

{

// 获取输入参数

cin.get(str, 128, '\n');

fflush(stdin);	// 及时清空输入流

char* t_argv[SHELL_ARGV_SPACE];

int j=0;

t_argv[0] = strtok (str," ");	// 分割输入参数

while(t_argv[j++]!=NULL)

{

t_argv[j] = strtok(NULL," ");

}

try	// 执行命令

{

int t_return = CmdTable[i].CmdTable(j-1, t_argv);

cout<<"命令退出，返回["<<t_return<<"]"<<endl;

}

catch (...)	// 捕获异常，维持稳定性

{

cout<<"捕获到异常，返回主程序..."<<endl;

}

break;

}

}

// 命令错误提示

if (i==SHELL_CMD_SPACE)

{

fflush(stdin);

cout<<"未搜索到命令："<<str<<endl;

// 添加默认操作

}

}

return 0;

}

//////////////////////////////////////////////////////////////////////////

int Cmd1(int argc, char* argv[])
{
	cout<<"Cmd1"<<endl;

	cout<<argc<<endl;

	for (int i=0; i<argc; i++)

	{

		cout<<argv[i]<<endl;

	}

	return 1;
}

int Cmd2(int argc, char* argv[])
{
	cout<<"Cmd2"<<endl;

	return 2;
}

