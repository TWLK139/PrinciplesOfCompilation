#pragma once
#ifndef LEXICALANALYZER_H

#define LEXICALANALYZER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <string>

using namespace std;

// ָ����Ԫ�����еĵ����ֱ�
#define ERROR_NUM           0       // ���ʴ���
#define KEYWORD_NUM         1       // ϵͳ�ؼ���
#define SEPARATWORD_NUM     2       // �ָ���
#define OPERAWORD_NUM       3       // �����
#define RELATWORD_NUM       4       // ��ϵ�����
#define CONSTANT_NUM        5       // ����
#define IDENTIFIER_NUM      6       // ��ʶ��
// ���嵥���ֱ����/Ӣ������
string TRAN_SYMBOLS[] = { "Error", "�ؼ���", "�ֽ��", "�����", "��ϵ�����", "����", "��ʶ��" };

// �ʷ�������
class LexicalAnalyzer
{
public:
    LexicalAnalyzer(const char* fileDir);                   // ���캯��������ϵͳ�ļ�����ȡ�ؼ��ֱ��ֽ�������������ϵ�������
    ~LexicalAnalyzer();

    void analysis(const char* fileName);                    // ������������ָ���ļ����дʷ�����
    void printResult();                                     // ��ӡ�ʷ���������ĺ���
private:
    set<string> keyWord;                                    // ����ϵͳ�ؼ��ֵļ���
    set<string> separatWord;                                // ����ָ����ļ���
    set<string> operaWord;                                  // ����������ļ���
    set<string> relatWord;                                  // �����ϵ������ļ���

    set<char> operaChar;                                    // ��������������õ��ķ���
    set<char> relatChar;                                    // �����ϵ����������õ��ķ���
    set<char> separatChar;                                  // ����ֽ�������õ��ķ���

    typedef struct wordBinarySequence                       // ����洢�ṹ
    {
        int type;                                           // �������ͱ��
        string word;                                        // ��������
        int row;                                            // ����������
        int col;                                            // ����������
    } wordBS;
    vector<wordBS> binary;                                  // ������е��ʵ�����


    bool analysisWord(char* s, int& ptr, int row);          // ������ĸ��ͷ�ĵ���        ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������
    bool analysisNumber(char* s, int& ptr, int row);        // �������ֿ�ͷ�ĵ���        ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������
    bool analysisOperator(char* s, int& ptr, int row);      // �����������ͷ�ĵ���      ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������
    bool analysisRelation(char* s, int& ptr, int row);      // ������ϵ�������ͷ�ĵ���  ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������
    bool analysisSeparator(char* s, int& ptr, int row);     // �����ֽ����ͷ�ĵ���      ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������
    bool analysisError(char* s, int& ptr, int row);         // ������Ч�ַ���ͷ�ĵ���    ��Դ�ļ��е�һ���ַ������ʿ�ʼλ�á���������������

    bool isWordChar(char c);                                // �ж�һ���ַ��Ƿ��ǵ�����ĸ�ַ�
    bool isNumberChar(char c);                              // �ж�һ���ַ��Ƿ��ǵ��������ַ�
    bool isOperatorChar(char c);                            // �ж�һ���ַ��Ƿ��ǵ���������ַ�
    bool isRelationChar(char c);                            // �ж�һ���ַ��Ƿ��ǵ�����ϵ������ַ�
    bool isSeparatorChar(char c);                           // �ж�һ���ַ��Ƿ��ǵ����ָ����ַ�

    string cutString(char* s, int begin, int end);          // ��һ���ַ���������ȡָ����Χ�����ַ���������beginλ�ã�������endλ�ã�

    bool isSpace(char c);                                   // �ж�һ���ַ��Ƿ��ǿո��
    bool isKeyWord(string s);                               // �ж��ַ����Ƿ���ϵͳ�ؼ���
    bool isOperator(string s);                              // �ж��ַ����Ƿ��ǺϷ������
    bool isRelation(string s);                              // �ж��ַ����Ƿ��ǺϷ���ϵ�����

    void addBinary(int type, string s, int row, int col);   // ���������һ���ʷ�����������������͡��������ݡ����������С����������У�
};

