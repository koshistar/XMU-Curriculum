package org.example.notebook;

import java.io.*;

public class FileTools {
    public static String readFile(File file)
    {
        //拼接更加高效
        StringBuilder resultStr=new StringBuilder();
        try{
            //缓冲流读取文件更高效
            BufferedReader bufferedReader=new BufferedReader(new FileReader(file));
            String line=bufferedReader.readLine();
            while(line!=null)
            {
                resultStr.append(line+"\n");
                line=bufferedReader.readLine();
            }
            bufferedReader.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return  resultStr.toString();
    }
    public static void writeFile(File file,String str)
    {
        try{
            BufferedWriter bufferedWriter=new BufferedWriter(new FileWriter(file));
            bufferedWriter.write(str);
            bufferedWriter.close();
        }catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
