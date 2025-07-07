import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        String sentence=in.nextLine();
        String processed=sentence.replaceAll("[^a-zA-Z ]","").toLowerCase();//忽略标点符号，并转化为小写字母
        Map<String,Integer> wordCount=new HashMap<>();
        String[] words=processed.split("\\s+");
        for(String word:words)
        {
//            System.out.println(word);
            if(!word.isEmpty())
            {
                wordCount.put(word,wordCount.getOrDefault(word,0)+1);
            }
        }
        int cnt=0;
        for(Map.Entry<String,Integer> entry:wordCount.entrySet())
        {
            if(entry.getValue()>1)
            {
                cnt++;
                System.out.println(entry.getKey()+"重复"+entry.getValue()+"次");
            }
        }
        if(cnt>0)
            System.out.print("共有"+cnt+"个单词重复");
        else
            System.out.println("没有重复单词");
    }
}