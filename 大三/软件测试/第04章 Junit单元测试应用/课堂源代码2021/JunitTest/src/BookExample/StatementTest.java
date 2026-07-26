package BookExample;

import junit.framework.TestCase;

public class StatementTest extends TestCase {
	private int A,B,X,R1;
	float R2;
	private BookExample be;

	protected void setUp() throws Exception {
		super.setUp();
		A=2;B=0;X=3;R1=2;R2=(float) 2.0;
		be=new BookExample();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	public void testComputeX1() {

		assertEquals(R1,be.ComputeX(A, B, X));
		//比较浮点数。断言将会检查实际的计算结果是否等于2.0，但是该检查只精确到小数点的后一位
		//assertEquals(R2,(float)be.ComputeX(A, B, X),0.1);
	}
	public void testComputeX2() {

		assertEquals(4,be.ComputeX(A, 1, X));
	}

}
