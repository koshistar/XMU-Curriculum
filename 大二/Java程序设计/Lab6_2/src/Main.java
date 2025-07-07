public class Main {
    static void someMethod() throws  Exception
    {
        try
        {
            someMethod2();
        }
        catch (Exception e)
        {
            throw e;
        }
    }
    static void someMethod2() throws Exception
    {
        throw new Exception("someMethod2's exception.");
    }
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
            try {
                someMethod();
            }
            catch (Exception e)
            {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
    }
}