import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Rational x,y;
        System.out.println("请输入两个有理数:");
        Scanner in=new Scanner(System.in);
        String strx=in.next();
        String stry=in.next();
        x=new Rational(strx);
        y=new Rational(stry);
        System.out.println(x.Add(y));
        System.out.println(x.Subtract(y));
        System.out.println(x.Multiply(y));
        System.out.println(x.Divide(y));
    }
}