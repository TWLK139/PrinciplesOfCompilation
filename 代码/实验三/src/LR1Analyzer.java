/*
editor：     计算机科学与技术18-2班，牛洪亮，2018217692
function：   LR1语法分析器，语法分析类，创建分析表、分析指定句子
*/

import java.util.*;

public class LR1Analyzer {
    public final static String EMPTY_CHAR = "@";                // 用于分析的空字

    public boolean analysisTableComplete;                       // 标记分析表是否创建成功
    public boolean analysisResultComplete;                      // 标记分析句子是否成功
    HashMap<Character, TreeSet<Character>> Vn = new HashMap<>();// 非终结符结构集合
    HashSet<Character> Vt = new HashSet<>();                    // 终结符字符集合
    ArrayList<DerType> derivation = new ArrayList<>();          // 产生式结构链表
    ArrayList<ProjectListType> projects = new ArrayList<>();    // 项目集族
                                                                // 分析表
    LinkedHashMap<Integer, LinkedHashMap<Character, ActionGotoType>> analysisTable = new LinkedHashMap<>();
    LinkedList<AnalysisType> resultTable = new LinkedList<>();  // 分析过程记录表

    public LR1Analyzer(String grammar, String sentence) {       // 构造函数，创建非终结符、终结符、产生式、项目集族、分析表、并分析句子，记录分析过程
        String[] gras = grammar.split("\n\r");          // 获得文法字符串数组
        Vn.clear();
        Vn.put('$', new TreeSet<>());                           // 初始化非终结符
                                                                // 初始化产生式
        derivation.add(new DerType('$', grammar.substring(0, 1)));

        for (String gra : gras) {                               // 遍历每一个产生式，获取所有非终结符、记录所有产生式
            Vn.put(gra.charAt(0), new TreeSet<>());             // 获取终结符
                                                                // 临时变量，记录产生式右侧，便于按照“|”拆分成多个产生式，并非别处理
            String[] aims = (gra.substring(3)).split("\\|");
            for (String aim : aims) {                           // 分条记录所有产生式
                derivation.add(new DerType(gra.charAt(0), aim));
            }
        }
        makeVtFromDerivations();                                // 从产生获取终结符
        makeFirst();                                            // 创建First集
        makeProjectList();                                      // 创建项目集族
        analysisTableComplete = makeAnalysisTable();            // 创建分析表，记录创建是否成功
        if (analysisTableComplete) {                            // 若分析表创建成功，则开始分析句子
                                                                // 分析句子，记录分析是否成功
            analysisResultComplete = analysis(new StringBuilder(sentence));
        } else {                                                // 分析表创建失败，则直接置句子分析结果为错误
            analysisResultComplete = false;
        }
    }

    private boolean isVn(char v) {                              // 判断一个字符是否是非终结符
        return Vn.containsKey(v);
    }

    private void makeVtFromDerivations() {                      // 从产生式获取终结符
        for (DerType di : derivation) {
            for (int i = 0; i < di.value.length(); i++) {       // 遍历产生式中的字符
                if (!isVn(di.value.charAt(i))) {                // 如果字符不是非终结符
                    Vt.add(di.value.charAt(i));
                }
            }
        }
    }

