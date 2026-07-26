
public class HelloWorld {
	
	
	public int SayX(int X){
		if(X==1) 
			return X;
		else
			return X*X;
	}

	public static void main(String[] args) {
		int x1=1;
		int x2=2;
		HelloWorld hw=new HelloWorld();
		System.out.println("this X is "+hw.SayX(x1));
		System.out.println("this X is "+hw.SayX(x2));

	}

}
