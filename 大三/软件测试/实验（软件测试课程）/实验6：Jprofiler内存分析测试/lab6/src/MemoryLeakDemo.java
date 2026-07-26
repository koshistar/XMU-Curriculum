import java.util.ArrayList;
import java.util.List;

public class MemoryLeakDemo {
    // 模拟内存泄漏：静态List持续持有对象引用，GC无法回收
    private static List<byte[]> leakList = new ArrayList<>();

    public static void main(String[] args) throws InterruptedException {
        System.out.println("开始模拟内存泄漏...");
        int i = 0;
        while (true) {
            // 每次分配 1MB 数据，添加到静态列表（不会被GC回收）
            byte[] data = new byte[1024 * 1024]; // 1MB
            leakList.add(data);
            i++;
            System.out.println("已分配 " + i + " MB，总计: " +
                    (leakList.size()) + " 个对象");
            Thread.sleep(500); // 每500ms分配一次
        }
    }
}
