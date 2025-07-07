import java.util.Random;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        Random ran =new Random();
        Graduate[] graduates=new Graduate[2];
        Undergraduate[] undergraduates=new Undergraduate[2];
        Curriculum[] necessaryCurricula=new Curriculum[2];
        Curriculum[] unnecessaryCurricula=new Curriculum[2];
        System.out.println("请输入两名本科生信息(学号、姓名、班级):");
        for(int i=0;i<2;i++)
        {
            String IDNum,name,_class;
            IDNum=in.next();
            name=in.next();
            _class=in.next();
            undergraduates[i]=new Undergraduate(IDNum,name,_class);
        }
        System.out.println("请输入两名研究生信息(学号、姓名、班级、导师):");
        for(int i=0;i<2;i++)
        {
            String IDNum,name,_class,mentor;
            IDNum=in.next();
            name=in.next();
            _class=in.next();
            mentor=in.next();
            graduates[i]=new Graduate(IDNum,name,_class,mentor);
        }
        System.out.println("请输入课程信息，前两个为必修，后两个为选修(编号、课程名、学分):");
        for(int i=0;i<2;i++)
        {
            String ID,name;
            int score;
            ID=in.next();
            name=in.next();
            score=in.nextInt();
            necessaryCurricula[i]=new Curriculum(ID,name,score,true);
            undergraduates[0].AddCurriculum(necessaryCurricula[i]);
            undergraduates[1].AddCurriculum(necessaryCurricula[i]);
            graduates[0].AddCurriculum(necessaryCurricula[i]);
            graduates[1].AddCurriculum(necessaryCurricula[i]);
        }
        for(int i=0;i<2;i++)
        {
            String ID, name;
            int score;
            ID = in.next();
            name = in.next();
            score = in.nextInt();
            unnecessaryCurricula[i] = new Curriculum(ID, name, score, false);
        }
        switch(ran.nextInt(3))
        {
            case 2:undergraduates[0].AddCurriculum(unnecessaryCurricula[1]);
            case 0:undergraduates[0].AddCurriculum(unnecessaryCurricula[0]);break;
            case 1:undergraduates[0].AddCurriculum(unnecessaryCurricula[1]);break;
        }
        switch(ran.nextInt(3))
        {
            case 2:undergraduates[1].AddCurriculum(unnecessaryCurricula[1]);
            case 0:undergraduates[1].AddCurriculum(unnecessaryCurricula[0]);break;
            case 1:undergraduates[1].AddCurriculum(unnecessaryCurricula[1]);break;
        }
        switch(ran.nextInt(3))
        {
            case 2:graduates[0].AddCurriculum(unnecessaryCurricula[1]);
            case 0:graduates[0].AddCurriculum(unnecessaryCurricula[0]);break;
            case 1:graduates[0].AddCurriculum(unnecessaryCurricula[1]);break;
        }
        switch(ran.nextInt(3))
        {
            case 2:graduates[1].AddCurriculum(unnecessaryCurricula[1]);
            case 0:graduates[1].AddCurriculum(unnecessaryCurricula[0]);break;
            case 1:graduates[1].AddCurriculum(unnecessaryCurricula[1]);break;
        }
        undergraduates[0].ShowCurriculum();
        undergraduates[1].ShowCurriculum();
        graduates[0].ShowCurriculum();
        graduates[1].ShowCurriculum();
    }
}