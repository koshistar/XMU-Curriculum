import java.util.ArrayList;
import java.util.Scanner;
import java.util.regex.Pattern;

public class MainMenu {
    private ArrayList<Expense> expenses=new ArrayList<Expense>();
    private Scanner in=new Scanner(System.in);
    private double money= 500000.0;//账户余额
    public void showMainMenu()
    {
        System.out.println("1：新增租金支出");
        System.out.println("2：新增商品支出");
        System.out.println("3：发工资");
        System.out.println("4：显示所有支付记录");
        System.out.println("5：显示账户余额");
        System.out.println("6：退出");
    }

    private void showRentMenu()
    {
        System.out.println("输入发票编号 描述 租金金额");
    }
    public void increaseRent()
    {
        //输入有误时重复循环，直到创建成功
        while(true)
        {
            showRentMenu();
            String input=in.nextLine();
            String[] inputs=input.split(" ");
            if(inputs.length!=3)
            {
                System.out.println("请输入三个参数");
                continue;
            }
            if(!isInteger(inputs[2]))
            {
                System.out.println("金额请输入整形");
                continue;
            }
            Expense e=new Rent(inputs[0],inputs[1],Integer.valueOf(inputs[2]));
            expenses.add(e);
            money-=e.getExpense();
            System.out.println(e);
            break;
        }
    }

    private void showBuyMenu()
    {
        System.out.println( "请输入发票号 商品名称 数量 单价");
    }
    public void increaseBuy()
    {
        while(true)
        {
            showBuyMenu();
            String input=in.nextLine();
            String[] inputs=input.split(" ");
            if(inputs.length!=4)
            {
                System.out.println("请输入四个参数");
                continue;
            }
            if(!isInteger(inputs[2]))
            {
                System.out.println("数量请输入整形");
                continue;
            }
            if(!isInteger(inputs[3]))
            {
                System.out.println("单价请输入整形");
                continue;
            }
            Expense e=new Buy(inputs[0],inputs[1],Integer.valueOf(inputs[2]),Integer.valueOf(inputs[3]));
            expenses.add(e);
            money-= e.getExpense();
            System.out.println(e);
            break;
        }
    }
    public void increaseSalary()
    {
        Expense e=new Salary();
        expenses.add(e);
        money-=e.getExpense();
        System.out.println(e);
    }
    public void showHistory()
    {
        for(Expense expense:expenses)
        {
            System.out.println(expense);
        }
    }
    public double getMoney()
    {
        return money;
    }
    public void showMoney()
    {
        System.out.println("账户余额"+String.format("%.1f",money));
    }
    //用于判断输入参数是否为整形
    private boolean isInteger(String str)
    {
        Pattern pattern=Pattern.compile("^[-\\+]?[\\d]*$");
        return pattern.matcher(str).matches();
    }
}
