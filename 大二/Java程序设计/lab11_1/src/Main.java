import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        System.out.println("请输入一个文件路径");
        Scanner in=new Scanner(System.in);
        String path=in.nextLine();
//        System.out.println(path);
        File file=new File(path)  ;
        if(file.exists())
        {
            if(file.isDirectory())
            {
                File[] fileList=file.listFiles();
                if(fileList==null)
                {
                    System.out.println("无法访问文件夹");
                    return;
                }
                int directoryNum=0;
                int fileNum=0;
                for(File f:fileList)
                {
                    if(f.isDirectory())
                    {
                        directoryNum++;
                    }
                    else if(f.isFile())
                    {
                        fileNum++;
                    }
                }
                System.out.println("该文件夹的文件个数为："+fileNum+"，文件夹个数为："+directoryNum);
            }
            else if(file.isFile())
            {
                SimpleDateFormat sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                System.out.println("该文件的大小为："+file.length()+"，最后修改日期为："+sdf.format(file.lastModified()));
            }
            else
            {
                System.out.println("该路径不是文件或文件夹");
            }
        }
        else
        {
            System.out.println("无效路径");
        }
    }
}