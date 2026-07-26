package xmu.software.testlessnon.testlab1;

public class Subsection
{
    public static int getValue(int d) {
        if (d == 0) {
            return 100;
        } else if (d < -2) {
            return Math.abs(d);
        } else if (d >= -2 && d < 2) {
            return d * d;
        } else { //d >= 2
            if (d > 32) {
                return Integer.MAX_VALUE;
            }
            return d * d * d;
        }
    }

    public static int sign(double d) {
        if (d < 0) {
            return -1;
        } else if (d > 0) {
            return 1;
        } else {
            return 0;
        }
    }
}
