import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        //（1）继承Thread
//        Thread1 thread1_1=new Thread1("线程1");
//        Thread1 thread1_2=new Thread1("线程2");
//        thread1_1.start();
//        thread1_2.start();
        //（2）实现Runnable接口，用Thread方法启动线程
//        Runnable thread2_1=new Thread2("线程1");
//        Runnable thread2_2=new Thread2("线程2");
//        new Thread(thread2_1).start();
//        new Thread(thread2_2).start();
        //（3）用线程池管理
        ExecutorService executorService= Executors.newFixedThreadPool(2);
        executorService.execute(()->{
            for(int i=1;i<=10;i++)
            {
                System.out.println("线程1:"+i);
            }
        });
        executorService.execute(()->{
            for(int i=1;i<=10;i++)
            {
                System.out.println("线程2:"+i);
            }
        });
        executorService.shutdown();
    }
}
class Thread1 extends  Thread{
    String threadName;
    public Thread1(String threadName)
    {
        this.threadName=threadName;
    }
    @Override
    public void run() {
        for(int i=1;i<=10;i++)
        {
            System.out.println(threadName+":"+i);
        }
    }
}
class Thread2 implements Runnable
{
    String threadName;
    public Thread2(String threadName)
    {
        this.threadName=threadName;
    }
    @Override
    public void run() {
        for(int i=1;i<=10;i++)
        {
            System.out.println(threadName+":"+i);
        }
    }
}