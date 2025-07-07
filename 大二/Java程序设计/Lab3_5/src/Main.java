import java.security.SecureRandom;
import java.util.Scanner;

public class Main {
    public static int GenerateNewQuestion()
    {
        SecureRandom ran=new SecureRandom();
        int x=ran.nextInt(9)+1;
        int y=ran.nextInt(9)+1;
        System.out.println("How much is "+x+" times "+y+" ?");
        return x*y;
    }
    public static String AnswerWrong()
    {
        String[] wrong ={"No. Please try again.  ","Wrong. Try once more. ", "Don't give up! ","No. Keep trying. "};
        SecureRandom ran=new SecureRandom();
        return wrong[ran.nextInt(wrong.length)];
    }
    public static String AnswerRight()
    {
        String[] right={"Very good! ","Excellent! ", "Nice Work! ","Keep up the good work! "};
        SecureRandom ran=new SecureRandom();
        return right[ran.nextInt(right.length)];
    }
    public static void main(String[] args) {
//        System.out.println("Hello, World!");

        Scanner in=new Scanner(System.in);
        int cnt=0;
        int rightCnt=0;
        int wrongCnt=0;
        LOOP:
        while(true)
        {
            int res=GenerateNewQuestion();
            int input=0;
            while(true)
            {
                input=in.nextInt();
                if(input==-1)
                {
                    System.out.println("Good Bye!");
                    break LOOP;
                }
                if(input==res)
                {
                    //可以用Switch，但没必要
//                    SecureRandom ran=new SecureRandom();
//                    switch(ran.nextInt(4)+1)
//                    {
//                        case 1: System.out.println("Very good!");break;
//                        case 2: System.out.println("Excellent!");break;
//                        case 3: System.out.println("Nice work!");break;
//                        case 4: System.out.println("Keep up the good work!");break;
//                    }
                    System.out.println(AnswerRight());
                    rightCnt++;
                    break;
                }
                else
                {
//                    SecureRandom ran=new SecureRandom();
//                    switch(ran.nextInt(4)+1)
//                    {
//                        case 1: System.out.println("No. Please try again.");break;
//                        case 2: System.out.println("Wrong. Try once more.");break;
//                        case 3: System.out.println("Don't give up!");break;
//                        case 4: System.out.println("No. Keep trying.");break;
//                    }
                    System.out.println(AnswerWrong());
                    wrongCnt++;
                }
            }
            cnt++;
            if(cnt==10)
            {
                    if(wrongCnt==0||rightCnt/wrongCnt>=3)
                    {
                        System.out.println("Congratulations, you are ready to go to the next level！");
                    }
                    else
                    {
                        System.out.println("Please ask your teacher for extra help. ");
                    }
                    cnt=0;
                    rightCnt=0;
                    wrongCnt=0;
            }
        }
    }
}