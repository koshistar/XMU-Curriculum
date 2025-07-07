public class Test {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Vehicle[] vs={new Vehicle(),new Motor(),new Ship(),new Aeroplane(),new Bus(),new Car()};
        for(Vehicle v:vs)
        {
            v.run();
        }
    }
}