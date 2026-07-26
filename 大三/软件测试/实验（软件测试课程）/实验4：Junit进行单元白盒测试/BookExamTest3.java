import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class BookExamTest3 {
        BookExam bookExam=new BookExam();

        // 条件覆盖
        // 条件1：A>1，条件2：B==0，条件3：A==2，条件4：X>1
        @Test
        public void testConditionCoverage1()
        {
                // TTTT
                assertEquals(3,bookExam.ComputeX(2,0,4));
        }
        @Test
        public void testConditionCoverage2()
        {
                // FFFF
                assertEquals(1,bookExam.ComputeX(1,1,1));
        }
}