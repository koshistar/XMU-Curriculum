public class Motor {
    private String branch;
    private String color;
    private int year;
    public Motor(String branch,String color,int year)
    {
        this.branch=branch;
        this.color=color;
        this.year=year;
    }
    public String getBranch()
    {
        return branch;
    }
    public String getColor()
    {
        return color;
    }
    public int getYear()
    {
        return year;
    }

    @Override
    public String toString() {
        return ",品牌："+branch+" 颜色："+color+" 出厂年份："+String.valueOf(year);
    }
}
