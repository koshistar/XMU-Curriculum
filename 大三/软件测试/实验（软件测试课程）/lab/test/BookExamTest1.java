import org.junit.Test;

import static org.junit.Assert.*;

public class BookExamTest1 {
        BookExam bookExam=new BookExam();

        // 语句覆盖
        @Test
        public void testStatementCoverage()
        {
                // A>1，B=0 执行第一个if
                // A==2 执行第二个if
                assertEquals(3,bookExam.ComputeX(2,0,4));
        }
}