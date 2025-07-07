import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Building building=new Building(100.0,10.0);
        Car car=new Car(100.0);
        Bicycle bicycle=new Bicycle(10.0,90.0);
        ArrayList<CarbonFootprint> carbonFootprintArrayList=new ArrayList<CarbonFootprint>();
        carbonFootprintArrayList.add(building);
        carbonFootprintArrayList.add(car);
        carbonFootprintArrayList.add(bicycle);
        for(CarbonFootprint carbonFootprint:carbonFootprintArrayList)
        {
            System.out.println(carbonFootprint);
        }
    }
}