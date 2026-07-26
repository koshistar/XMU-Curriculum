package xmu.software.testlessnon.testlab1;

public class StringUtils
{
    public static String[] string2Array(String str, char splitChar,
                                        boolean trim) {
        if (str == null) {
            return null;
        } else {
            String tempStr = str;
            int arraySize = 0; //数组大小
            String[] resultArr = null;
            if (trim) { //如果需要删除头尾多余的分隔符
                tempStr = trim(str, splitChar);
            }
            arraySize = getCharCount(tempStr, splitChar) + 1;
            resultArr = new String[arraySize];
            int fromIndex = 0, endIndex = 0;
            for (int i = 0; i < resultArr.length; i++) {
                endIndex = tempStr.indexOf(splitChar, fromIndex);
                if (endIndex == -1) {
                    resultArr[i] = tempStr.substring(fromIndex);
                    break;
                }
                resultArr[i] = tempStr.substring(fromIndex, endIndex);
                fromIndex = endIndex + 1;
            }
            return resultArr;
        }
    }

    //将字符串前面和后面的多余分隔符去除掉。
    private static String trim(String str, char splitChar) {
        int beginIndex = 0, endIndex = str.length();
        for (int i = 0; i < str.length(); i++) {
            if (str.charAt(i) != splitChar) {
                beginIndex = i;
                break;
            }
        }
        for (int i = str.length(); i > 0; i--) {
            if (str.charAt(i - 1) != splitChar) {
                endIndex = i;
                break;
            }
        }
        return str.substring(beginIndex, endIndex);
    }

    //计算字符串中分隔符中个数
    private static int getCharCount(String str, char splitChar) {
        int count = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str.charAt(i) == splitChar) {
                count++;
            }
        }
        return count;
    }

    public static void main(String[] args) {
        String[] s = StringUtils.string2Array("a,b,c", ',', false);
        System.out.println("s:" + s.length + " item:" + s[0]);
        System.out.println("s:" + s.length + " item:" + s[1]);
        System.out.println("s:" + s.length + " item:" + s[2]);

    }
}
