public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
            try {
                SomeClass sc=new SomeClass();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
    }
}