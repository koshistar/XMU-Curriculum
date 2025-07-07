public class Graduate extends Student
{
    private String mentor;
    public Graduate(String IDNum,String name,String _class,String mentor)
    {
        this.IDNum=IDNum;
        this.name=name;
        this._class=_class;
        this.mentor=mentor;
    }
    public void ShowCurriculum()
    {
        System.out.println("学号:"+IDNum+";姓名:"+name+";班级:"+_class+";导师:"+mentor+";选课:");
        for(Curriculum curriculum :curricula)
        {
            System.out.println("编号:"+curriculum.GetID()+",名称:"+curriculum.GetName()+",学分:"+curriculum.GetScore()+";"+curriculum.IsNecessary());
        }
    }
}
