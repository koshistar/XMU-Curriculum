package com.example.lab10_4;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.ChoiceDialog;
import javafx.scene.control.Label;
import javafx.scene.control.TextInputDialog;

public class HelloController {
    @FXML
    private void showInformationDialogueBox()
    {
        Alert alert=new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("信息对话框");
        alert.setHeaderText("这是一个信息对话框");
        alert.setContentText("这是对话框的内容");
        alert.showAndWait();
    }
    @FXML
    private void showWarningDialogueBox()
    {
        Alert alert=new Alert(Alert.AlertType.WARNING);
        alert.setTitle("警告对话框");
        alert.setHeaderText("这是一个警告对话框");
        alert.setContentText("这是对话框的内容");
        alert.showAndWait();
    }
    @FXML
    private void showErrorDialogueBox()
    {
        Alert alert=new Alert(Alert.AlertType.ERROR);
        alert.setTitle("错误对话框");
        alert.setHeaderText("这是一个错误对话框");
        alert.setContentText("这是对话框的内容");
        alert.showAndWait();
    }
    @FXML
    private void showConformationDialogueBox()
    {
        Alert alert=new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle("确认对话框");
        alert.setHeaderText("这是一个确认对话框");
        alert.setContentText("这是对话框的内容");
        alert.showAndWait();
    }
    @FXML
    private void showChoiceDialogueBox()
    {
        ChoiceDialog<String> dialogue=new ChoiceDialog<>("选项1","选项1","选项2","选项3");
        dialogue.setTitle("选择对话框");
        dialogue.setHeaderText("请选择一个选型");
        dialogue.setContentText("这是选项内容");
        dialogue.showAndWait().ifPresent(choice->{System.out.println("你选择了"+choice);});
    }
    @FXML
    private void showInputDialogueBox()
    {
        TextInputDialog dialogue=new TextInputDialog("默认值");
        dialogue.setTitle("输入对话框");
        dialogue.setHeaderText("请输入文本");
        dialogue.setContentText("文本内容");
        dialogue.showAndWait().ifPresent(input->System.out.println("用户输入了"+input));
    }
}