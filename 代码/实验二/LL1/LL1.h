#ifndef LL1_H
#define LL1_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <string>

using namespace std;

#define NULL_WORD '@'           // ������@�����ڳ����д������
#define ERROR_TABLE "error"     // ����������еĴ����ʶ
// ����������̲�����ʶ��
#define ERROR       0               // �����ʶ
#define INIT        1               // ��ʼ����ʶ
#define POP         2               // ��ջ��ʶ
#define POP_PUSHU   3               // ��ջ����ջ��ʶ
#define GETNEXT     4               // ȡ��һ����ʶ
                                // ���������̱�ʶת��Ϊ�������
string TRAN_SYMBOLS[] = { "ERROR", "��ʼ��", "POP", "POP, PUSHU", "GETNEXT(I)" };

// �����ⲿ�������ַ����ָ�������ַ���s�����ַ�c�ָ��vector
vector<string> splitString(const string& s, char c);

class ParserLL1                 // LL1��������
{
public:
    ParserLL1(const char* fileName);                // ���캯������ȡ�ķ������ļ��������ķ�����ȡFIRST��FOLLOW��������������
    ~ParserLL1();

    bool analysis(const char* fileName);            // ������������ȡ�����ļ��������﷨�Ƿ���ȷ
    void printResult();                             // ��ӡ���
    void printTable();                              // ��ӡ������
    void printFirst();
    void printFollow();
private:
    typedef struct
    {
        char source;
        string aim;
    } derType;                                      // ����ʽ���ݽṹ
    vector<derType> derivation;                     // ��¼����ʽ������

    typedef struct
    {
        set<char> first;
        set<char> follow;
    } VnMes;                                        // ���ս�����ݽṹ������FIRST��FOLLOW��
    map<char, VnMes> Vn;                            // ���ս���ļ�ֵ�ԣ��������ս����ֵ��FIRST��FOLLOW��
    set<char> Vt;                                   // �ս������
    map<char, map<char, derType>> analysisTable;    // ������ṹ

    typedef struct
    {
        int step;
        string analysis;
        string leave;
        string useDer;
        int operate;
    } resType;                                      // �����¼���ݽṹ
    vector<resType> LL1Result;                      // �����¼����

    void makeDerivationFromString(string s);        // �����ַ�����������ʽ
    bool isVn(char c);                              // �ж�һ���ַ��Ƿ��Ƿ��ս��
    void makeVtFromDerivations();                   // �Ӳ���ʽ��ȡ�ս��
    void makeFirst();                               // ��ȡ�ս����FIRST��
    void makeFollow();                              // ��ȡ�ս����FOLLOW��
    void makeAnalysisTable();                       // ����������
    string getStringFromVector(vector<char> vc);    // ��vector<char>�ַ�������string�ַ��������ڼ�¼�������ʱ�Է���ջ��ȡ�ַ����������
                                                    // ��¼����������š�����ջ��ʣ���ַ���������ʽ������
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
            Vn.insert(map<char, VnMes>::value_type(s[0], tem));     // ��¼���ս����ÿ�������ǵ����Ϊ���ս��
            line++;
        }
    }
    file.close();

    makeVtFromDerivations();                                        // �Ӳ���ʽ��ȡ���ս��
    makeFirst();                                                    // ����FIRST��
    makeFollow();                                                   // ����FOLLOW��
    makeAnalysisTable();                                            // ����������
}

ParserLL1::~ParserLL1()
{
}

vector<string> splitString(const string& s, char c)                 // �ָ��ַ��������ַ������ַ��ָ�Ϊ�ַ�����
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

