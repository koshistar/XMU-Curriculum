import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        MainMenu mm=new MainMenu();
        Scanner in =new Scanner(System.in);
        while(true)
        {
            mm.showMenu();
            String input=in.next();
            switch (input)
            {
                case "1":mm.createMotor();
                    break;
                case "2":mm.inquiryMotor();
                    break;
                case "3":mm.listMotor();
                    break;
                case "4":
                    return;
                default:System.out.println("请输入1-4的整数");
            }
        }
    }
}