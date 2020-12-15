/*
editor：     计算机科学与技术18-2班，牛洪亮，2018217692
function：   LR1语法分析器，初始化程序，创建窗口。
*/

import javax.swing.*;
import java.io.*;

public class LR1Initial {
    public final static String EMPTY_CHAR = "ε";                // 定义显示用的空字
    private final static String _EMPTY_CHAR = "��";            // 从文件中读取到的空字

    public static void main(String[] args) {
        SwingUtilities.invokeLater(()->{                         // 保证窗口稳定显示
            LR1Window lr = new LR1Window();                      // 创建分析其的窗口
            lr.setGrammar(readFile("grammar/demo.gram"));   // 从默认文件获取示例文法
            lr.setSentence("i+(i*i)+i");                              // 默认文法的测试句子
        });
    }

                                                                // 读文法文件的函数
    private static String readFile(String fileName) {
        try {
            String res = "";
            BufferedReader file = new BufferedReader(new InputStreamReader(new FileInputStream(new File(fileName))));
            String tem = file.readLine();
            while (tem != null) {
                tem = tem.replaceAll(" ", "");
                tem = tem.replaceAll(_EMPTY_CHAR, EMPTY_CHAR);  // 把文件中的空字用程序中空字替换
                res  = res + tem + "\n\r";
                tem = file.readLine();
            }
            return res;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }
}