LexicalAnalyzer::LexicalAnalyzer(const char* fileDir = "sign/")     // ���캯����Ĭ�ϴ�./sign/Ŀ¼��ȡϵͳ�����ļ�
{
    ifstream file;
    char s[80];

    file.open(((string)fileDir).append("keyWord.sign"), ios::in);   // ��ȡ�ؼ����ļ�
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            keyWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("operaWord.sign"), ios::in); // ��ȡ������ļ�
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // �ֽ���ַ������뼯��
            {
                operaChar.insert(s[i]);
            }
            operaWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("relatWord.sign"), ios::in); // ��ȡ��ϵ������ļ�
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // �ֽ���ַ������뼯��
            {
                relatChar.insert(s[i]);
            }
            relatWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("separatWord.sign"), ios::in);// ��ȡ�ָ����ļ�
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // �ֽ���ַ������뼯��
            {
                separatChar.insert(s[i]);
            }
            separatWord.insert((string)s);
        }
    }
    file.close();
}

LexicalAnalyzer::~LexicalAnalyzer()
{
}

void LexicalAnalyzer::analysis(const char* fileName = "program/main.cpp")   // ����Դ���룬Ĭ��Դ�����ļ�Ϊ./program/main.cpp
{
    ifstream file;
    char s[1000];
    int ptr = 0;
    int row = 0;

    file.open(fileName, ios::in);
    if (file.is_open())
    {
        while (file.getline(s, 1000))
        {
            row++;                                      // ��ȡ��ǰ�к�
            for (ptr = 0; s[ptr] != '\0';)
            {
                if (isSpace(s[ptr]))                    // Ϊ�ո����ֱ�ӷ�����һ�ַ�
                {
                    ptr++;
                    continue;
                }
                else if (isWordChar(s[ptr]))            // ����ĸ��������ĸ��ͷ���ʷ���
                {
                    analysisWord(s, ptr, row);
                }
                else if (isNumberChar(s[ptr]))          // �����֣��������ֿ�ͷ���ʷ���
                {
                    analysisNumber(s, ptr, row);
                }
                else if (isOperatorChar(s[ptr]))        // ��������������������ͷ���ʷ���
                {
                    analysisOperator(s, ptr, row);
                }
                else if (isRelationChar(s[ptr]))        // �ǹ�ϵ����������ù�ϵ�������ͷ���ʷ���
                {
                    analysisRelation(s, ptr, row);
                }
                else if (isSeparatorChar(s[ptr]))       // �Ƿֽ�������÷ֽڷ���ͷ���ʷ���
                {
                    analysisSeparator(s, ptr, row);
                }
                else                                    // �ǷǷ��ַ������÷Ƿ��ַ���ͷ���ʷ���
                {
                    analysisError(s, ptr, row);
                }
            }
        }
    }
    file.close();
}

