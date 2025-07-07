import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        int input=in.nextInt();
        if(isPrime(input))
        {
            System.out.println(input+"是质数");
        }
        else
        {
            Set<Integer> primes=new HashSet<>();
            int temp=input;
            while(temp!=1)
            {
                for(int i=2;i<=temp;i++)
                {
                    if(isPrime(i)&&temp%i==0)
                    {
                        temp/=i;
                        primes.add(i);
                        break;
                    }
                }
            }
            System.out.println(input+"的因子为:");
            for(int p:primes)
            {
                System.out.print(p+" ");
            }
        }
    }
    private static boolean isPrime(int x)
    {
        for(int i=2;i*i<=x;i++)
        {
            if(x%i==0)
            {
                return false;
            }
        }
        return true;
    }
}