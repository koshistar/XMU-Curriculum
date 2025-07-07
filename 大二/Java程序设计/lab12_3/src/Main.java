import java.util.LinkedList;
import java.util.Queue;
import java.util.Random;
import java.util.concurrent.*;

public class Main {
    private static class CircularBufferArea{
        private int capacity;
        private Queue<Integer> buffer;
        private Object lock=new Object();
        public CircularBufferArea(int capacity)
        {
            this.capacity=capacity;
            this.buffer=new LinkedList<>();
        }
        public void put(int value) throws InterruptedException {
            synchronized (lock)
            {
                while(buffer.size()==capacity)
                {
                    lock.wait();
                }
//                System.out.println(buffer.size());
                buffer.add(value);
                System.out.println("put:"+value);
                lock.notifyAll();
            }
        }
        public int get() throws InterruptedException {
            synchronized (lock) {
                while (buffer.size() == 0) {
                    lock.wait();
                }
                int val = buffer.poll();
                System.out.println("get:"+val);
                lock.notifyAll();
                return val;
            }
        }
    }
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        final int times=100;//循环的次数
        //Way1
//        CircularBufferArea circularBufferArea=new CircularBufferArea(4);
//        Thread thread1=new Thread(()->{
//            Random ran=new Random();
//            for(int i=0;i<times;i++)
//            {
//                try{
//                    int val=ran.nextInt(10)+1;
//                    circularBufferArea.put(val);
//                }
//                catch (InterruptedException e) {
//                    throw new RuntimeException(e);
//                }
//            }
//        });
//        Thread thread2=new Thread(()->{
//            for(int i=0;i<times;i++)
//            {
//                try
//                {
//                    int val=circularBufferArea.get();
//                } catch (InterruptedException e) {
//                    throw new RuntimeException(e);
//                }
//            }
//        });
//        thread1.start();
//        thread2.start();
        //Way2
        BlockingQueue<Integer> circularBufferArea=new ArrayBlockingQueue<>(4);
        ExecutorService executorService= Executors.newFixedThreadPool(2);
        executorService.execute(new Thread(()->{
            for(int i=0;i<times;i++)
            {
                Random ran=new Random();
                int value= ran.nextInt(10)+1;
                try {
                    circularBufferArea.put(value);
                    System.out.println("put:"+value);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }));
        executorService.execute(new Thread(()->{
            for(int i=0;i<times;i++)
            {
                try {
                    int value=circularBufferArea.take();
                    System.out.println("get:"+value);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }));
        executorService.shutdown();
    }
}