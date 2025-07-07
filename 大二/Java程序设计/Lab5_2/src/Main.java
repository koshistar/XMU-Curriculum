import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Scanner in=new Scanner(System.in);
        int n=in.nextInt();
        in.nextLine();
        for(int i=0;i<n;i++)
        {
            String str=in.nextLine();
            String[] data=str.split(" ");
            int[] edges=new int[data.length];
            for(int j=0;j<data.length;j++)
            {
                edges[j]=Integer.parseInt(data[j]);
            }
            if(edges.length==2)
            {
                Graph rec=new Rectangle(edges[0],edges[1]);
                System.out.println(rec.area());
            }
            else if(edges.length==3)
            {
                Graph tri=new Triangle(edges[0],edges[1],edges[2]);
                System.out.println(tri.area());
            }
        }
    }
}