#pragma once
#ifndef LEXICALANALYZER_H

#define LEXICALANALYZER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <string>

using namespace std;

// 指定二元序列中的单词种别
#define ERROR_NUM           0       // 单词错误
#define KEYWORD_NUM         1       // 系统关键字
#define SEPARATWORD_NUM     2       // 分隔符
#define OPERAWORD_NUM       3       // 运算符
#define RELATWORD_NUM       4       // 关系运算符
#define CONSTANT_NUM        5       // 常数
#define IDENTIFIER_NUM      6       // 标识符
// 定义单词种别的中/英文名称
string TRAN_SYMBOLS[] = { "Error", "关键字", "分界符", "运算符", "关系运算符", "常数", "标识符" };

// 词法分析器
class LexicalAnalyzer
{
public:
    LexicalAnalyzer(const char* fileDir);                   // 构造函数，分析系统文件，获取关键字表、分界符表、运算符表、关系运算符表
    ~LexicalAnalyzer();

    void analysis(const char* fileName);                    // 分析函数，对指定文件进行词法分析
    void printResult();                                     // 打印词法分析结果的函数
private:
    set<string> keyWord;                                    // 保存系统关键字的集合
    set<string> separatWord;                                // 保存分隔符的集合
    set<string> operaWord;                                  // 保存运算符的集合
    set<string> relatWord;                                  // 保存关系运算符的集合

    set<char> operaChar;                                    // 保存运算符中所用到的符号
    set<char> relatChar;                                    // 保存关系运算符中所用到的符号
    set<char> separatChar;                                  // 保存分界符中所用到的符号

    typedef struct wordBinarySequence                       // 结果存储结构
    {
        int type;                                           // 单词类型编号
        string word;                                        // 单词内容
        int row;                                            // 单词所在行
        int col;                                            // 单词所在列
    } wordBS;
    vector<wordBS> binary;                                  // 存放所有单词的数组


    bool analysisWord(char* s, int& ptr, int row);          // 分析字母开头的单词        （源文件中的一行字符、单词开始位置、单词所在行数）
    bool analysisNumber(char* s, int& ptr, int row);        // 分析数字开头的单词        （源文件中的一行字符、单词开始位置、单词所在行数）
    bool analysisOperator(char* s, int& ptr, int row);      // 分析运算符开头的单词      （源文件中的一行字符、单词开始位置、单词所在行数）
    bool analysisRelation(char* s, int& ptr, int row);      // 分析关系运算符开头的单词  （源文件中的一行字符、单词开始位置、单词所在行数）
    bool analysisSeparator(char* s, int& ptr, int row);     // 分析分界符开头的单词      （源文件中的一行字符、单词开始位置、单词所在行数）
    bool analysisError(char* s, int& ptr, int row);         // 分析无效字符开头的单词    （源文件中的一行字符、单词开始位置、单词所在行数）

    bool isWordChar(char c);                                // 判断一个字符是否是单个字母字符
    bool isNumberChar(char c);                              // 判断一个字符是否是单个数字字符
    bool isOperatorChar(char c);                            // 判断一个字符是否是单个运算符字符
    bool isRelationChar(char c);                            // 判断一个字符是否是单个关系运算符字符
    bool isSeparatorChar(char c);                           // 判断一个字符是否是单个分隔符字符

    string cutString(char* s, int begin, int end);          // 从一个字符数组中提取指定范围构成字符串（包含begin位置，不包含end位置）

    bool isSpace(char c);                                   // 判断一个字符是否是空格符
    bool isKeyWord(string s);                               // 判断字符串是否是系统关键词
    bool isOperator(string s);                              // 判断字符串是否是合法运算符
    bool isRelation(string s);                              // 判断字符串是否是合法关系运算符

    void addBinary(int type, string s, int row, int col);   // 像结果中添加一条词法分析结果（单词类型、单词内容、单词所在行、单词所在列）
};

