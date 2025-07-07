public class Test {
    public static void main(String[] args) {
        for(TrafficLight tl :TrafficLight.values())
        {
            System.out.println(tl+": "+tl.ToString());
        }
    }
}