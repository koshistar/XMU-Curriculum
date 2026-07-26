import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class BookExamTest4 {
        BookExam bookExam=new BookExam();

        // 分支-条件覆盖
        @Test
        public void testBranchConditionCoverage1() {
                // 分支1真，分支2真
                assertEquals(3, bookExam.ComputeX(2, 0, 4));
        }
        @Test
        public void testBranchConditionCoverage2() {
                // 分支1假，分支2假
                assertEquals(1, bookExam.ComputeX(1, 1, 1));
        }
        @Test
        public void testBranchConditionCoverage3() {
                // 分支1真，分支2假
                assertEquals(1, bookExam.ComputeX(3, 0, 3)); // 3/3=1 → 不满足X>1
        }
        @Test
        public void testBranchConditionCoverage4() {
                // 分支1假，分支2真
                assertEquals(6, bookExam.ComputeX(1, 2, 5));
        }
}