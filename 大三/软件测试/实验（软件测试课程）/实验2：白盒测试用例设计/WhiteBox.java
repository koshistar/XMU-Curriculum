import java.util.Scanner;

public class WhiteBox {
    public static char function1(int x, int y) {
        char c;
        if ((x >= 90) && (y >= 90)) {
            c = 'A';
        } else {
            if ((x + y) >= 165) {
                c = 'B';
            } else {
                c = 'C';
            }
        }
        return c;
    }

    public static void function2(int n) {
        int k = 2;
        System.out.print(n + "=");
        while (k <= n) {
            if (k == n) {
                System.out.println(n);
                break;
            } else {
                if (n % k == 0) {
                    System.out.print(k + "*");
                    n = n / k;
                } else {
                    k++;
                }
            }
        }
    }

    public static void main(String[] args) {
        Scanner scaner = new Scanner(System.in);
        // 第一题
//        System.out.print("请输入两个正整数分别为x、y：");
//        int x = scaner.nextInt();
//        int y = scaner.nextInt();
//        System.out.println("结果为：" + function1(x, y));
        //测试代码
//        int [][] testCases={{90,90,'A'},{90,80,'B'},{80,90,'B'},{80,80,'C'}};
//        System.out.println("第一题测试");
//        for(int[] tc:testCases)
//        {
//            int x=tc[0];
//            int y=tc[1];
//            char expected=(char)tc[2];
//            char result=function1(x,y);
//            boolean pass=(result==expected);
//            System.out.printf("x=%d,y=%d => 结果=%c,期望=%c %s%n",x,y,result,expected,pass?"通过":"失败");
//        }
        // 第二题
//        System.out.print("请输入n：");
//        int n = scaner.nextInt();
//        function2(n);
        //测试代码
        int[] testCases={1,2,3,4};
        for(int n:testCases)
        {
            System.out.print("测试 n="+n+" 输出： ");
            function2(n);
            System.out.println();
        }
    }
}