LexicalAnalyzer::LexicalAnalyzer(const char* fileDir = "sign/")     // 构造函数，默认从./sign/目录获取系统参数文件
{
    ifstream file;
    char s[80];

    file.open(((string)fileDir).append("keyWord.sign"), ios::in);   // 获取关键字文件
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            keyWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("operaWord.sign"), ios::in); // 获取运算符文件
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // 分解出字符，计入集合
            {
                operaChar.insert(s[i]);
            }
            operaWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("relatWord.sign"), ios::in); // 获取关系运算符文件
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // 分解出字符，计入集合
            {
                relatChar.insert(s[i]);
            }
            relatWord.insert((string)s);
        }
    }
    file.close();

    file.open(((string)fileDir).append("separatWord.sign"), ios::in);// 获取分隔符文件
    if (file.is_open())
    {
        while (file.getline(s, 80))
        {
            for (int i = 0; s[i] != '\0'; i++)                      // 分解出字符，计入集合
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

void LexicalAnalyzer::analysis(const char* fileName = "program/main.cpp")   // 分析源代码，默认源代码文件为./program/main.cpp
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
            row++;                                      // 获取当前行号
            for (ptr = 0; s[ptr] != '\0';)
            {
                if (isSpace(s[ptr]))                    // 为空格符则直接分析下一字符
                {
                    ptr++;
                    continue;
                }
                else if (isWordChar(s[ptr]))            // 是字母，调用字母开头单词分析
                {
                    analysisWord(s, ptr, row);
                }
                else if (isNumberChar(s[ptr]))          // 是数字，调用数字开头单词分析
                {
                    analysisNumber(s, ptr, row);
                }
                else if (isOperatorChar(s[ptr]))        // 是运算符，调用运算符开头单词分析
                {
                    analysisOperator(s, ptr, row);
                }
                else if (isRelationChar(s[ptr]))        // 是关系运算符，调用关系运算符开头单词分析
                {
                    analysisRelation(s, ptr, row);
                }
                else if (isSeparatorChar(s[ptr]))       // 是分界符，调用分节符开头单词分析
                {
                    analysisSeparator(s, ptr, row);
                }
                else                                    // 是非法字符，调用非法字符开头单词分析
                {
                    analysisError(s, ptr, row);
                }
            }
        }
    }
    file.close();
}

