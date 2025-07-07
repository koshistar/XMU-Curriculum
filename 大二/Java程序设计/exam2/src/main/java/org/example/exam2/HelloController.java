package org.example.exam2;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.stage.FileChooser;

import java.io.*;
import java.net.URL;
import java.util.*;

public class HelloController implements Initializable {
   @FXML private TextField queryTextField;
    @FXML private TextField numberTextField;
    @FXML private TextField nameTextField;
    @FXML private TextField occupationTextField;

    private FileChooser fileChooser=new FileChooser();
    private File currentFile;//当前编辑的文件
    private boolean isModified=false;

    Map<File,Teacher> teachers=new HashMap<>();//一个文件对应一个教工
    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("TextFiles","*.txt"));
    }
    public boolean getIsModified()
    {
        return isModified;
    }
    @FXML private void onTextModified()//检测文本是否被修改
    {
        isModified=true;
    }

    @FXML private void onOpen()
    {
        File file=fileChooser.showOpenDialog(null);
        if(file==null)//用户取消选择
            return;
        try {//对文本格式有严格要求，否则无法读入
            String text = readFile(file);
            String[] texts = text.split("\n");
            String number=null,name=null,occupation=null;
            for (String t : texts) {
                String[] s = t.split(":");
                if (s[0].equals("教工号"))
                {
                    number=s[1];
                }
                else if(s[0].equals("姓名"))
                {
                    name=s[1];
                }
                else if(s[0].equals("职称"))
                {
                    occupation=s[1];
                }
            }
            teachers.put(file,new Teacher(number,name,occupation));
            numberTextField.setText(number);
            nameTextField.setText(name);
            occupationTextField.setText(occupation);
            currentFile=file;
            isModified=false;
        }catch (Exception e)
        {
            showAlert("提示","文件不合法", Alert.AlertType.WARNING);
        }
    }
    @FXML private void onQuery()
    {
        String queryName=queryTextField.getText();
        if(queryName.length()==0)
        {
            showAlert("提示","查询不能为空", Alert.AlertType.WARNING);
            return;
        }
        if(teachers.isEmpty())
        {
            showAlert("提示","没有找到该用户", Alert.AlertType.INFORMATION);
            return;
        }
        for(Map.Entry<File,Teacher> entry:teachers.entrySet())//遍历查询是否有该教工
        {
            if(entry.getValue().name.equals(queryName))
            {
                numberTextField.setText(entry.getValue().number);
                nameTextField.setText(entry.getValue().name);
                occupationTextField.setText(entry.getValue().occupation);
                currentFile=entry.getKey();
                isModified=false;
                return;
            }
        }
        showAlert("提示","没有找到该用户", Alert.AlertType.INFORMATION);
    }
    @FXML private void onNew()
    {
        currentFile=null;
        numberTextField.clear();
        nameTextField.clear();
        occupationTextField.clear();
        isModified=false;
    }
    @FXML public void onSave() throws IOException {
        String resultText="教工号:"+numberTextField.getText()+"\n姓名:"+nameTextField.getText()+"\n职称:"+occupationTextField.getText();
        if (currentFile!=null)
        {
            FileWriter fileWriter=new FileWriter(currentFile);
            fileWriter.write(resultText);
            fileWriter.close();//关闭流
            teachers.put(currentFile,new Teacher(numberTextField.getText(),nameTextField.getText(),occupationTextField.getText()));
        }
        else
        {//当前文件路径下创建新文件
            File resultFile=new File(HelloController.class.getResource("").getPath()+"data.txt");
            resultFile.createNewFile();
            FileWriter fileWriter=new FileWriter(resultFile);
            fileWriter.write(resultText);
            fileWriter.close();//关闭流
            teachers.put(resultFile,new Teacher(numberTextField.getText(),nameTextField.getText(),occupationTextField.getText()));
        }
        isModified=false;
    }

    //读取文件流
    public String readFile(File file)
    {
        StringBuilder resultStr=new StringBuilder();
        try{
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
    private void showAlert(String title, String message, Alert.AlertType type)
    {
        Alert alert=new Alert(type);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
}