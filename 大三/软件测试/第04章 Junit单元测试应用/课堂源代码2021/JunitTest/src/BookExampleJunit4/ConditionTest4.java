package BookExampleJunit4;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import BookExample.BookExample;

public class ConditionTest4 {
	private int A1,A2,A3,B1,B2,B3,X1,X2,X3,R1,R2,R3;
	private BookExample be;
	
	@Before
	public void setUp() throws Exception {
		A1=2;B1=0;X1=3;R1=2;
		A2=1;B2=0;X2=1;R2=1;
		A3=2;B3=1;X3=1;R3=2;
		be=new BookExample();
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testComputeX() {
		assertEquals(R1,be.ComputeX(A1, B1, X1));
		assertEquals(R2,be.ComputeX(A2, B2, X2));
		assertEquals(R3,be.ComputeX(A3, B3, X3));
	}

}
