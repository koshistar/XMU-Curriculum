import java.util.Random;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Main {
    public static void main(String args[])
    {
        Random ran=new Random();
        CircularBufferArea circularBufferArea=new CircularBufferArea<Integer>(4);
        Thread put=new Thread(()->{
            try
            {
                for(int i=0;i<10;i++)
                {
                    int value=Math.abs(ran.nextInt()%10);
                    circularBufferArea.put(value);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        });
        Thread take=new Thread(()->
        {
            try{
                for(int i=0;i<10;i++)
                {
                  var value=circularBufferArea.take();
                }
            }
            catch (InterruptedException e)
            {
                Thread.currentThread().interrupt();
            }
        });
        put.start();
        take.start();
    }
}
class CircularBufferArea<T> {
    private int capacity;
    private T[] buffer;

    private int count = 0;
    private int putIndex = 0;
    private int takeIndex = 0;

    private final Lock lock = new ReentrantLock();
    private final Condition notFull = lock.newCondition();
    private final Condition notEmpty = lock.newCondition();

    public CircularBufferArea(int capacity) {
        this.capacity = capacity;
        this.buffer = (T[]) new Object[capacity];
    }
    public void put(T item) throws InterruptedException {
        lock.lock();
        try {
            while (count == capacity) {
                notFull.await();
            }
            buffer[putIndex] = item;
            System.out.println("put "+item);
            putIndex = (putIndex + 1) % capacity;
            count++;

            notEmpty.signal();
        } finally {
            lock.unlock();
        }
    }

    public T take() throws InterruptedException {
        lock.lock();
        try {
            while (count == 0)
                notEmpty.await();

            T item = buffer[takeIndex];
            buffer[takeIndex] = null;
            takeIndex = (takeIndex + 1) % capacity;
            count--;
            notFull.signal();
            System.out.println("take "+item);
            return item;
        } finally {
            lock.unlock();
        }
    }
}