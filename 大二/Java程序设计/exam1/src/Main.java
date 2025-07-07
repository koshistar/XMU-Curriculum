import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        MainMenu mm=new MainMenu();
        //程序运行时，先显示余额
        mm.showMoney();
        while(true)
        {
            //每次执行后都显示一遍主菜单
            mm.showMainMenu();
            String input=in.next();
            //根据输入执行相应方法
            switch (input)
            {
                case "1":mm.increaseRent();break;
                case "2":mm.increaseBuy();break;
                case "3":mm.increaseSalary();break;
                case "4":mm.showHistory();break;
                case "5":mm.showMoney();break;
                case "6":return;
                default:System.out.println("请输入1-6的整形");break;
            }
        }
    }
}