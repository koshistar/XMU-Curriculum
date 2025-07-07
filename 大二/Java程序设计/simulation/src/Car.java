public class Car extends Motor{
    private int persons;
    private String carriage;
    public Car(String branch,String color,int persons,int year,String carriage)
    {
        super(branch,color,year);
        this.carriage=carriage;
        this.persons=persons;
    }

    @Override
    public String toString() {
        return "小汽车"+super.toString()+" 载客量："+String.valueOf(persons)+" 厢数："+carriage;
    }
}
