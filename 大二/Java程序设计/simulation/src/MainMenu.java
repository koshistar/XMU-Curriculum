import java.util.ArrayList;
import java.util.Scanner;
import java.util.regex.Pattern;

public class MainMenu {
    private ArrayList<Motor> motors=new ArrayList<Motor>();
    private Scanner in=new Scanner(System.in);
    public void showMenu()
    {
        System.out.println("请输入对应输入进行相应菜单操作");
        System.out.println("1.                  新增车辆");
        System.out.println("2.                  查询车辆");
        System.out.println("3.                  列出所有车辆");
        System.out.println("4.                  结束程序");
    }
    public void menu1()
    {
        System.out.println("请输入车辆信息，示例:");
        System.out.println("        小汽车 丰田  红色  4   2007    2厢");
        System.out.println("        卡车 雷诺  红色  3.5   2008");
        System.out.println("结束新增，请输入end");
    }
    public void createMotor()
    {
        while(true)
        {
            menu1();
            String input=in.nextLine();
            String[] inputs=input.split("[ ]+");
            System.out.println(inputs.length);
            switch(inputs[0])
            {
                case "小汽车":if(inputs.length!=6)
                {
                    System.out.println("请输入6个参数");
                    createFail();
                    break;
                }
                System.out.println(inputs[5]);
                if(!isInteger(inputs[3]))
                {
                    System.out.println("第四个应为整数");
                    createFail();
                    break;
                }
                if(!isInteger(inputs[4]))
                {
                    System.out.println("第五个应为整数");
                    createFail();
                    break;
                }
                if(inputs[5].equals("2厢")&&inputs[5].equals("3厢"))
                {
                    System.out.println("第六个应为2厢或3厢");
                    createFail();
                    break;
                }
                motors.add(new Car(inputs[1],inputs[2],Integer.valueOf(inputs[3]),Integer.valueOf(inputs[4]),inputs[5]));
                System.out.println("创建成功");
                break;
                case "卡车":
                    if(inputs.length!=5)
                    {
                        System.out.println("请输入5个参数");
                        createFail();
                        break;
                    }
                    if(!isDouble(inputs[3]))
                    {
                        System.out.println("第四个应为浮点数");
                        createFail();
                        break;
                    }
                    if(!isInteger(inputs[4]))
                    {
                        System.out.println("第五个应为整数");
                        createFail();
                        break;
                    }
                    motors.add(new Trunk(inputs[1],inputs[2],Double.valueOf(inputs[3]),Integer.valueOf(inputs[4])));
                    System.out.println("创建成功");
                    break;
                case "end":System.out.println("结束新增");
                    return;
                default:System.out.println("第一个应为小汽车或者卡车");
                createFail();
            }
        }
    }
    private void createFail()
    {
        System.out.println("\n创建不成功");
    }

    private void inquiryMenu()
    {
        System.out.println("请按照“类型 商标 颜色 出厂车 ”的顺序输入条件，若条件为空用“null”代替，2个示例：");
        System.out.println("                    小汽车 丰田  红色  2007");
        System.out.println("                    null 丰田 null null");
        System.out.println("输入end返回上级菜单");
    }
    public void inquiryMotor()
    {
        while (true)
        {
            inquiryMenu();
            String input=in.nextLine();
            String[] inputs=input.split(" ");
            if(inputs[0].equals("end"))
            {
                return;
            }
            if(inputs.length!=4)
            {
                System.out.println("请输入4个参数");
                continue;
            }
            ArrayList<Motor> temp=new ArrayList<>();
            for(Motor motor:motors)
            {
                temp.add(motor);
            }
            if(inputs[0]=="小汽车")
            {
                for(Motor motor:temp)
                {
                    if(motor.getClass()!=Car.class)
                    {
                        temp.remove(motor);
                    }
                }
            }
            else if(inputs[0]=="卡车")
            {
                for(Motor motor:temp)
                {
                    if(motor.getClass()!=Trunk.class)
                    {
                        temp.remove(motor);
                    }
                }
            }
            else if(inputs[0]=="null")
                ;
            else
            {
                System.out.println("没有搜索到相应的交通工具");
                continue;
            }
            if(inputs[1]!="null")
            {
                for(Motor motor:temp)
                {
                    if(motor.getBranch()!=inputs[1])
                    {
                        temp.remove(motor);
                    }
                }
            }
            if(inputs[2]!="null")
            {
                for(Motor motor:temp)
                {
                    if(motor.getColor()!=inputs[2])
                    {
                        temp.remove(motor);
                    }
                }
            }
            if(inputs[3]=="null")
                ;
            else if(!isInteger(inputs[3]))
            {
                System.out.println("第四个参数应为整形");
                continue;
            }
            else
            {
                for(Motor motor:temp)
                {
                    if(motor.getYear()!=Integer.valueOf(inputs[3]))
                    {
                        temp.remove(motor);
                    }
                }
            }
            if(temp.isEmpty())
            {
                System.out.println("没有搜索到相应的交通工具");
            }
            else
            {
                System.out.println("搜索到"+temp.size()+"辆车");
                for(Motor motor:temp)
                {
                    System.out.println(motor);
                }
            }
        }
    }

    public void listMotor()
    {
        System.out.println("目前有"+motors.size()+"辆车的信息如下：");
        for(Motor motor :motors)
        {
            System.out.println(motor);
        }
    }

    private boolean isInteger(String str)
    {
        Pattern pattern=Pattern.compile("^[-\\+]?[\\d]*$");
        return pattern.matcher(str).matches();
    }
    private boolean isDouble(String str)
    {
        Pattern pattern=Pattern.compile("^[-\\+]?\\d*[.]\\d+$");
        return pattern.matcher(str).matches();
    }
}
