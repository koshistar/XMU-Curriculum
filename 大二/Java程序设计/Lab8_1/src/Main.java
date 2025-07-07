import java.util.Scanner;
import  java.util.Set;
import java.util.HashSet;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
//        String[] names={"仲村百合","立华奏","音无结弦","日向秀树","岩泽雅美","渊田久子","入江美雪","关根诗织","芳冈由依","松下护驒","直井文人","野田","高松","椎名","游佐","藤卷","大山","竹山","查","TK"};
        Set<String> names=new HashSet<String>();//自动删除重复名字
        Scanner in=new Scanner(System.in);
        System.out.println("请输入一个名字序列(输入#quit退出)");
        while(true)
        {
            String name=in.next();
            if(name.equals("#quit"))
            {
                break;
            }
            names.add(name);
        }
        System.out.println("以下是你输入的名字集合：");
        for(String name : names)
        {
            System.out.print(name+" ");
        }
    }
}