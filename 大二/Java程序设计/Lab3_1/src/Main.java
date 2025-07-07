public class Main {
    public static boolean isPrime(int x)
    {
        if(x<2)
        {
            return false;
        }
        if(x==2)
        {
            return true;
        }
        else
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
    public static void main(String[] args) {
//        System.out.println("Hello, World!");

        //题目a
//        Scanner in =new Scanner(System.in);
//        System.out.print("请输入一个数: ");
//        int x = in.nextInt();
//        System.out.printf("\n"+x+(isPrime(x)?"是素数":"不是素数"));

        //题目b
        //需要测试（10000-3）/2次
        System.out.print("2 ");
        for(int i=3;i<10000;i+=2)
        {
            if(isPrime(i))
            {
                System.out.print(i+" ");
            }
        }

        System.out.println();
        //题目c
        boolean[] flag =new boolean[10000];
        for(int i=4;i<10000;i+=i)
        {
            flag[i]=true;
        }
        //需要测试（10000的平方根-3）次
        for(int i=3;i * i <= 10000;i+=2)
        {
            if(!flag[i]&&isPrime(i))
            {
                for(int j=2*i;j<10000;j+=i)
                {
                    flag[j]=true;
                }
            }
        }
        System.out.print("2 ");
        for(int i=3;i<10000;i+=2)
        {
            if(!flag[i])
            {
                System.out.print(i+" ");
            }
        }
    }
}