public class Trunk extends Motor{
    private double weight;
    public Trunk(String branch,String color,double weight,int year)
    {
        super(branch,color,year);
        this.weight=weight;
    }
    @Override
    public String toString() {
        return "卡车"+super.toString()+" 载重量："+String.valueOf(weight);
    }
}
