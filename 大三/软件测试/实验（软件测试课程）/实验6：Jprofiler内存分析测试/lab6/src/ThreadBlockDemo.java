public class ThreadBlockDemo {
    private static final Object lock1 = new Object();
    private static final Object lock2 = new Object();

    public static void main(String[] args) {
        // 线程A：先锁lock1，再锁lock2
        Thread threadA = new Thread(() -> {
            synchronized (lock1) {
                System.out.println("线程A获得lock1，等待lock2...");
                try { Thread.sleep(100); } catch (InterruptedException e) {}
                synchronized (lock2) {
                    System.out.println("线程A获得lock2");
                }
            }
        }, "Thread-A");

        // 线程B：先锁lock2，再锁lock1（与A形成死锁）
        Thread threadB = new Thread(() -> {
            synchronized (lock2) {
                System.out.println("线程B获得lock2，等待lock1...");
                try { Thread.sleep(100); } catch (InterruptedException e) {}
                synchronized (lock1) {
                    System.out.println("线程B获得lock1");
                }
            }
        }, "Thread-B");

        threadA.start();
        threadB.start();
    }
}
