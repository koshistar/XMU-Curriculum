import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class BookExamTest2 {
        BookExam bookExam=new BookExam();

        // 分支覆盖
        @Test
        public void testBranchCoverage1()
        {
                // 第一个if： true，第二个if： true
                assertEquals(3,bookExam.ComputeX(2,0,4));
        }
        @Test
        public void testBranchCoverage2()
        {
                // 第一个if： false，第二个if： false
                assertEquals(1,bookExam.ComputeX(1, 1,1));
        }
}