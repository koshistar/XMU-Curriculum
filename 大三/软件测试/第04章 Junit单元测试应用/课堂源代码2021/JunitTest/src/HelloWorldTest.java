import junit.framework.TestCase;

public class HelloWorldTest extends TestCase {
	private int x1,x2,r1,r2;
	private HelloWorld hw;

	protected void setUp() throws Exception {
		super.setUp();
		x1=1;r1=1;
		x2=2;r2=4;
		hw=new HelloWorld();
		
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	public void testSayX() {
		assertEquals(r1,hw.SayX(x1));
		assertEquals(r2,hw.SayX(x2));
	}
}
