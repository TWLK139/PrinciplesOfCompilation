#ifndef LL1_H
#define LL1_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <string>

using namespace std;

#define NULL_WORD '@'           // 定义用@符号在程序中代替空字
#define ERROR_TABLE "error"     // 定义分析表中的错误标识
// 定义分析过程操作标识符
#define ERROR       0               // 错误标识
#define INIT        1               // 初始化标识
#define POP         2               // 出栈标识
#define POP_PUSHU   3               // 出栈、入栈标识
#define GETNEXT     4               // 取下一个标识
                                // 将分析过程标识转化为文字输出
string TRAN_SYMBOLS[] = { "ERROR", "初始化", "POP", "POP, PUSHU", "GETNEXT(I)" };

// 定义外部函数，字符串分割函数，将字符串s按照字符c分割成vector
vector<string> splitString(const string& s, char c);

class ParserLL1                 // LL1分析功能
{
public:
    ParserLL1(const char* fileName);                // 构造函数，读取文法定义文件，创建文法并求取FIRST、FOLLOW集，创建分析表
    ~ParserLL1();

    bool analysis(const char* fileName);            // 分析函数，读取句子文件，分析语法是否正确
    void printResult();                             // 打印结果
    void printTable();                              // 打印分析表
    void printFirst();
    void printFollow();
private:
    typedef struct
    {
        char source;
        string aim;
    } derType;                                      // 产生式数据结构
    vector<derType> derivation;                     // 记录产生式的数组

    typedef struct
    {
        set<char> first;
        set<char> follow;
    } VnMes;                                        // 非终结符数据结构，包含FIRST、FOLLOW集
    map<char, VnMes> Vn;                            // 非终结符的键值对，键：非终结符，值：FIRST、FOLLOW集
    set<char> Vt;                                   // 终结符集合
    map<char, map<char, derType>> analysisTable;    // 分析表结构

    typedef struct
    {
        int step;
        string analysis;
        string leave;
        string useDer;
        int operate;
    } resType;                                      // 结果记录数据结构
    vector<resType> LL1Result;                      // 结果记录数组

    void makeDerivationFromString(string s);        // 分析字符串创建产生式
    bool isVn(char c);                              // 判断一个字符是否是非终结符
    void makeVtFromDerivations();                   // 从产生式获取终结符
    void makeFirst();                               // 获取终结符的FIRST集
    void makeFollow();                              // 获取终结符的FOLLOW集
    void makeAnalysisTable();                       // 创建分析表
    string getStringFromVector(vector<char> vc);    // 从vector<char>字符串创建string字符串，用于记录分析结果时对分析栈获取字符串用于输出
                                                    // 记录结果，步骤标号、分析栈、剩余字符串、产生式、操作
    void recordResult(int step, string analysis, string leave, derType useDer, int operate);
};

ParserLL1::ParserLL1(const char* fileName = "grammar/demo.gram")
{
    ifstream file;
    char s[40];
    int line;

    file.open(fileName, ios::in);
    if (file.is_open())
    {
        line = 0;
        while (file.getline(s, 40))
        {
            makeDerivationFromString((string)s);
            set<char> temFirst;
            set<char> temFollow;
            VnMes tem = {
                temFirst,
                temFollow,
            };
            Vn.insert(map<char, VnMes>::value_type(s[0], tem));     // 记录非终结符，每个产生是的左侧为非终结符
            line++;
        }
    }
    file.close();

    makeVtFromDerivations();                                        // 从产生式获取非终结符
    makeFirst();                                                    // 创建FIRST集
    makeFollow();                                                   // 创建FOLLOW集
    makeAnalysisTable();                                            // 创建分析表
}

ParserLL1::~ParserLL1()
{
}

vector<string> splitString(const string& s, char c)                 // 分割字符串，把字符串按字符分割为字符数组
{
    vector<string> v;
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v;
}

void ParserLL1::makeDerivationFromString(string s)                  // 从字符串创建产生式
{
    char sour = s[0];
    vector<string> aim = splitString(s.substr(3), '|');

    for (int i = 0; i < aim.size(); i++)                           // 循环产生产生式
    {
        if (aim[i] == "ε")                                        // 将文法源文件中的空字替换为语法分析器使用的空字符号，便于处理
        {
            aim[i] = NULL_WORD;
        }
        derType tem = {
            sour,
            aim[i]
        };

        derivation.push_back(tem);
    }
}

bool ParserLL1::isVn(char c)
{
    return Vn.find(c) != Vn.end();                                  // 在非终结符集合中找不到即为终结符
}

