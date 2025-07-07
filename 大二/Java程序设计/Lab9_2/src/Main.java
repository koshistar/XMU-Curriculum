import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        String name,key,mail;
        LOOP:
        while (true) {
            System.out.print("用户:");
            name = in.nextLine();
            System.out.print("密码:");
            key = in.nextLine();
            System.out.print("邮箱:");
            mail = in.nextLine();
            if(name.length()==0)
            {
                System.out.println("用户名不能为空");
                continue;
            }
            if(key.length()==0)
            {
                System.out.println("密码不能为空");
                continue;
            }
            if(mail.length()==0)
            {
                System.out.println("邮箱不能为空");
                continue;
            }

            if(Character.isDigit(name.charAt(0)))
            {
                System.out.println("用户名第一位不能为数字");
                continue;
            }
            for(int i=0;i<name.length();i++)
            {
                if(!(Character.isLetterOrDigit(name.charAt(i)))&&name.charAt(i)!='_')
                {
                    System.out.println("用户名只能由字母、数字和_组成");
                    continue LOOP;
                }
            }

            if(key.length()<8)
            {
                System.out.println("密码长度至少8位");
                continue;
            }
            for(int i=0;i<key.length();i++)
            {
                //中文字符也算字母
                if(!(Character.isLetterOrDigit(key.charAt(i))||key.charAt(i)=='_'))
                {
                    System.out.println("密码只能由字母、数字和下划线组成");
                    continue LOOP;
                }
            }

            if(mail.indexOf('@')==-1)
            {
                System.out.println("邮箱中应包含@");
                continue;
            }
            String[] temp=mail.substring(mail.indexOf('@')+1).split("[.]");
            if(temp.length<2)
            {
                System.out.println("邮箱中@后应该出现多个由'.'分割的词");
                continue;
            }

            System.out.println("输入成功");
            break;
        }
    }
}