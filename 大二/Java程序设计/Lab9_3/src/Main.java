import java.util.Scanner;

public class Main {
    public static  boolean isNext(Character c1,Character c2)
    {
        switch(c1)
        {
            case 'q':
                if(c2=='w'||c2=='a')
                    return true;
                else
                    return false;
            case 'w':
                if(c2=='q'||c2=='e'||c2=='s'||c2=='a')
                    return true;
                else
                    return false;
            case 'e':
                if(c2=='w'||c2=='r'||c2=='d'||c2=='s')
                    return true;
                else
                    return false;
            case 'r':
                if(c2=='e'||c2=='t'||c2=='f'||c2=='d')
                    return true;
                else
                    return false;
            case 't':
                if(c2=='r'||c2=='y'||c2=='g'||c2=='f')
                    return true;
                else
                    return false;
            case 'y':
                if(c2=='t'||c2=='u'||c2=='h'||c2=='g')
                    return true;
                else
                    return false;
            case 'u':
                if(c2=='y'||c2=='i'||c2=='j'||c2=='h')
                    return true;
                else
                    return false;
            case 'i':
                if(c2=='u'||c2=='o'||c2=='j'||c2=='k')
                    return true;
                else
                    return false;
            case 'o':
                if(c2=='i'||c2=='p'||c2=='k'||c2=='l')
                    return true;
                else
                    return false;
            case 'p':
                if(c2=='o'||c2=='l')
                    return true;
                else
                    return false;
            case 'a':
                if(c2=='q'||c2=='w'||c2=='s'||c2=='z')
                    return true;
                else
                    return false;
            case 's':
                if(c2=='a'||c2=='w'||c2=='e'||c2=='z'||c2=='d'||c2=='x')
                    return true;
                else
                    return false;
            case 'd':
                if(c2=='e'||c2=='r'||c2=='s'||c2=='f'||c2=='x'||c2=='c')
                    return true;
                else
                    return false;
            case 'f':
                if(c2=='r'||c2=='t'||c2=='d'||c2=='g'||c2=='c'||c2=='v')
                    return true;
                else
                    return false;
            case 'g':
                if(c2=='t'||c2=='y'||c2=='f'||c2=='h'||c2=='v'||c2=='b')
                    return true;
                else
                    return false;
            case 'h':
                if(c2=='y'||c2=='u'||c2=='g'||c2=='j'||c2=='b'||c2=='n')
                    return true;
                else
                    return false;
            case 'j':
                if(c2=='u'||c2=='i'||c2=='h'||c2=='k'||c2=='n'||c2=='m')
                    return true;
                else
                    return false;
            case 'k':
                if(c2=='i'||c2=='o'||c2=='j'||c2=='l'||c2=='m')
                    return true;
                else
                    return false;
            case 'l':
                if(c2=='o'||c2=='p'||c2=='k')
                    return true;
                else
                    return false;
            case 'z':
                if(c2=='a'||c2=='s'||c2=='x')
                    return true;
                else
                    return false;
            case 'x':
                if(c2=='z'||c2=='s'||c2=='d'||c2=='c')
                    return true;
                else
                    return false;
            case 'c':
                if(c2=='x'||c2=='d'||c2=='f'||c2=='v')
                    return true;
                else
                    return false;
            case 'v':
                if(c2=='c'||c2=='f'||c2=='g'||c2=='b')
                    return true;
                else
                    return false;
            case 'b':
                if(c2=='v'||c2=='g'||c2=='h'||c2=='n')
                    return true;
                else
                    return false;
            case 'n':
                if(c2=='b'||c2=='h'||c2=='j'||c2=='m')
                    return true;
                else
                    return false;
            case 'm':
                if(c2=='n'||c2=='j'||c2=='k')
                    return true;
                else
                    return false;
            default:
                return false;
        }
    }
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        String[] wordList={"bloody","chaos","black","requiem","taboo","jihad","order","gungnir","longinus","excalibur","armageddon","sin","guilty","punishment","overload"};
        Scanner in=new Scanner(System.in);
        String input=in.next();
        for(String word:wordList)
        {
            if(word.equals(input))
            {
                System.out.println("Word is spelled correctly.");
                return;
            }
        }
        System.out.println("Word is not spelled correctly.");
        for(String word:wordList)
        {
            if(word.length()==input.length()) {
                int cnt=0;
                int cnt2=0;
                for (int i = 0; i < word.length(); ++i) {
                    if (word.charAt(i)!=input.charAt(i))
                    {
                        if(isNext(word.charAt(i),input.charAt(i)))
                        {
                            cnt2++;
                            if(cnt2==1)
                            {
                                break;
                            }
                        }
                        if(i<word.length()-1&&word.charAt(i)==input.charAt(i+1)&&word.charAt(i+1)==input.charAt(i))
                        {
                            cnt++;
                            if(cnt>1)
                            {
                                break;
                            }
                            i++;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                if(cnt==1||cnt2==1)
                {
                    System.out.println("Did you mean \""+word+"\"?");
                    return;
                }
            }
            else if(word.length()+1==input.length())
            {
//                System.out.println("case2");
                int cnt=0;
                for(int i=0,j=0;i<input.length();i++,j++)
                {
                    if(word.charAt(j)!=input.charAt(i))
                    {
                        if(cnt==1)
                        {
                            cnt=2;
                            break;
                        }
                        if(i==0)
                        {
                            break;
                        }
                        if(input.charAt(i-1)==input.charAt(i)&&input.charAt(i+1)==word.charAt(j))
                        {
                            cnt++;
                            i++;
                        }
                    }
                    if(i==j&&j==word.length()-1)
                    {
                        if(input.charAt(i+1)==word.charAt(j))
                        {
                            System.out.println("Did you mean \""+word+"\"?");
                            return;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
//                System.out.println(cnt);
                if(cnt==1)
                {
                    System.out.println("Did you mean \""+word+"\"?");
                    return;
                }
            }
        }
    }
}