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
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
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
                    System.out.println("Very Good!");
                    break;
                }
                else
                {
                    System.out.println("No, Please try again. ");
                }
            }
        }
    }
}