void ParserLL1::makeVtFromDerivations()                             // 从产生式获取终结符集
{
    for (int i = 0; i < derivation.size(); i++)
    {
        for (int j = 0; j < derivation[i].aim.length(); j++)
        {
            if (!isVn(derivation[i].aim[j]) && derivation[i].aim[j] != NULL_WORD)   // 不是非终结符且不是空字
            {
                Vt.insert(derivation[i].aim[j]);
            }
        }
    }
}

void ParserLL1::makeFirst()                                                         // 创建First集
{
    for (int i = derivation.size() - 1; i >= 0; i--)                                // 倒序遍历所有产生式，依次添加First集元素
    {
        bool addEmpty = true;                                                       // 标记需要在First集中添加空字，会在下面的分析中对该标记进行有效修改

        for (int j = 0; j < derivation[i].aim.length(); j++)                        // 依次分析产生式右侧的字符，按照不同的规则添加不同内容
        {
            if (isVn(derivation[i].aim[j]))                                         // 如果是非终结符，要添加该非终结符的所有非空字First集元素，并根据是否可推出空字判断是否需要遍历下一个字符
            {
                                                                                    // 先标记当前非终结符的原First集中是否有空字，便于后文判断是否需要删除First集中新引入的空字
                bool flag = Vn[derivation[i].source].first.find(NULL_WORD) != Vn[derivation[i].source].first.end();
                                                                                    // 将产生式右侧的非终结符的所有非空字First集元素插入
                Vn[derivation[i].source].first.insert(Vn[derivation[i].aim[j]].first.begin(), Vn[derivation[i].aim[j]].first.end());

                if (!flag)                                                          // 根据实际情况判断是否需要删除空字
                {
                    Vn[derivation[i].source].first.erase(NULL_WORD);
                }
                                                                                    // 判断产生式右侧的非终结符是否能推出空字（First集中是否包含空字）决定是否需要分析下一个字符
                if (Vn[derivation[i].aim[j]].first.find(NULL_WORD) == Vn[derivation[i].aim[j]].first.end())
                {
                    addEmpty = false;                                               // 如果不包含空字，则标记不需要添加空字，且应当结束对该产生式的分析
                    break;
                }
            }
            else                                                                    // 如果不是非终结符（是终结符或空字）则只需要将该符号添加进First集即可，并应当结束对该产生式的分析
            {
                Vn[derivation[i].source].first.insert(derivation[i].aim[j]);
                addEmpty = false;
                break;
            }
        }

        if (addEmpty)                                                               // addEmpty为true的充要条件：产生式右侧的符号全都是非终结符，且整个产生式可以推出空字
        {
            Vn[derivation[i].source].first.insert(NULL_WORD);
        }
    }
}

void ParserLL1::makeFollow()                                                        // 创建Follow集
{
    Vn[derivation[0].source].follow.insert('#');                                    // 先向文法开始的Follow集中添加‘#’

    for (int i = 0; i < derivation.size(); i++)                                     // 顺序遍历所有产生式，分别分析每个终结符的Follow集
    {
        char tem = derivation[i].source;                                            // 记录当前应当分析的非终结符
        for (int j = 0; j < derivation.size(); j++)
        {
            int ci = derivation[j].aim.find(tem);
            if (ci != derivation[j].aim.npos)                                       // 获取产生式右侧包含当前分析的非终结符的产生式
            {
                bool addFollow = true;                                              // 标记需要添加该产生左侧非终结符的Follow集，该标记会在下文中根据实际情况做出有效修改
                for (int k = ci + 1; k < derivation[j].aim.length(); k++)           // 遍历产生式右侧的每一个符号
                {
                    if (isVn(derivation[j].aim[k]))                                 // 如果是非终结符，则需要添加该非终结符的所有First集，并判断是否需要继续遍历下一个字符
                    {
                                                                                    // 插入该非终结符的所有非空字First集元素
                        Vn[tem].follow.insert(Vn[derivation[j].aim[k]].first.begin(), Vn[derivation[j].aim[k]].first.end());
                        Vn[tem].follow.erase(NULL_WORD);
                                                                                    // 如果该非终结符不能推出空字，则应当结束该产生式的分析，并标记无需添加产生式左侧非终结符的Follow集
                        if (Vn[derivation[j].aim[k]].first.find(NULL_WORD) == Vn[derivation[j].aim[k]].first.end())
                        {
                            addFollow = false;
                            break;
                        }
                    }
                    else                                                            // 如果不是终结符，则只需向Follow集中添加该终结符，并结束对该产生式的分析，并标记无需添加产生式左侧非终结符的Follow集
                    {
                        Vn[tem].follow.insert(derivation[j].aim[k]);
                        addFollow = false;
                        break;
                    }
                }

                if (addFollow)                                                      // 需要添加产生式左侧非终结符的Follow集的充要条件：该非终结符的后面的字符都是非终结符或空字，且其后面的字符串能够通过零步或多步推出空字
                {
                    Vn[tem].follow.insert(Vn[derivation[j].source].follow.begin(), Vn[derivation[j].source].follow.end());
                }
            }
        }
    }
}

