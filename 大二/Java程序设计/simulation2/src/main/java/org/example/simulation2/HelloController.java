package org.example.simulation2;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.stage.FileChooser;

import javax.swing.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class HelloController {
    @FXML private TextArea textArea;
    @FXML private void onOpen() throws FileNotFoundException {
        JFileChooser jfileChooser=new JFileChooser();
        jfileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        int result=jfileChooser.showOpenDialog(null);
        File file=null;
        if(result==JFileChooser.APPROVE_OPTION)
        {
            file=jfileChooser.getSelectedFile();
        }
        File[] files=file.listFiles();
        int[] count=new int[26];
        for(File f:files)
        {
            if(f.getName().endsWith(".txt"))
            {
                textArea.appendText(f.getAbsolutePath()+"\n");
                String content=new Scanner(f).useDelimiter("\\Z").next();
                for(int i=0;i<content.length();i++)
                {
                    char c=Character.toLowerCase(content.charAt(i));
                    if(c>='a'&&c<='z')
                        count[c-'a']++;
                }
            }
        }
        for(int i=0;i<26;i++)
        {
            if(count[i]!=0)
            {
                textArea.appendText((char)(i+'a')+"字符出现"+count[i]+"次\n");
            }
        }
        File resultFile=new File(HelloController.class.getResource("").getPath()+"结果.txt");
        try {
            resultFile.createNewFile();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        String fileText=textArea.getText();
        try {
            FileWriter fileWriter=new FileWriter(resultFile);
            fileWriter.write(fileText);
            fileWriter.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}