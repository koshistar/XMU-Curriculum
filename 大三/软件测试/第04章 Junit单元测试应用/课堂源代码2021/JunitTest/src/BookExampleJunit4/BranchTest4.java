package BookExampleJunit4;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import BookExample.BookExample;

public class BranchTest4 {
	private int A1,A2,A3,A4,B1,B2,B3,B4,X1,X2,X3,X4,R1,R2,R3,R4;
	private BookExample be;

	@Before
	public void setUp() throws Exception {
		A1=2;B1=0;X1=3;R1=2;
		A2=1;B2=0;X2=1;R2=1;
		A3=3;B3=0;X3=3;R3=1;
		A4=2;B4=1;X4=2;R4=3;
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
		assertEquals(R4,be.ComputeX(A4, B4, X4));
	}

}
