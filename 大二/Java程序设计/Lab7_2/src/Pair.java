public class Pair<F,S> {
    private F f;
    private S s;
    public Pair()
    {

    }
    public Pair(F f,S s)
    {
        this.f=f;
        this.s=s;
    }
    public F getF()
    {
        return f;
    }
    public S getS()
    {
        return s;
    }
    public void setF(F f)
    {
        this.f=f;
    }
    public void setS(S s)
    {
        this.s=s;
    }

    @Override
    public String toString() {
        return "F:"+getF()+";S:"+getS();
    }
}
