package BookExample;

import junit.framework.Test;
import junit.framework.TestSuite;

public class Junit3TestSuite {

	public static Test suite() {
		TestSuite suite = new TestSuite(Junit3TestSuite.class.getName());
		//$JUnit-BEGIN$
		suite.addTestSuite(BranchTest.class);
		suite.addTestSuite(ConditionTest.class);
		suite.addTestSuite(PathTest.class);
		suite.addTestSuite(StatementTest.class);
		//$JUnit-END$
		return suite;
	}

}
