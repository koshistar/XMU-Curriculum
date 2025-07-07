public class Triangle extends Graph{
    private int a,b,c;
    public Triangle(int x,int y,int z)
    {
        a=x;
        b=y;
        c=z;
    }
    public int area()
    {
        double p=(a+b+c)/2.0;
        return (int)Math.sqrt(p*(p-a)*(p-b)*(p-c));
    }
}
