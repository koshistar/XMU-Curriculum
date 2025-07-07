public class Main {
    static <T> boolean isEqualTo(T a,T b)
    {
        return a.equals(b);
    }
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Object ob1=null;
        Object ob2=new Object();
        Integer integer1=new Integer(5);
        Integer integer2=new Integer(10);
        try
        {
            System.out.println(isEqualTo(ob1,ob1));
        }
        catch (Exception e)
        {
            System.out.println(new RuntimeException("Can;t equals.").getMessage());
        }
        try {
            System.out.println(isEqualTo(integer1,integer2));
        }
        catch (Exception e)
        {
            System.out.println(new RuntimeException("Can;t equals.").getMessage());
        }
        try {
            System.out.println(isEqualTo(ob1,integer2));
        }
        catch (Exception e)
        {
            System.out.println(new RuntimeException("Can;t equals.").getMessage());
        }
    }
}