void ParserLL1::makeAnalysisTable()                                                 // 创建分析表
{
    derType error_der = {                                                           // 初始化分析表的全部被容均为ERROR
        '\0',
        ERROR_TABLE
    };
    for (vector<derType>::iterator dei = derivation.begin(); dei != derivation.end(); dei++)
    {
        for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)
        {
            analysisTable[(*dei).source][*vti] = error_der;
        }
        analysisTable[(*dei).source]['#'] = error_der;                              // 添加‘#’字段
    }

                                                                                    // 遍历所有产生式，根据产生式填写分析表
    for (vector<derType>::iterator dei = derivation.begin(); dei != derivation.end(); dei++)
    {
        for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)          // 遍历所有终结符，是否写入分析表的条件
        {
            bool proFollow = true;
            for (int i = 0; i < (*dei).aim.length(); i++)                           // 分析该产生式的首符集是否包含终结符
            {
                if (isVn((*dei).aim[i]))                                            // 如果字符是非终结符，需要判断其First集是否包含该终结符或空字
                {
                                                                                    // 包含该终结符，向表中写入相关数据，但仍要继续分析判断能否推出空字
                    if (Vn[(*dei).aim[i]].first.find(*vti) != Vn[(*dei).aim[i]].first.end())
                    {
                        analysisTable[(*dei).source][*vti] = *dei;
                    }                                                               // 只要不包含空字即结束本次分析
                    if (Vn[(*dei).aim[i]].first.find(NULL_WORD) == Vn[(*dei).aim[i]].first.end())
                    {
                        proFollow = false;
                        break;
                    }
                }                                                                   // 是非终结符，只需判断是否相等，相等则写入并结束分析，不相等则直接结束分析，同时标记无需对Follow集分析
                else if ((*dei).aim[i] == *vti)
                {
                    analysisTable[(*dei).source][*vti] = *dei;
                    proFollow = false;
                    break;
                }
            }
            if (proFollow)                                                          // 需要分析Follow集的充要条件：产生式右侧的字符均为非终结符或空字且其可以经过零次或多次推导出空字
            {
                for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)
                {
                    if (Vn[(*dei).source].follow.find(*vti) != Vn[(*dei).source].follow.end())
                    {
                        analysisTable[(*dei).source][*vti] = *dei;
                    }
                }
                                                                                    // 因为终结符集中没有‘#’，因此需要单独处理
                if (Vn[(*dei).source].follow.find('#') != Vn[(*dei).source].follow.end())
                {
                    analysisTable[(*dei).source]['#'] = *dei;
                }
            }
        }
    }
}

bool ParserLL1::analysis(const char* fileName = "program/main.la")                  // 分析控制程序，默认从./program/main.la获取句子
{
    ifstream file;
    string leave;
    vector<char> analysis;

    file.open(fileName, ios::in);                                                   // 从文件中取出一行句子
    if (file.is_open())
    {
        char tem[100];
        file.getline(tem, 100);
        leave = tem;
    }
    file.close();

    if (leave[leave.length() - 1] != '#')                                           // 对句子简单处理，添加‘#’
    {
        leave.push_back('#');
    }

    analysis.push_back('#');                                                        // 初始化分析栈
    analysis.push_back(derivation[0].source);
                                                                                    // 记录初始化操作
    recordResult(0, getStringFromVector(analysis), leave.substr(0), { '\0', "\0" }, INIT);

    int line = 0;
    int i = 0;                                                                      // 初始化行号
    while (i < leave.length())
    {
        line++;
        if (analysis[analysis.size() - 1] == leave[i])                              // 如果分析栈和输入栈的栈顶相等，则同时出栈
        {
            i++;
            analysis.pop_back();
            recordResult(line, getStringFromVector(analysis), leave.substr(i), { '\0',"\0" }, GETNEXT);
        }
        else                                                                        // 不相等则需要查表
        {
            derType tem = analysisTable[analysis[analysis.size() - 1]][leave[i]];
            if (tem.aim == ERROR_TABLE)                                             // 记录出错
            {
                recordResult(line, "ERROR", "ERROR", { '\0',"\0" }, ERROR);
                return false;
            }
            analysis.pop_back();                                                    // 否则分析栈需要出栈
            if (tem.aim[0] == NULL_WORD)                                            // 并对查表结果进行入栈操作
            {
                recordResult(line, getStringFromVector(analysis), leave.substr(i), tem, POP);
            }
            else
            {
                for (int j = tem.aim.length() - 1; j >= 0; j--)
                {
                    analysis.push_back(tem.aim[j]);
                }
                recordResult(line, getStringFromVector(analysis), leave.substr(i), tem, POP_PUSHU);
            }
        }
    }

    if (leave[i - 1] == '#')                                                        // 当剩余字符串为‘#’则说明分析成功
    {
        return true;
    }
    else
    {
        return false;
    }
}

