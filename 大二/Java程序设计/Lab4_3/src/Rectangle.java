public class Rectangle extends Graph{
    private int a,b;
    public Rectangle(int x,int y)
    {
        a=x;
        b=y;
    }
    public int area()
    {
        return a*b;
    }
}
