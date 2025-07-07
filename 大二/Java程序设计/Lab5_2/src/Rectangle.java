public class Rectangle extends Graph{
    int a,b;
    public Rectangle(int a,int b)
    {
        this.a=a;
        this.b=b;
    }
    @Override
    public int area() {
        return a*b;
    }
}
