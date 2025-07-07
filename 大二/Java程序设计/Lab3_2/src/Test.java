import java.util.Random;
import java.util.Scanner;

class Student {
    private String IDnum;
    private String name;
    private String _class;
    private String telephone;
    public Student(String IDnum,String name,String _class)
    {
        this.IDnum=IDnum;
        this.name=name;
        this._class=_class;
    }
    public Student(String IDnum,String name,String _class,String telephone)
    {
        this.IDnum=IDnum;
        this.name=name;
        this._class=_class;
        this.telephone=telephone;
    }
    public String GetName()
    {
        return name;
    }
}
class Curriculum {
    private String ID;
    private String name;
    public Curriculum(String ID,String name)
    {
        this.ID=ID;
        this.name=name;
    }
    public String GetName()
    {
        return name;
    }
}
public class Test {
    public static void Main()
    {
        Scanner in = new Scanner(System.in);
        System.out.println("请输入两个学生信息(1-无电话信息，2-有电话信息)（学号/姓名/班级/电话）:");
        Student[] s=new Student[2];
        Curriculum[] c=new  Curriculum[2];
        for(int i=0;i<2;i++)
        {
            int flag=in.nextInt();
            if(flag==1)
            {
                String IDnum=in.next();
                String name=in.next();
                String _class=in.next();
                s[i]=new Student(IDnum,name,_class);
            }
            else if(flag==2)
            {
                String IDnum=in.next();
                String name=in.next();
                String _class=in.next();
                String telephone=in.next();
                s[i]=new Student(IDnum,name,_class,telephone);
            }
        }
        System.out.println("\n请输入课程信息:（编号/名称）");
        for(int i=0;i<2;i++)
        {
            String ID=in.next();
            String name=in.next();
            c[i]=new Curriculum(ID,name);
        }
        System.out.println("学生"+s[0].GetName()+"选择课程"+c[0].GetName()+",成绩为"+ (int)(Math.random()*100+(int)Math.random()));
        System.out.println("学生"+s[1].GetName()+"选择课程"+c[1].GetName()+",成绩为"+ (int)(Math.random()*100+(int)Math.random()));
    }
}