bool LexicalAnalyzer::analysisWord(char* s, int& ptr, int row)                      // 字母开头单词分析
{
    int begin = ptr;
    string tem;

    for (; (isWordChar(s[ptr]) || isNumberChar(s[ptr])) && s[ptr] != '\0'; ptr++);  // 讲所有会出现在标识符（关键字）中的字符（字母、数字）遍历完
                                                                                    // 判断该单词后的第一个字符是否合法
    if (isOperatorChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                                // 合法，则判断是否是系统关键字，根据结果分别记录
        tem = cutString(s, begin, ptr);                                             // 将遍历的字符截取生成字符串
        if (isKeyWord(tem))
        {
            addBinary(KEYWORD_NUM, tem, row, begin);                                // 记录为关键字
        }
        else
        {
            addBinary(IDENTIFIER_NUM, tem, row, begin);                             // 记录为标识符
        }

        return true;
    }
    else
    {                                                                               // 非法字符，继续遍历直到遇到具有分割意义的字符（运算符、关系运算符、分界符、空格符）
        for (; !isOperatorChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // 截取整个非法字符串
        addBinary(ERROR_NUM, tem, row, begin);                                      // 记录错误

        return false;
    }
}

bool LexicalAnalyzer::analysisNumber(char* s, int& ptr, int row)                    // 数字开头的单词分析
{
    int begin = ptr;
    string tem;
    bool dot = false;                                                               // 记录小数点是否出现过

    for (; (isNumberChar(s[ptr]) || (s[ptr] == '.' && !dot)) && s[ptr] != '\0'; ptr++)
    {
        if (s[ptr] == '.')                                                          // 只允许小数点出现一次
        {
            dot = true;
        }
    }
    // 判断该单词后的第一个字符是否合法
    if (isOperatorChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {
        tem = cutString(s, begin, ptr);                                             // 将遍历的字符截取生成字符串
        addBinary(CONSTANT_NUM, tem, row, begin);                                   // 记录为常数

        return true;
    }
    else
    {                                                                               // 非法字符，继续遍历直到遇到具有分割意义的字符（运算符、关系运算符、分界符、空格符）
        for (; !isOperatorChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // 截取整个非法字符串
        addBinary(ERROR_NUM, tem, row, begin);                                      // 记录为非法

        return false;
    }
}

bool LexicalAnalyzer::analysisOperator(char* s, int& ptr, int row)                  // 运算符开头的单词分析
{
    int begin = ptr;
    string tem;

    for (; isOperatorChar(s[ptr]) && s[ptr] != '\0'; ptr++);                        // 将所有连续的运算符字符归入字符串
                                                                                    // 判断该单词后的第一个字符是否合法
    if (isWordChar(s[ptr]) || isNumberChar(s[ptr]) || isRelationChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                               // 单个字符均合法，还需要判断整个字符串是否是合法运算符
        tem = cutString(s, begin, ptr);                                             // 将遍历的字符截取生成字符串
        if (isOperator(tem))
        {
            addBinary(OPERAWORD_NUM, tem, row, begin);                              // 记录为运算符

            return true;
        }
        else
        {
            addBinary(ERROR_NUM, tem, row, begin);                                  // 记录非法

            return false;
        }
    }
    else
    {                                                                               // 非法字符，继续遍历直到遇到具有分割意义的字符（运算符、关系运算符、分界符、空格符等）
        for (; !isWordChar(s[ptr]) && !isNumberChar(s[ptr]) && !isRelationChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // 截取整个非法字符串
        addBinary(ERROR_NUM, tem, row, begin);                                      // 记录非法

        return false;
    }
}

bool LexicalAnalyzer::analysisRelation(char* s, int& ptr, int row)                  // 关系运算符开头的单词分析
{
    int begin = ptr;
    string tem;

    for (; isRelationChar(s[ptr]) && s[ptr] != '\0'; ptr++);                        // 将所有连续的运算符字符归入字符串
                                                                                    // 判断该单词后的第一个字符是否合法
    if (isWordChar(s[ptr]) || isNumberChar(s[ptr]) || isOperatorChar(s[ptr]) || isSeparatorChar(s[ptr]) || isSpace(s[ptr]))
    {                                                                               // 单个字符均合法，还需要判断整个字符串是否是合法运算符
        tem = cutString(s, begin, ptr);                                             // 将遍历的字符截取生成字符串
        if (isRelation(tem))
        {
            addBinary(RELATWORD_NUM, tem, row, begin);                              // 记录为关系符运算符

            return true;
        }
        else
        {
            addBinary(ERROR_NUM, tem, row, begin);                                  // 记录为非法

            return false;
        }
    }
    else
    {                                                                               // 非法字符，继续遍历直到遇到具有分割意义的字符（运算符、关系运算符、分界符、空格符等）
        for (; !isWordChar(s[ptr]) && !isNumberChar(s[ptr]) && !isOperatorChar(s[ptr]) && !isSeparatorChar(s[ptr]) && !isSpace(s[ptr]) && s[ptr] != '\0'; ptr++);
        tem = cutString(s, begin, ptr);                                             // 截取整个非法字符串
        addBinary(ERROR_NUM, tem, row, begin);                                      // 记录非法

        return false;
    }
}

bool LexicalAnalyzer::analysisSeparator(char* s, int& ptr, int row)                 // 分析分节符开头单词
{
    int begin = ptr;
    string tem;

    if (isSeparatorChar(s[ptr]))                                                    // 目前分界符均为单字符，且不涉及与后续字符构成非法字符的情况，可直接记录，但考虑到可能会在其他地方调用，因此添加了判断
    {
        ptr += 1;
        tem = cutString(s, begin, ptr);                                             // 将单个字符转成字符串，进行记录
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

bool LexicalAnalyzer::analysisError(char* s, int& ptr, int row)                     // 非法字符开头的单词分析
{
    int begin = ptr;
    string tem;
    // 遍历，直到遇到具有分割意义的字符（运算符、关系运算符、分界符、空格符等）
    for (; !isSpace(s[ptr]) && !isSeparatorChar(s[ptr]) && !isOperatorChar(s[ptr]) && s[ptr] != '\0'; ptr++);
    tem = cutString(s, begin, ptr);
    addBinary(ERROR_NUM, tem, row, begin);

    return false;
}

bool LexicalAnalyzer::isWordChar(char c)
{                                                                                   // 判断一个字符是否是字母
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
{                                                                                   // 判断一个字符是否是数字
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
{                                                                                   // 判断一个字符是否是运算符（运算符由构造函数获得）
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
{                                                                                   // 判断一个字符是否是关系运算符（关系运算符由构造函数获得）
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
{                                                                                   // 判断一个字符是否是分界符（分界符由构造函数获得）
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
{                                                                                  // 从char[]字符数组截取部分字符转成string类型字符串
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
{                                                                                   // 判断一个字符是否是空格符
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
{                                                                                   // 判断一个字符串是否是关键字（关键字由构造函数读文件获得）
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
{                                                                                   // 判断一个字符串是否是运算符（运算符由构造函数读文件获得）
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
{                                                                                   // 判断一个字符串是否是关系运算符（关系运算符由构造函数读文件获得）
    if (relatWord.find(s) != relatWord.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LexicalAnalyzer::addBinary(int type, string s, int row, int col)               // 向结果记录数组添加一条数据
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
{                                                                                   // 打印二元序列
    cout << "\t单词\t" << "\t二元序列" << "\t类型\t" << "\t位置(行,列)" << endl;
    for (int i = 0; i < binary.size(); i++)
    {                                                                               // 通过内容长度简单判断需要的制表符数量
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