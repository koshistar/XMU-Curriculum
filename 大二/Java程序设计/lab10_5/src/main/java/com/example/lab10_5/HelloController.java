package com.example.lab10_5;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;

public class HelloController {
    @FXML
    private TextField userNameField;
    @FXML
    private PasswordField passwordField;
    @FXML
    private PasswordField confirmPasswordField;
    @FXML
    private void handleRegister()
    {
        String userName=userNameField.getText().trim();
        String password=passwordField.getText();
        String confirmPassword=confirmPasswordField.getText();
        if(userName.length()<4)
        {
            showAlert("用户名错误","用户名长度不能小于4");
            return;
        }
        if(password.isEmpty())
        {
            showAlert("密码错误","密码不能为空");
            return;
        }
        if(!confirmPassword.equals(password))
        {
            showAlert("密码错误","两次输入的密码不一致");
            return;
        }
        showAlert("注册成功","欢迎你 "+userName);
    }
    @FXML
    private  void handleCancel()
    {
        userNameField.clear();
        passwordField.clear();
        confirmPasswordField.clear();
    }
    private void showAlert(String title,String message)
    {
        Alert alert=new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setContentText(message);
        alert.showAndWait();
    }
}