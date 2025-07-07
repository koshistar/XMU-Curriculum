public class Triangle extends Graph{
    int a,b,c;
    public Triangle(int a, int b,int c)
    {
        this.a=a;
        this.b=b;
        this.c=c;
    }
    @Override
    public int area() {
        double p=(a+b+c)/2.0;
        return (int)Math.sqrt(p*(p-a)*(p-b)*(p-c));
    }
}