void ParserLL1::makeDerivationFromString(string s)                  // ���ַ�����������ʽ
{
    char sour = s[0];
    vector<string> aim = splitString(s.substr(3), '|');

    for (int i = 0; i < aim.size(); i++)                           // ѭ����������ʽ
    {
        if (aim[i] == "��")                                        // ���ķ�Դ�ļ��еĿ����滻Ϊ�﷨������ʹ�õĿ��ַ��ţ����ڴ���
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
    return Vn.find(c) != Vn.end();                                  // �ڷ��ս���������Ҳ�����Ϊ�ս��
}

void ParserLL1::makeVtFromDerivations()                             // �Ӳ���ʽ��ȡ�ս����
{
    for (int i = 0; i < derivation.size(); i++)
    {
        for (int j = 0; j < derivation[i].aim.length(); j++)
        {
            if (!isVn(derivation[i].aim[j]) && derivation[i].aim[j] != NULL_WORD)   // ���Ƿ��ս���Ҳ��ǿ���
            {
                Vt.insert(derivation[i].aim[j]);
            }
        }
    }
}

void ParserLL1::makeFirst()                                                         // ����First��
{
    for (int i = derivation.size() - 1; i >= 0; i--)                                // ����������в���ʽ���������First��Ԫ��
    {
        bool addEmpty = true;                                                       // �����Ҫ��First������ӿ��֣���������ķ����жԸñ�ǽ�����Ч�޸�

        for (int j = 0; j < derivation[i].aim.length(); j++)                        // ���η�������ʽ�Ҳ���ַ������ղ�ͬ�Ĺ�����Ӳ�ͬ����
        {
            if (isVn(derivation[i].aim[j]))                                         // ����Ƿ��ս����Ҫ��Ӹ÷��ս�������зǿ���First��Ԫ�أ��������Ƿ���Ƴ������ж��Ƿ���Ҫ������һ���ַ�
            {
                                                                                    // �ȱ�ǵ�ǰ���ս����ԭFirst�����Ƿ��п��֣����ں����ж��Ƿ���Ҫɾ��First����������Ŀ���
                bool flag = Vn[derivation[i].source].first.find(NULL_WORD) != Vn[derivation[i].source].first.end();
                                                                                    // ������ʽ�Ҳ�ķ��ս�������зǿ���First��Ԫ�ز���
                Vn[derivation[i].source].first.insert(Vn[derivation[i].aim[j]].first.begin(), Vn[derivation[i].aim[j]].first.end());

                if (!flag)                                                          // ����ʵ������ж��Ƿ���Ҫɾ������
                {
                    Vn[derivation[i].source].first.erase(NULL_WORD);
                }
                                                                                    // �жϲ���ʽ�Ҳ�ķ��ս���Ƿ����Ƴ����֣�First�����Ƿ�������֣������Ƿ���Ҫ������һ���ַ�
                if (Vn[derivation[i].aim[j]].first.find(NULL_WORD) == Vn[derivation[i].aim[j]].first.end())
                {
                    addEmpty = false;                                               // ������������֣����ǲ���Ҫ��ӿ��֣���Ӧ�������Ըò���ʽ�ķ���
                    break;
                }
            }
            else                                                                    // ������Ƿ��ս�������ս������֣���ֻ��Ҫ���÷�����ӽ�First�����ɣ���Ӧ�������Ըò���ʽ�ķ���
            {
                Vn[derivation[i].source].first.insert(derivation[i].aim[j]);
                addEmpty = false;
                break;
            }
        }

        if (addEmpty)                                                               // addEmptyΪtrue�ĳ�Ҫ����������ʽ�Ҳ�ķ���ȫ���Ƿ��ս��������������ʽ�����Ƴ�����
        {
            Vn[derivation[i].source].first.insert(NULL_WORD);
        }
    }
}

void ParserLL1::makeFollow()                                                        // ����Follow��
{
    Vn[derivation[0].source].follow.insert('#');                                    // �����ķ���ʼ��Follow������ӡ�#��

    for (int i = 0; i < derivation.size(); i++)                                     // ˳��������в���ʽ���ֱ����ÿ���ս����Follow��
    {
        char tem = derivation[i].source;                                            // ��¼��ǰӦ�������ķ��ս��
        for (int j = 0; j < derivation.size(); j++)
        {
            int ci = derivation[j].aim.find(tem);
            if (ci != derivation[j].aim.npos)                                       // ��ȡ����ʽ�Ҳ������ǰ�����ķ��ս���Ĳ���ʽ
            {
                bool addFollow = true;                                              // �����Ҫ��Ӹò��������ս����Follow�����ñ�ǻ��������и���ʵ�����������Ч�޸�
                for (int k = ci + 1; k < derivation[j].aim.length(); k++)           // ��������ʽ�Ҳ��ÿһ������
                {
                    if (isVn(derivation[j].aim[k]))                                 // ����Ƿ��ս��������Ҫ��Ӹ÷��ս��������First�������ж��Ƿ���Ҫ����������һ���ַ�
                    {
                                                                                    // ����÷��ս�������зǿ���First��Ԫ��
                        Vn[tem].follow.insert(Vn[derivation[j].aim[k]].first.begin(), Vn[derivation[j].aim[k]].first.end());
                        Vn[tem].follow.erase(NULL_WORD);
                                                                                    // ����÷��ս�������Ƴ����֣���Ӧ�������ò���ʽ�ķ����������������Ӳ���ʽ�����ս����Follow��
                        if (Vn[derivation[j].aim[k]].first.find(NULL_WORD) == Vn[derivation[j].aim[k]].first.end())
                        {
                            addFollow = false;
                            break;
                        }
                    }
                    else                                                            // ��������ս������ֻ����Follow������Ӹ��ս�����������Ըò���ʽ�ķ����������������Ӳ���ʽ�����ս����Follow��
                    {
                        Vn[tem].follow.insert(derivation[j].aim[k]);
                        addFollow = false;
                        break;
                    }
                }

                if (addFollow)                                                      // ��Ҫ��Ӳ���ʽ�����ս����Follow���ĳ�Ҫ�������÷��ս���ĺ�����ַ����Ƿ��ս������֣����������ַ����ܹ�ͨ���㲽��ಽ�Ƴ�����
                {
                    Vn[tem].follow.insert(Vn[derivation[j].source].follow.begin(), Vn[derivation[j].source].follow.end());
                }
            }
        }
    }
}

void ParserLL1::makeAnalysisTable()                                                 // ����������
{
    derType error_der = {                                                           // ��ʼ���������ȫ�����ݾ�ΪERROR
        '\0',
        ERROR_TABLE
    };
    for (vector<derType>::iterator dei = derivation.begin(); dei != derivation.end(); dei++)
    {
        for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)
        {
            analysisTable[(*dei).source][*vti] = error_der;
        }
        analysisTable[(*dei).source]['#'] = error_der;                              // ��ӡ�#���ֶ�
    }

                                                                                    // �������в���ʽ�����ݲ���ʽ��д������
    for (vector<derType>::iterator dei = derivation.begin(); dei != derivation.end(); dei++)
    {
        for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)          // ���������ս�����Ƿ�д������������
        {
            bool proFollow = true;
            for (int i = 0; i < (*dei).aim.length(); i++)                           // �����ò���ʽ���׷����Ƿ�����ս��
            {
                if (isVn((*dei).aim[i]))                                            // ����ַ��Ƿ��ս������Ҫ�ж���First���Ƿ�������ս�������
                {
                                                                                    // �������ս���������д��������ݣ�����Ҫ���������ж��ܷ��Ƴ�����
                    if (Vn[(*dei).aim[i]].first.find(*vti) != Vn[(*dei).aim[i]].first.end())
                    {
                        analysisTable[(*dei).source][*vti] = *dei;
                    }                                                               // ֻҪ���������ּ��������η���
                    if (Vn[(*dei).aim[i]].first.find(NULL_WORD) == Vn[(*dei).aim[i]].first.end())
                    {
                        proFollow = false;
                        break;
                    }
                }                                                                   // �Ƿ��ս����ֻ���ж��Ƿ���ȣ������д�벢�����������������ֱ�ӽ���������ͬʱ��������Follow������
                else if ((*dei).aim[i] == *vti)
                {
                    analysisTable[(*dei).source][*vti] = *dei;
                    proFollow = false;
                    break;
                }
            }
            if (proFollow)                                                          // ��Ҫ����Follow���ĳ�Ҫ����������ʽ�Ҳ���ַ���Ϊ���ս�������������Ծ�����λ����Ƶ�������
            {
                for (set<char>::iterator vti = Vt.begin(); vti != Vt.end(); vti++)
                {
                    if (Vn[(*dei).source].follow.find(*vti) != Vn[(*dei).source].follow.end())
                    {
                        analysisTable[(*dei).source][*vti] = *dei;
                    }
                }
                                                                                    // ��Ϊ�ս������û�С�#���������Ҫ��������
                if (Vn[(*dei).source].follow.find('#') != Vn[(*dei).source].follow.end())
                {
                    analysisTable[(*dei).source]['#'] = *dei;
                }
            }
        }
    }
}

