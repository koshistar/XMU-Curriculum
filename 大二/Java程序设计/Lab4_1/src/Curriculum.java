public class Curriculum {
    private String ID;
    private String name;
    private int score;
    private boolean isNecessary;
    public Curriculum(String ID,String name,int score,boolean isNecessary)
    {
        this.ID=ID;
        this.name=name;
        this.score=score;
        this.isNecessary=isNecessary;
    }
    public String GetID()
    {
        return ID;
    }
    public String GetName()
    {
        return name;
    }
    public String GetScore()
    {
        return String.valueOf(score);
    }
    public String IsNecessary()
    {
        return isNecessary?"必修":"选修";
    }
}