    private void makeFirst() {                                  // 创建所有非终结符的First集
        for (int i = derivation.size() - 1; i >= 0; i--) {      // 遍历每个产生式
            boolean addEmpty = true;                            // 标记该产生是的终结符能否推导出空字
                                                                // 依次遍历该产生式可以推出的字符
            for (int j = 0; j < derivation.get(i).value.length(); j++) {
                if (isVn(derivation.get(i).value.charAt(j))) {  // 如果当前字符是非中介符，则将非终结符的First集添加进产生式左侧的非终结符的First集中
                                                                // 先记录产生式左侧的非终结符First集中是否已经包含空字，便于后续确定是否需要删除空字
                    boolean flag = Vn.get(derivation.get(i).key).contains(EMPTY_CHAR.charAt(0));
                                                                // 将First集添加
                    Vn.get(derivation.get(i).key).addAll(Vn.get(derivation.get(i).value.charAt(j)));
                    if (!flag) {                                // 如果A的First集中初始没有空字，则需删除合并后的集合中的空字
                        Vn.get(derivation.get(i).key).remove(EMPTY_CHAR.charAt(0));
                    }
                                                                // 如果当前产生式右侧的非终结符包含空字，则还需进一步分析下一个字符，不包含空字则可结束当前产生式左侧非终结符的First集分析
                    if (!(Vn.get(derivation.get(i).value.charAt(j)).contains(EMPTY_CHAR.charAt(0)))) {
                        addEmpty = false;
                        break;
                    }
                } else {                                        // 如果当前字符不是非终结符（终结符或空字）则直接向First集中添加该字符即可，并记录之后无需再添加空字
                    Vn.get(derivation.get(i).key).add(derivation.get(i).value.charAt(j));
                    addEmpty = false;
                    break;
                }
            }

            if (addEmpty) {                                     // 需额外添加空字的触发条件为：A-+->@，即产生式右侧的字符均为非终结符，且每个非终结符都可*退出空字
                Vn.get(derivation.get(i).key).add(EMPTY_CHAR.charAt(0));
            }
        }
    }

                                                                // 从字符串、展望符集构成的结构中获得First集
    private TreeSet<Character> getFirst(String s, TreeSet<Character> first) {
        TreeSet<Character> tem = new TreeSet<>();               // 建立临时存储
        boolean flag = true;                                    // 标记是否允许添加展望符集

        for (int i = 0; i < s.length(); i++) {                  // 遍历字符串的所有字符
            if (isVn(s.charAt(i))) {                            // 如果字符是非终结符
                tem.addAll(Vn.get(s.charAt(i)));                // 将该非终结符的First集中除了空字的字符添加到临时记录
                tem.remove(EMPTY_CHAR.charAt(0));
                                                                // 如果该非终结符的First集包含空字，则需要继续遍历下一个字符，否则可终止程序
                if (!Vn.get(s.charAt(i)).contains(EMPTY_CHAR.charAt(0))) {
                    flag = false;
                    break;
                }
            } else if (s.charAt(i) == EMPTY_CHAR.charAt(0)) {   // 如果该字符是空字，则可直接终止循环，并标记准备添加展望符集
                flag = true;                                    // 标记需添加展望符集
                break;
            } else {                                            // 如果是终结符，则可直接添加该终结符，且终止循环并禁止添加展望符集
                tem.add(s.charAt(i));
                flag = false;
                break;
            }
        }
        if (flag) {                                             // 如果允许添加展望符集
            tem.addAll(first);
        }

        return tem;
    }

