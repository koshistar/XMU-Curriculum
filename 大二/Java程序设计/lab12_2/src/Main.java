import java.util.Random;

public class Main {
    private static  final int arr[]=new int[10000];
    private static Random ran=new Random();
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Thread thread1=new Thread(new MyThread(0,2500-1,1,10));
        Thread thread2=new Thread(new MyThread(2500,5000-1,100,1000));
        Thread thread3=new Thread(new MyThread(5000,7500-1,2000,3000));
        Thread thread4=new Thread(new MyThread(7500,10000-1,4000,5000));

        thread1.run();
        thread2.run();
        thread3.run();
        thread4.run();

        for(int val:arr)
        {
            System.out.print(val+" ");
        }
    }
    static class MyThread implements Runnable
    {
        int fromValue;
        int toValue;
        int minValue;
        int maxValue;
        public MyThread(int fromValue,int toValue,int minValue,int maxValue)
        {
            this.fromValue=fromValue;
            this.toValue=toValue;
            this.minValue=minValue;
            this.maxValue=maxValue;
        }
        @Override
        public void run() {
            for(int i=fromValue;i<=toValue;i++)
            {
                arr[i]=minValue+Math.abs(ran.nextInt())%(maxValue-minValue);
            }
        }
    }
}
