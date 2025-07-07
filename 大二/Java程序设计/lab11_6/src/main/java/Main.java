import org.apache.poi.xwpf.usermodel.*;
import org.openxmlformats.schemas.wordprocessingml.x2006.main.CTBody;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

public class Main {
    public static void main(String[] args)
    {
        String inputPath="37220232203780+马鑫.docx";
        String outputPath="output.docx";
        try {
            //读取word文档
            FileInputStream fis=new FileInputStream(inputPath);
            XWPFDocument document=new XWPFDocument(fis);

            //修改文档内容
            List<XWPFParagraph> paragraphs=document.getParagraphs();
            for(XWPFParagraph paragraph:paragraphs)
            {
                String text=paragraph.getText();
                if(text!=null&&!text.isEmpty())
                {
                    String newText=text.replace("代码","code");
                    for(int i=paragraph.getRuns().size()-1;i>=0;i--)
                    {
                        paragraph.removeRun(i);
                    }
                    XWPFRun newRun=paragraph.createRun();
                    newRun.setText(newText);
                }
            }

            //保存文档内容
            FileOutputStream fos=new FileOutputStream(outputPath);
            document.write(fos);
            fos.close();
            document.close();
        }
        catch (IOException e)
        {
            System.err.println("ERROR!");
            e.printStackTrace();
        }
    }
}