bool LexicalAnalyzer::analysisWord(char* s, int& ptr, int row)                      // ��ĸ��ͷ���ʷ���
{
    int begin = ptr;
    string tem;

    for (; (isWordChar(s[ptr]) || isNumberChar(s[ptr])) && s[ptr] != '\0'; ptr++);  // �����л�����ڱ�ʶ�����ؼ��֣��е��ַ�����ĸ�����֣�������
                                                                                    // �жϸõ��ʺ�ĵ�һ���ַ��Ƿ�Ϸ�
    if (isOperatorChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                                // �Ϸ������ж��Ƿ���ϵͳ�ؼ��֣����ݽ���ֱ��¼
        tem = cutString(s, begin, ptr);                                             // ���������ַ���ȡ�����ַ���
        if (isKeyWord(tem))
        {
            addBinary(KEYWORD_NUM, tem, row, begin);                                // ��¼Ϊ�ؼ���
        }
        else
        {
            addBinary(IDENTIFIER_NUM, tem, row, begin);                             // ��¼Ϊ��ʶ��
        }

        return true;
    }
    else
    {                                                                               // �Ƿ��ַ�����������ֱ���������зָ�������ַ������������ϵ��������ֽ�����ո����
        for (; !isOperatorChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // ��ȡ�����Ƿ��ַ���
        addBinary(ERROR_NUM, tem, row, begin);                                      // ��¼����

        return false;
    }
}

bool LexicalAnalyzer::analysisNumber(char* s, int& ptr, int row)                    // ���ֿ�ͷ�ĵ��ʷ���
{
    int begin = ptr;
    string tem;
    bool dot = false;                                                               // ��¼С�����Ƿ���ֹ�

    for (; (isNumberChar(s[ptr]) || (s[ptr] == '.' && !dot)) && s[ptr] != '\0'; ptr++)
    {
        if (s[ptr] == '.')                                                          // ֻ����С�������һ��
        {
            dot = true;
        }
    }
    // �жϸõ��ʺ�ĵ�һ���ַ��Ƿ�Ϸ�
    if (isOperatorChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {
        tem = cutString(s, begin, ptr);                                             // ���������ַ���ȡ�����ַ���
        addBinary(CONSTANT_NUM, tem, row, begin);                                   // ��¼Ϊ����

        return true;
    }
    else
    {                                                                               // �Ƿ��ַ�����������ֱ���������зָ�������ַ������������ϵ��������ֽ�����ո����
        for (; !isOperatorChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // ��ȡ�����Ƿ��ַ���
        addBinary(ERROR_NUM, tem, row, begin);                                      // ��¼Ϊ�Ƿ�

        return false;
    }
}

bool LexicalAnalyzer::analysisOperator(char* s, int& ptr, int row)                  // �������ͷ�ĵ��ʷ���
{
    int begin = ptr;
    string tem;

    for (; isOperatorChar(s[ptr]) && s[ptr] != '\0'; ptr++);                        // ������������������ַ������ַ���
                                                                                    // �жϸõ��ʺ�ĵ�һ���ַ��Ƿ�Ϸ�
    if (isWordChar(s[ptr]) || isNumberChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                               // �����ַ����Ϸ�������Ҫ�ж������ַ����Ƿ��ǺϷ������
        tem = cutString(s, begin, ptr);                                             // ���������ַ���ȡ�����ַ���
        if (isOperator(tem))
        {
            addBinary(OPERAWORD_NUM, tem, row, begin);                              // ��¼Ϊ�����

            return true;
        }
        else
        {
            addBinary(ERROR_NUM, tem, row, begin);                                  // ��¼�Ƿ�

            return false;
        }
    }
    else
    {                                                                               // �Ƿ��ַ�����������ֱ���������зָ�������ַ������������ϵ��������ֽ�����ո���ȣ�
        for (; !isWordChar(s[ptr]) && !isNumberChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // ��ȡ�����Ƿ��ַ���
        addBinary(ERROR_NUM, tem, row, begin);                                      // ��¼�Ƿ�

        return false;
    }
}

bool LexicalAnalyzer::analysisRelation(char* s, int& ptr, int row)                  // ��ϵ�������ͷ�ĵ��ʷ���
{
    int begin = ptr;
    string tem;

    for (; isRelationChar(s[ptr]) && s[ptr] != '\0'; ptr++);                        // ������������������ַ������ַ���
                                                                                    // �жϸõ��ʺ�ĵ�һ���ַ��Ƿ�Ϸ�
    if (isWordChar(s[ptr]) || isNumberChar(s[ptr]) || isOperatorChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                               // �����ַ����Ϸ�������Ҫ�ж������ַ����Ƿ��ǺϷ������
        tem = cutString(s, begin, ptr);                                             // ���������ַ���ȡ�����ַ���
        if (isRelation(tem))
        {
            addBinary(RELATWORD_NUM, tem, row, begin);                              // ��¼Ϊ��ϵ�������

            return true;
        }
        else
        {
            addBinary(ERROR_NUM, tem, row, begin);                                  // ��¼Ϊ�Ƿ�

            return false;
        }
    }
    else
    {                                                                               // �Ƿ��ַ�����������ֱ���������зָ�������ַ������������ϵ��������ֽ�����ո���ȣ�
        for (; !isWordChar(s[ptr]) && !isNumberChar(s[ptr]) && !isOperatorChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // ��ȡ�����Ƿ��ַ���
        addBinary(ERROR_NUM, tem, row, begin);                                      // ��¼�Ƿ�

        return false;
    }
}

bool LexicalAnalyzer::analysisSeparator(char* s, int& ptr, int row)                 // �����ֽڷ���ͷ����
{
    int begin = ptr;
    string tem;

    if (isSeparatorChar(s[ptr]))                                                    // Ŀǰ�ֽ����Ϊ���ַ����Ҳ��漰������ַ����ɷǷ��ַ����������ֱ�Ӽ�¼�������ǵ����ܻ��������ط����ã����������ж�
    {
        ptr += 1;
        tem = cutString(s, begin, ptr);                                             // �������ַ�ת���ַ��������м�¼
        addBinary(SEPARATWORD_NUM, tem, row, begin);

        return true;
    }
    else
    {
        ptr += 1;
        tem = cutString(s, begin, ptr);
        addBinary(ERROR_NUM, tem, row, begin);

        return false;
    }
}

bool LexicalAnalyzer::analysisError(char* s, int& ptr, int row)                     // �Ƿ��ַ���ͷ�ĵ��ʷ���
{
    int begin = ptr;
    string tem;
    // ������ֱ���������зָ�������ַ������������ϵ��������ֽ�����ո���ȣ�
    for (; !isSpace(s[ptr]) && !isSeparatorChar(s[ptr]) && !isOperatorChar(s[ptr]) && s[ptr] != '\0'; ptr++);
    tem = cutString(s, begin, ptr);
    addBinary(ERROR_NUM, tem, row, begin);

    return false;
}

bool LexicalAnalyzer::isWordChar(char c)
{                                                                                   // �ж�һ���ַ��Ƿ�����ĸ
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isNumberChar(char c)
{                                                                                   // �ж�һ���ַ��Ƿ�������
    if (c >= '0' && c <= '9')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isOperatorChar(char c)
{                                                                                   // �ж�һ���ַ��Ƿ����������������ɹ��캯����ã�
    if (operaChar.find(c) != operaChar.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isRelationChar(char c)
{                                                                                   // �ж�һ���ַ��Ƿ��ǹ�ϵ���������ϵ������ɹ��캯����ã�
    if (relatChar.find(c) != relatChar.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isSeparatorChar(char c)
{                                                                                   // �ж�һ���ַ��Ƿ��Ƿֽ�����ֽ���ɹ��캯����ã�
    if (separatChar.find(c) != separatChar.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

string LexicalAnalyzer::cutString(char* s, int begin, int end)
{                                                                                  // ��char[]�ַ������ȡ�����ַ�ת��string�����ַ���
    char tem[1000];
    int i = 0;

    for (; (i + begin) < end; i++)
    {
        tem[i] = s[i + begin];
    }
    tem[i] = '\0';

    return (string)tem;
}

bool LexicalAnalyzer::isSpace(char c)
{                                                                                   // �ж�һ���ַ��Ƿ��ǿո��
    if (c == ' ' || c == '\t' || c == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isKeyWord(string s)
{                                                                                   // �ж�һ���ַ����Ƿ��ǹؼ��֣��ؼ����ɹ��캯�����ļ���ã�
    if (keyWord.find(s) != keyWord.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isOperator(string s)
{                                                                                   // �ж�һ���ַ����Ƿ����������������ɹ��캯�����ļ���ã�
    if (operaWord.find(s) != operaWord.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LexicalAnalyzer::isRelation(string s)
{                                                                                   // �ж�һ���ַ����Ƿ��ǹ�ϵ���������ϵ������ɹ��캯�����ļ���ã�
    if (relatWord.find(s) != relatWord.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LexicalAnalyzer::addBinary(int type, string s, int row, int col)               // ������¼�������һ������
{
    wordBS tem = {
        type,
        s,
        row,
        col + 1
    };

    binary.push_back(tem);
}

void LexicalAnalyzer::printResult()
{                                                                                   // ��ӡ��Ԫ����
    cout << "\t����\t" << "\t��Ԫ����" << "\t����\t" << "\tλ��(��,��)" << endl;
    for (int i = 0; i < binary.size(); i++)
    {                                                                               // ͨ�����ݳ��ȼ��ж���Ҫ���Ʊ������
        cout << "\t" << binary[i].word;
        if (binary[i].word.length() < 8) cout << "\t\t";
        else if (binary[i].word.length() < 16) cout << "\t";
        cout << "(" << binary[i].type << "," << binary[i].word << ")";
        if (binary[i].word.length() < 4) cout << "\t\t";
        else if (binary[i].word.length() < 11) cout << "\t";
        cout << TRAN_SYMBOLS[binary[i].type];
        if (TRAN_SYMBOLS[binary[i].type].length() < 8) cout << "\t\t";
        else if (TRAN_SYMBOLS[binary[i].type].length() < 16) cout << "\t";
        cout << "(" << binary[i].row << "," << binary[i].col << ")" << endl;
    }
}
#endif // !LEXICALANALYZER_H