string ParserLL1::getStringFromVector(vector<char> vc)                              // 辅助函数，从vector数组获取字符串
{
    string tem = "";
    for (int i = 0; i < vc.size(); i++)
    {
        tem.push_back(vc[i]);
    }

    return tem;
}

                                                                                    // 结果记录函数，依次为，步骤号、分析栈、剩余字符串栈、使用的产生式、操作类型
void ParserLL1::recordResult(int step, string analysis, string leave, derType useDer, int operate)
{
    string s = "\0";
    if (useDer.aim == "@")                                                          // 将程序中用的空字替换成输出用的空字
    {
        useDer.aim = "ε";
    }
    s = (useDer.aim.length() != 0) ? (s + useDer.source + "->" + useDer.aim) : "\0";// 将带有结构的产生式转换为字符串
    resType tem = {
        step,
        analysis,
        leave,
        s,
        operate
    };

    LL1Result.push_back(tem);
}

void ParserLL1::printResult()                                                       // 打印分析过程
{
    cout << endl << "分析结果如下：" << endl;
    cout << "\t步骤\t" << "\t分析栈\t" << "\t剩余串\t" << "\t产生式\t" << "\t动作" << endl;
    for (int i = 0; i < LL1Result.size(); i++)
    {                                                                               // 通过内容长度简单判断需要的制表符数量
        cout << "\t" << LL1Result[i].step << "\t\t";
        cout << LL1Result[i].analysis;
        if (LL1Result[i].analysis.length() < 8) cout << "\t\t";
        else if (LL1Result[i].analysis.length() < 16) cout << "\t";
        cout << LL1Result[i].leave;
        if (LL1Result[i].leave.length() < 8) cout << "\t\t";
        else if (LL1Result[i].leave.length() < 16) cout << "\t";
        cout << LL1Result[i].useDer;
        if (LL1Result[i].useDer.length() < 8) cout << "\t\t";
        else if (LL1Result[i].useDer.length() < 16) cout << "\t";
        cout << TRAN_SYMBOLS[LL1Result[i].operate];
        if (LL1Result[i].operate == POP_PUSHU)
        {
            cout << "(";
            for (int j = LL1Result[i].useDer.length() - 1; j >= 3; j--)
            {
                cout << LL1Result[i].useDer[j];
            }
            cout << ")";
        }
        cout << endl;
    }
}

void ParserLL1::printTable()                                                        // 打印分析表
{
    cout << endl << "创建分析表如下：" << endl;
    for (map<char, derType>::iterator ati = ((analysisTable.begin())->second).begin(); ati != ((analysisTable.begin())->second).end(); ati++)
    {
        cout << "\t" << ati->first;
    }
    cout << endl;
    for (map<char, map<char, derType>>::iterator ati = analysisTable.begin(); ati != analysisTable.end(); ati++)
    {
        cout << ati->first << "\t";
        for (map<char, derType>::iterator atj = (ati->second).begin(); atj != (ati->second).end(); atj++)
        {
            if ((atj->second).aim == ERROR_TABLE)
            {
                cout << atj->second.aim << "\t";
            }
            else if (atj->second.aim == "@")
            {
                cout << atj->second.source << "->ε" << "\t";
            }
            else
            {
                cout << atj->second.source << "->" << atj->second.aim << "\t";
            }
        }
        cout << endl;
    }
}

void ParserLL1::printFirst()                                                        // 打印First集
{
    cout << "非终结符的FIRST集为：" << endl;
    for (map<char, VnMes>::iterator vni = Vn.begin(); vni != Vn.end(); vni++)
    {
        cout << vni->first << "\t";
        for (set<char>::iterator fi = vni->second.first.begin(); fi != vni->second.first.end(); fi++)
        {
            cout << *fi << "\t";
        }
        cout << endl;
    }
}

void ParserLL1::printFollow()                                                       // 打印Follow集
{
    cout << "非终结符的FOLLOW集为：" << endl;
    for (map<char, VnMes>::iterator vni = Vn.begin(); vni != Vn.end(); vni++)
    {
        cout << vni->first << "\t";
        for (set<char>::iterator fi = vni->second.follow.begin(); fi != vni->second.follow.end(); fi++)
        {
            cout << *fi << "\t";
        }
        cout << endl;
    }
}
#endif // !LL1_H