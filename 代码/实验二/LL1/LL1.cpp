#include "LL1.h"

int main(void)
{
	ParserLL1 pLL1;
	pLL1.printFirst();
	pLL1.printFollow();
	pLL1.printTable();

	bool flag = pLL1.analysis();

	pLL1.printResult();

	if (flag)
	{
		cout << "�﷨���������ȷ��" << endl;
	}
	else
	{
		cout << "�﷨�����������" << endl;
	}

	return 0;
}