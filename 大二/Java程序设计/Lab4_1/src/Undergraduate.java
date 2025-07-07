public class Undergraduate extends Student{
    public Undergraduate(String IDNum,String name,String _class) {
        this.IDNum = IDNum;
        this.name = name;
        this._class = _class;
    }
    public void ShowCurriculum()
    {
        System.out.println("学号:"+IDNum+";姓名:"+name+";班级:"+_class+";选课:");
        for(Curriculum curriculum :curricula)
        {
            System.out.println("编号:"+curriculum.GetID()+",名称:"+curriculum.GetName()+",学分:"+curriculum.GetScore()+";"+curriculum.IsNecessary());
        }
    }
}