bool ParserLL1::analysis(const char* fileName = "program/main.la")                  // �������Ƴ���Ĭ�ϴ�./program/main.la��ȡ����
{
    ifstream file;
    string leave;
    vector<char> analysis;

    file.open(fileName, ios::in);                                                   // ���ļ���ȡ��һ�о���
    if (file.is_open())
    {
        char tem[100];
        file.getline(tem, 100);
        leave = tem;
    }
    file.close();

    if (leave[leave.length() - 1] != '#')                                           // �Ծ��Ӽ򵥴�����ӡ�#��
    {
        leave.push_back('#');
    }

    analysis.push_back('#');                                                        // ��ʼ������ջ
    analysis.push_back(derivation[0].source);
                                                                                    // ��¼��ʼ������
    recordResult(0, getStringFromVector(analysis), leave.substr(0), { '\0', "\0" }, INIT);

    int line = 0;
    int i = 0;                                                                      // ��ʼ���к�
    while (i < leave.length())
    {
        line++;
        if (analysis[analysis.size() - 1] == leave[i])                              // �������ջ������ջ��ջ����ȣ���ͬʱ��ջ
        {
            i++;
            analysis.pop_back();
            recordResult(line, getStringFromVector(analysis), leave.substr(i), { '\0',"\0" }, GETNEXT);
        }
        else                                                                        // ���������Ҫ���
        {
            derType tem = analysisTable[analysis[analysis.size() - 1]][leave[i]];
            if (tem.aim == ERROR_TABLE)                                             // ��¼����
            {
                recordResult(line, "ERROR", "ERROR", { '\0',"\0" }, ERROR);
                return false;
            }
            analysis.pop_back();                                                    // �������ջ��Ҫ��ջ
            if (tem.aim[0] == NULL_WORD)                                            // ���Բ����������ջ����
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

    if (leave[i - 1] == '#')                                                        // ��ʣ���ַ���Ϊ��#����˵�������ɹ�
    {
        return true;
    }
    else
    {
        return false;
    }
}

string ParserLL1::getStringFromVector(vector<char> vc)                              // ������������vector�����ȡ�ַ���
{
    string tem = "";
    for (int i = 0; i < vc.size(); i++)
    {
        tem.push_back(vc[i]);
    }

    return tem;
}

                                                                                    // �����¼����������Ϊ������š�����ջ��ʣ���ַ���ջ��ʹ�õĲ���ʽ����������
void ParserLL1::recordResult(int step, string analysis, string leave, derType useDer, int operate)
{
    string s = "\0";
    if (useDer.aim == "@")                                                          // ���������õĿ����滻������õĿ���
    {
        useDer.aim = "��";
    }
    s = (useDer.aim.length() != 0) ? (s + useDer.source + "->" + useDer.aim) : "\0";// �����нṹ�Ĳ���ʽת��Ϊ�ַ���
    resType tem = {
        step,
        analysis,
        leave,
        s,
        operate
    };

    LL1Result.push_back(tem);
}

void ParserLL1::printResult()                                                       // ��ӡ��������
{
    cout << endl << "����������£�" << endl;
    cout << "\t����\t" << "\t����ջ\t" << "\tʣ�മ\t" << "\t����ʽ\t" << "\t����" << endl;
    for (int i = 0; i < LL1Result.size(); i++)
    {                                                                               // ͨ�����ݳ��ȼ��ж���Ҫ���Ʊ������
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

void ParserLL1::printTable()                                                        // ��ӡ������
{
    cout << endl << "�������������£�" << endl;
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
                cout << atj->second.source << "->��" << "\t";
            }
            else
            {
                cout << atj->second.source << "->" << atj->second.aim << "\t";
            }
        }
        cout << endl;
    }
}

void ParserLL1::printFirst()                                                        // ��ӡFirst��
{
    cout << "���ս����FIRST��Ϊ��" << endl;
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

void ParserLL1::printFollow()                                                       // ��ӡFollow��
{
    cout << "���ս����FOLLOW��Ϊ��" << endl;
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