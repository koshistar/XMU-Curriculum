package BookExampleJunit4;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({ BranchTest4.class, ConditionTest4.class, PathTest4.class, StatementTest4.class })
public class Junit4TestSuite {

}
