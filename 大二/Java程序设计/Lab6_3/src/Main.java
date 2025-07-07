public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Student s=new Student();
        try
        {
            s.setName("这是一个非法名字");
        } catch (IllegalNameException e) {
            System.out.println(e.getMessage());
        }
        try {
            s.setAddress("这是一个非法地址");
        } catch (IllegalAddressException e) {
            System.out.println(e.getMessage());
        }
    }
}