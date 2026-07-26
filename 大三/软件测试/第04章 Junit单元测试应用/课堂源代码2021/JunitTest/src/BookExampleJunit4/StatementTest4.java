package BookExampleJunit4;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import BookExample.BookExample;

public class StatementTest4 {
	private int A,B,X,R;
	private BookExample be;

	@Before
	//可以将setup修改为init
	public void setUp() throws Exception {
		A=2;B=0;X=3;R=2;
		be=new BookExample();
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	//与test反射机制无关，名称随便改，只要前面加上一个@Test
	public void testComputeX() {
		assertEquals(R,be.ComputeX(A, B, X));
	}

}