    private void makeProjectList() {                            // 创建展望符集
        //*********************************************************创建一个只含[$->· $,#]临时的产生式集合，便于后续的自动分析，添加该产生式后可以自动生成第一个状态
        projects.add(new ProjectListType());                    // 向项目集族中添加一个新建的项目集
                                                                // 向第一个项目集中添加一个上述的项目（项目的圆点位置记为-1）
        projects.get(0).proLink.add(new ProjectType(derivation.get(0), 0, -1, new TreeSet<>()));
        projects.get(0).proLink.get(0).expectation.add('#');
        projects.get(0).next.put(derivation.get(0).value.charAt(0), -1);
        //*********************************************************
        for (int psi = 0; psi < projects.size(); psi++) {       // 项目集族中如果还有未遍历的项目集
                                                                // 则遍历项目的展望符集去产生新的项目集
            for (Map.Entry<Character, Integer> nmi : projects.get(psi).next.entrySet()) {
                ProjectListType temP = new ProjectListType();   // 新建临时项目集，便于项目集产生完成后判断是否可以添加入项目集族
                                                                // 遍历项目集中的每一个项目，且将包含当前正在分析的字符的项目添加进临时项目集，并将圆点前进一步
                for (ProjectType pti : projects.get(psi).proLink) {
                                                                // 两组判断条件，第一组为正常判断，第二组为项目[$->· $,#]的判断
                    if ((pti.pointer > -1 && pti.pointer < pti.derivation.value.length() && pti.derivation.value.charAt(pti.pointer) == nmi.getKey()) || (pti.pointer == -1 && pti.derivation.value.charAt(0) == nmi.getKey())) {
                                                                // 将前进一步的项目添加进新的项目集
                        temP.proLink.add(new ProjectType(pti.derivation, pti.identify, pti.pointer + 1, pti.expectation));
                                                                // 记录该项目的下一个待分析的字符
                        if (temP.proLink.get(temP.proLink.size() - 1).pointer < temP.proLink.get(temP.proLink.size() - 1).derivation.value.length()) {
                            temP.next.put(temP.proLink.get(temP.proLink.size() - 1).derivation.value.charAt(temP.proLink.get(temP.proLink.size() - 1).pointer), -1);
                        }
                    }
                }
                                                                // 遍历临时项目集，对该项目集进行自我展开
                for (int tpi = 0; tpi < temP.proLink.size(); tpi++) {
                                                                // 如果该产生式的圆点没有到达产生式尾部，则需进行处理
                    if (temP.proLink.get(tpi).pointer < temP.proLink.get(tpi).derivation.value.length()) {
                        char nextChar = temP.proLink.get(tpi).derivation.value.charAt(temP.proLink.get(tpi).pointer);
                        if (isVn(nextChar)) {                   // 圆点后面的符号为非终结符，则需要展开
                            for (int di = 0; di < derivation.size(); di++) {
                                                                // 遍历所有产生式，获取左侧符号为圆点后面的非终结符的产生式
                                if (derivation.get(di).key == nextChar) {
                                    String tem;
                                    if (temP.proLink.get(tpi).pointer + 1 >= temP.proLink.get(tpi).derivation.value.length()) {
                                        tem = EMPTY_CHAR;
                                    } else {
                                        tem = temP.proLink.get(tpi).derivation.value.substring(temP.proLink.get(tpi).pointer + 1);
                                    }                           // tem中存储了原产生式圆点后面的非终结符后的所有字符（若无，则空字）
                                                                // 利用以上信息创建一个临时项目
                                    ProjectType temp = new ProjectType(derivation.get(di), di, 0, getFirst(tem, temP.proLink.get(tpi).expectation));
                                    boolean hasP = false;       // 记录该项目是否已经存在于这个项目集中
                                    for (int hi = 0; hi < temP.proLink.size(); hi++) {
                                        if (temP.proLink.get(hi).toString().equals(temp.toString())) {
                                            hasP = true;
                                            break;
                                        }
                                    }
                                    if (!hasP) {                // 不存在，添加项目，并记录该项目带来的下一个需要处理的字符
                                        temP.proLink.add(temp);
                                        if (temP.proLink.get(temP.proLink.size() - 1).pointer < temP.proLink.get(temP.proLink.size() - 1).derivation.value.length()) {
                                            temP.next.put(temP.proLink.get(temP.proLink.size() - 1).derivation.value.charAt(temP.proLink.get(temP.proLink.size() - 1).pointer), -1);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                                                                // 对该项目集进行重新排序，便于确定项目集的唯一编号，用于判断该项目及是否已经存在于项目集族中
                Collections.sort(temP.proLink, new ComparatorProject());

                for (int i = 0; i < projects.size(); i++) {     // 判断项目集族中是否已经存在临时项目集
                    if (projects.get(i).toString().equals(temP.toString())) {
                        nmi.setValue(i);                        // 若存在，需要将记录好 i = go(I, a/A)
                        break;
                    }
                }                                               // 若不存在临时项目，则 i = go(I, a/A)的i依然为-1
                if (nmi.getValue() == -1) {                     // 如上所述，项目集族中不存在临时项目集，则记录临时项目集，并将 i = go(I, a/A) 的i指向临时项目集
                    nmi.setValue(projects.size());
                    projects.add(temP);
                }
            }
        }
    }

    private boolean makeAnalysisTable() {                       // 创建分析表
        for (int si = 0; si < projects.size(); si++) {          // 根据项目集中的状态数量、终结符数量、非终结符数量初始化分析表，表中状态进初始化为ERROR（出错）
            LinkedHashMap<Character, ActionGotoType> temAG = new LinkedHashMap<>();
            for (char vt : Vt) {
                temAG.put(vt, new ActionGotoType(ActionGotoType.ERROR, 0));
            }
                                                                // 额外添加#
            temAG.put('#', new ActionGotoType(ActionGotoType.ERROR, 0));
            for (int di = 0; di < derivation.size(); di++) {
                temAG.put(derivation.get(di).key, new ActionGotoType(ActionGotoType.ERROR, 0));
            }

            analysisTable.put(si, temAG);
        }

        for (int psi = 1; psi < projects.size(); psi++) {       // 遍历项目集族，修改分析表中状态
                                                                // 遍历每一个项目
            for (int pi = 0; pi < projects.get(psi).proLink.size(); pi++) {
                                                                // 临时记录当前项目
                ProjectType temPro = projects.get(psi).proLink.get(pi);
                                                                // 判断当前项目是否是归约项目
                if (temPro.pointer < temPro.derivation.value.length()) {
                                                                // 归约项目，判断是否产生重入（当前对应分析表的位置不为ERROR即说明将发生重入）
                    if (analysisTable.get(psi).get(temPro.derivation.value.charAt(temPro.pointer)).type == ActionGotoType.ERROR) {
                                                                // 记录Action或Goto表
                        analysisTable.get(psi).get(temPro.derivation.value.charAt(temPro.pointer)).type = ActionGotoType.MOVE_IN;
                        analysisTable.get(psi).get(temPro.derivation.value.charAt(temPro.pointer)).aim = projects.get(psi).next.get(temPro.derivation.value.charAt(temPro.pointer));
                    } else if (analysisTable.get(psi).get(temPro.derivation.value.charAt(temPro.pointer)).aim != projects.get(psi).next.get(temPro.derivation.value.charAt(temPro.pointer))) {
                        return false;                           // 重入，报告分析表创建失败
                    }
                } else {                                        // 归约项
                    for (char ec : temPro.expectation) {        // 变量展望符集，将符合展望符集的Action表记录为归约
                                                                // 先判断是否重入
                        if (analysisTable.get(psi).get(ec).type == ActionGotoType.ERROR) {
                            analysisTable.get(psi).get(ec).type = ActionGotoType.REDUCE;
                            analysisTable.get(psi).get(ec).aim = temPro.identify;
                        } else if (analysisTable.get(psi).get(ec).aim != temPro.identify) {
                            return false;                       // 重入，报告分析表创建失败
                        }
                    }
                }
                                                                // 判断是否是[$->E·, #](假设E为原文法的开始符)，归约项，如是，则置Action表对应位置为Acc(接受)
                if (temPro.identify == 0 && temPro.pointer == temPro.derivation.value.length()) {
                                                                // 判断是否重入的条件更为宽松，因为会在之前的分析中被记录为归约到某个产生式，而不是接受，但事实上并不会造成此处归约和接受为相同含义，不会造成重入
                    if (analysisTable.get(psi).get('#').type == ActionGotoType.ERROR || (analysisTable.get(psi).get('#').type == ActionGotoType.REDUCE && analysisTable.get(psi).get('#').aim == 0)) {
                        analysisTable.get(psi).get('#').type = ActionGotoType.ACCEPT;
                    } else {
                        return false;
                    }
                }
            }
        }
        return true;                                            // 分析表创建成功
    }

    public Object[] getAnalysisTable() {                        // 为外部提供非结构化的数据，向UI界面提供数组形式的数据，共两个数组，第一个是一维数组，记录了数据的标题栏；第二个是二维数组，记录了表格内容
        String[] title = new String[analysisTable.get(0).keySet().size() + 1];
        String[][] datas = new String[analysisTable.size() - 1][analysisTable.get(0).keySet().size() + 1];
        int i;
        title[0] = "状态";
        i = 1;
        for (char rowKey : analysisTable.get(0).keySet()) {
            title[i] = rowKey+"";
            i++;
        }
        i = -2;
        for (int row : analysisTable.keySet()) {
            i++;
            if (i == -1) {
                continue;
            }
            datas[i][0] = "I" + row;
            int j = 1;
            for (ActionGotoType col : analysisTable.get(i + 1).values()) {
                datas[i][j] = ActionGotoType.TYPE_STRING[col.type];
                if (col.type > ActionGotoType.ACCEPT) {
                    datas[i][j] += (" " + col.aim);
                }
                j++;
            }
        }

        return new Object[]{title, datas};
    }

    public boolean analysis(StringBuilder sentence) {           // 分析句子，将原句子作为输入串栈
        LinkedList<Integer> status = new LinkedList<>();        // 创建状态栈
        StringBuilder symbol = new StringBuilder("#");          // 创建符号栈，默认栈底为#
        status.add(1);                                          // 初始化状态栈，由于数据结构原因（见上述），此处将状态1作为初始状态
        if (sentence.charAt(sentence.length() - 1) != '#') {    // 判断输入串栈是否在末尾添加了#
            sentence.append('#');
        }
                                                                // 向结果记录表添加一条初始化记录
        resultTable.add(new AnalysisType(status, symbol, sentence, AnalysisType.INITIAL));

        while (sentence.length() > 0) {                         // 循环处理输入串栈
                                                                // 获取当前分析表对应的位置
            ActionGotoType temAGT = analysisTable.get(status.get(status.size() - 1)).get(sentence.charAt(0));
            if (temAGT.type == ActionGotoType.MOVE_IN) {        // 移进
                status.add(temAGT.aim);
                symbol.append(sentence.charAt(0));
                sentence.deleteCharAt(0);
                                                                // 记录
                resultTable.add(new AnalysisType(status, symbol, sentence, AnalysisType.MOVE_IN));
            } else if (temAGT.type == ActionGotoType.REDUCE) {  // 归约
                DerType derType = derivation.get(temAGT.aim);   // 获得规约产生式
                                                                // 对状态栈、符号栈进行归约
                for (int di = 0; di < derType.value.length(); di++) {
                    status.remove(status.size() - 1);
                }
                symbol.delete(symbol.length() - derType.value.length(), symbol.length());
                symbol.append(derType.key);
                status.add(analysisTable.get(status.get(status.size() - 1)).get(symbol.charAt(symbol.length() - 1)).aim);
                                                                // 记录
                resultTable.add(new AnalysisType(status, symbol, sentence, AnalysisType.REDUCE));
            } else if (temAGT.type == ActionGotoType.ERROR) {   // 输入不合理
                resultTable.add(new AnalysisType(status, symbol, sentence, AnalysisType.ERROR));
                return false;                                   // 报错，句子不合法
            } else {                                            // 分析成功
                status.remove(status.size() - 1);
                symbol.deleteCharAt(symbol.length() - 1);
                symbol.append(derivation.get(0).key);
                                                                // 记录
                resultTable.add(new AnalysisType(status, symbol, sentence, AnalysisType.ACCEPT));
                return true;                                    // 报告句子符合文法
            }
        }

        return true;
    }

    public Object[] getResultTable() {                        // 为外部提供非结构化的数据，向UI界面提供数组形式的数据，共两个数组，第一个是一维数组，记录了数据的标题栏；第二个是二维数组，记录了表格内容
        String[] title = {"步骤", "状态", "符号", "输入串", "操作"};
        String[][] datas = new String[resultTable.size()][5];

        for (int i = 0; i < resultTable.size(); i++) {
            datas[i][0] = i + "";
            datas[i][1] = resultTable.get(i).status;
            datas[i][2] = resultTable.get(i).symbol;
            datas[i][3] = resultTable.get(i).input;
            datas[i][4] = AnalysisType.ANALYSIS_TYPE[resultTable.get(i).type];
        }

        return new Object[]{title, datas};
    }
}

class DerType {                                             // 产生式的数据结构
    public Character key;                                   // 产生式左侧非终结符
    public String value;                                    // 产生式右侧

    public DerType(Character k, String v) {
        key = k;
        value = v;
    }
}

class ProjectType {                                         // 项目的数据结构
    public DerType derivation;                              // 项目所用到的产生式
    public int identify;                                    // 产生式的唯一标识id，与产生式一一对应
    public int pointer;                                     // 圆点的位置
    public TreeSet<Character> expectation;                  // 圆点的位置

    public ProjectType(DerType d, int i, int p, TreeSet<Character> e) {
        derivation = d;
        identify = i;
        pointer = p;
        expectation = e;
    }

    public String toString() {                              // 通过产生式的唯一标识id、圆点的位置、圆点的位置确定该项目的标识，用于后续比较
        String tem = identify + "" + pointer;
        for (char e : expectation){
            tem += e;
        }
        return tem;
    }
}

class ProjectListType {                                     // 项目集数据结构
    ArrayList<ProjectType> proLink = new ArrayList<>();     // 记录项目集内容的链表
                                                            // 项目的go函数，指向了下一个分析字符所对应的下一个状态
    LinkedHashMap<Character, Integer> next = new LinkedHashMap<>();

    public String toString() {                              // 通过项目的标识符，获取项目集的标识符，用于判断两个项目是否相同
        String tem = "";
        for (ProjectType pt : proLink) {
            tem += pt.toString();
        }
        return tem;
    }
}

class ActionGotoType {                                      // 关于Action、Goto表的一些定义和数据结构
    public static final int ERROR = 0;                      // 出错
    public static final int ACCEPT = 1;                     // 接受
    public static final int MOVE_IN = 2;                    // 移进
    public static final int REDUCE = 3;                     // 归约
                                                            // 上述标识对应的文字
    public static final String[] TYPE_STRING = {"ERR", "ACC", "S", "R"};
    public int type;                                        // 记录标表格内容类型（如：移进、归约、接受、出错）
    public int aim;                                         // 记录移进或归约的目标

    public ActionGotoType(int t, int a) {
        type = t;
        aim = a;
    }
}

class AnalysisType {                                        // 关于分析表的一些定义和数据结构
    public static final int ERROR = 0;                      // 出错
    public static final int ACCEPT = 1;                     // 接受
    public static final int MOVE_IN = 2;                    // 移进
    public static final int REDUCE = 3;                     // 归约
    public static final int INITIAL = 4;                    // 初始化
                                                            // 上述标识对应的文字
    public static final String[] ANALYSIS_TYPE = {"出错", "接受", "移进", "归约", "初始化"};

    String status;                                          // 记录状态栈
    String symbol;                                          // 记录符号栈
    String input;                                           // 记录输入串栈
    int type;                                               // 记录当前操作的类型（如：移进、归约、初始化等）

    public AnalysisType(LinkedList<Integer> st, StringBuilder sy, StringBuilder in, int ty) {
        StringBuilder tem = new StringBuilder();
        for (int i = 0; i < st.size(); i++) {
            tem.append(st.get(i));
            tem.append('-');                                // 状态符可能具有多位数字，用-分割
        }
        tem.deleteCharAt(tem.length() - 1);
        status = new String(tem);
        symbol = new String(sy);
        input = new String(in);
        type = ty;
    }
}
                                                            // 用于项目集中的项目排序的比较规则
class ComparatorProject implements Comparator<ProjectType> {

    @Override
    public int compare(ProjectType o1, ProjectType o2) {
        if (!o1.toString().equals(o2.toString())) {
            return -1;
        } else {
            return 0;
        }
    }
}
