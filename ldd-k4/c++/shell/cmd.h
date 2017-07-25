#include <vector>

class CCmd
{
protected:
	string m_strCmdName;
public:
	CCmd(){}
	CCmd(string cmdName)
	{
		this->m_strCmdName = cmdName;
	}
	string GetName()
	{
		return m_strCmdName;
	}
	virtual int Run()= 0;
};

class CShell
{
private:
	vector<CCmd*> m_veCCmdList;
public:
	int ParseCmd(string name);
	int AddCmd(CCmd *cmd);
};
