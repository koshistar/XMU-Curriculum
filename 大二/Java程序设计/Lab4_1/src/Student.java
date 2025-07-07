import java.util.ArrayList;

public class Student {
    protected String IDNum;
    protected String name;
    protected String _class;
    protected ArrayList<Curriculum> curricula=new ArrayList<>();
    public void AddCurriculum(Curriculum curriculum)
    {
        curricula.add(curriculum);
    }
 }
