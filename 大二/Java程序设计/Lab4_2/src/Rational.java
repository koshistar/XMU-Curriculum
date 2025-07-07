public class Rational {
    private int  numerator;
    private int denominator;
    public Rational(int num)
    {
        numerator=num;
        denominator=1;
    }
  private boolean IsInt(String str)
  {
      int i=0;
      if(str.charAt(0)=='-')
      {
          i++;
      }
      for(;i<str.length();i++)
      {
          if(str.charAt(i)>'9'||str.charAt(i)<'0')
              return false;
      }
      return true;
  }
  public Rational(String num)
  {
      if(IsInt(num))
      {
          numerator=Integer.valueOf(num);
          denominator=1;
      }
      else if(num.indexOf('.')!=-1)//小数
      {
          String integer=num.substring(0,num.indexOf('.'));//整数部分
          String decimal=num.substring(num.indexOf('.')+1,num.length());//小数部分
          if(IsInt(integer)&&IsInt(decimal))
          {
              numerator=Integer.valueOf(integer+decimal);
              denominator=(int)Math.pow(10,decimal.length());
              Simplify();
          }
      }
      else if(num.indexOf('/')!=-1)
      {
          String n=num.substring(0,num.indexOf('/'));
          String d=num.substring(num.indexOf('/')+1,num.length());
          if(Integer.valueOf(d)==0)
          {
              System.out.println("分母不能为0");
          }
          else
          {
              numerator=Integer.valueOf(n);
              denominator=Integer.valueOf(d);
              Simplify();
          }
      }
      else
          System.out.println("输入错误");
  }
  private void Simplify()//化简
  {
      int flag=1;
      if(numerator<0)
      {
          numerator*=-1;
          flag*=-1;
      }
      if(denominator<0)
      {
          denominator*=-1;
          flag*=-1;
      }
      int hcf=HCF(numerator,denominator);
      numerator=flag*numerator/hcf;
      denominator/=hcf;
  }
  private int HCF(int a,int b)//最大公因数
  {
      if(a<b)
      {
          int t=b;
          b=a;
          a=t;
      }
      while(b!=0)
      {
          int c=a%b;
          a=b;
          b=c;
      }
      return a;
  }

    public int GetNumerator()
    {
        return numerator;
    }
    public int GetDenominator()
    {
        return denominator;
    }
    public String toString()
    {
        return String.valueOf(numerator)+"/"+String.valueOf(denominator);
    }

    public Rational Add(Rational x)
    {
        int numerator=this.numerator*x.denominator+this.denominator*x.numerator;
        int denominator=this.denominator*x.denominator;
        return new Rational(numerator+"/"+denominator);
    }
    public Rational Subtract(Rational x)
    {
        int numerator=this.numerator*x.denominator-this.denominator*x.numerator;
        int denominator=this.denominator*x.denominator;
        return new Rational(numerator+"/"+denominator);
    }
    public Rational Multiply(Rational x)
    {
        int numerator=this.numerator*x.numerator;
        int denominator=this.denominator*x.denominator;
        return new Rational(numerator+"/"+denominator);
    }
    public Rational Divide(Rational x)
    {
        int numerator=this.numerator*x.denominator;
        int denominator=this.denominator*x.numerator;
        return new Rational(numerator+"/"+denominator);
    }
    public Rational Abs()
    {
        return new Rational(Math.abs(numerator)+"/"+denominator);
    }
    public boolean GetSign()//正为true,负为false
    {
        return numerator>=0;
    }
    public boolean Equals(Rational x)
    {
        return numerator==x.numerator&&denominator==x.denominator;
    }
    public boolean CompareTo(Rational x)
    {
        if(numerator>=0&&x.numerator<0)
            return true;
        if(numerator<0&&x.numerator>=0)
            return false;
        return numerator*x.denominator>denominator*x.numerator;
    }
    public Rational GetMax(Rational x)
    {
        return this.CompareTo(x)?this:x;
    }
    public Rational GetMin(Rational x)
    {
        return this.CompareTo(x)?x:this;
    }

    public int ToInt()
    {
        return numerator/denominator;
    }
    public double ToDouble()
    {
        return (double) numerator/denominator;
    }
}