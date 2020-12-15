import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class LR1Window {
    private JFrame frame;
    private JPanel window;
    private JScrollPane analysisPanel;
    private JScrollPane resultPanel;

    private JTextField top_sentence;
    private JTextArea right_grammar;
    private JTable left_analysisTable;
    private JTable bottom_result;
    private JButton button_analysis;

    public LR1Window() {
        frame = new JFrame("LR(1) Analysis Program");
        window = new JPanel();
        JPanel sentencePanel = new JPanel();
        JScrollPane grammarPanel = new JScrollPane();
        resultPanel = new JScrollPane();
        analysisPanel = new JScrollPane();

        top_sentence = new JTextField();
        right_grammar = new JTextArea();
        left_analysisTable = new JTable();
        bottom_result = new JTable();
        button_analysis = new JButton("Analysis");

        frame.setResizable(false);
        frame.setSize(400, 500);
        frame.setContentPane(window);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);       //关闭
        frame.setVisible(true);                                     //显示窗口

        window.setLayout(new BorderLayout());
        sentencePanel.setPreferredSize(new Dimension(window.getWidth(), (int)(window.getHeight()*0.1)));
        sentencePanel.setLayout(new FlowLayout());
        grammarPanel.setPreferredSize(new Dimension(window.getWidth(), (int)(window.getHeight()*0.6)));
        resultPanel.setPreferredSize(new Dimension(window.getWidth(), (int)(window.getHeight()*0.3)));
        analysisPanel.setPreferredSize(new Dimension(0, (int)(window.getHeight()*0.6)));

        top_sentence.setPreferredSize(new Dimension((int)(window.getWidth()*0.7), (int)(window.getHeight()*0.08)));
        top_sentence.setMargin(new Insets(0, 10, 0, 10));
        button_analysis.setPreferredSize(new Dimension((int)(window.getWidth()*0.25), (int)(window.getHeight()*0.08)));
        right_grammar.setLineWrap(true);
        right_grammar.setMargin(new Insets(5,10,10,5));
        button_analysis.setBackground(new Color(255, 255,255));
        button_analysis.addActionListener(new AnalyzerActionListener());

        sentencePanel.add(top_sentence);
        sentencePanel.add(button_analysis);
        grammarPanel.setViewportView(right_grammar);
        resultPanel.setViewportView(bottom_result);
        analysisPanel.setViewportView(left_analysisTable);

        window.add(sentencePanel, BorderLayout.NORTH);
        window.add(grammarPanel, BorderLayout.WEST);
        window.add(analysisPanel, BorderLayout.EAST);
        window.add(resultPanel, BorderLayout.SOUTH);
    }


    public void setSentence(String sentence) {
        top_sentence.setText(sentence);
    }

    public void setGrammar(String grammar) {
        right_grammar.append(grammar);
    }

    class AnalyzerActionListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            LR1Analyzer analyzer = new LR1Analyzer(right_grammar.getText().replaceAll(LR1Analyzer.EMPTY_CHAR, LR1Analyzer.EMPTY_CHAR), top_sentence.getText().replaceAll(" ", ""));
            if (analyzer.analysisTableComplete) {
                Object[] table_a = analyzer.getAnalysisTable();
                SwingUtilities.invokeLater(()->{
                    frame.setSize(new Dimension(400 + ((String[])table_a[0]).length * 50, 700));
                    analysisPanel.setPreferredSize(new Dimension(((String[])table_a[0]).length * 50, (int)(window.getHeight()*0.6)));
                    left_analysisTable.setModel(new DefaultTableModel((String[][])(table_a[1]), (String[])(table_a[0])));
                    left_analysisTable.setEnabled(false);
                });
                if (analyzer.analysisResultComplete) {
                    Object[] table_r = analyzer.getResultTable();
                    SwingUtilities.invokeLater(()->{
                        resultPanel.setPreferredSize(new Dimension(window.getWidth(), (int)(window.getHeight()*0.3)+120));
                        bottom_result.setModel(new DefaultTableModel((String[][])(table_r[1]), (String[])(table_r[0])));
                        bottom_result.setEnabled(false);
                    });
                } else {
                    JOptionPane.showMessageDialog(frame, "句子分析失败，该句子不能用当前语法分析器分析。", "错误", JOptionPane.ERROR_MESSAGE);
                }
            } else {
                JOptionPane.showMessageDialog(frame, "语法分析失败，请检查语法是否是LR（1）的！","错误",JOptionPane.ERROR_MESSAGE);
            }
        }
    }
}
