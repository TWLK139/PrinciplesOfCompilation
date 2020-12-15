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
		cout << "语法分析结果正确！" << endl;
	}
	else
	{
		cout << "语法分析结果错误！" << endl;
	}

	return 0;
}