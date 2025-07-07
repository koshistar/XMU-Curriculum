import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        String filePath="buy_price.xlsx";
        String outputPath="modified.xlsx";

        try
        {
            //读取excel
            FileInputStream fis=new FileInputStream(filePath);
            Workbook workbook=WorkbookFactory.create(fis);

            //修改内容
            Sheet sheet=workbook.getSheetAt(0);
            Row row=sheet.getRow(0);
            Cell cell=row.getCell(0);
            cell.setCellValue("114514");

            //保存修改
            FileOutputStream fos=new FileOutputStream(outputPath);
            workbook.write(fos);
        }
        catch (IOException e)
        {
            System.err.println("ERROR!");
            e.printStackTrace();
        }
    }
}
