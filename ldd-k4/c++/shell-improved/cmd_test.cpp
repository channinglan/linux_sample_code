#include <string>  
#include <iostream>  
using namespace std;  
#include "cmd.h"  

class CWorkCmd: public CCmd  
{  
	public:  
		CWorkCmd(string cmdName):CCmd(cmdName)
		{}  
		int Run()  
		{  
			cout << "cmd " << m_strCmdName << endl;  
			return 0;  
		}  
};  

int main()  
{ 
	//单例模式
	CWorkCmd cmdObj("hello");  
	CShell *shell = CShell::GetInstance();

	shell->AddCmd(&cmdObj);
	shell->ParseCmd("hello");
	CWorkCmd cmdObj1("world");  
	shell->AddCmd(&cmdObj1);
	shell->ParseCmd("helloxxxx");
	return 0;  
}
