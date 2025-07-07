package com.example.lab10_3;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;

import java.util.Random;

public class HelloController {
    @FXML private TextField pointTextField;
    @FXML private TextField gradeTextField;
    @FXML private Button boButton;

    private Random ran=new Random();
    private String point="";
    public void initialize()
    {
        boButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent actionEvent) {
                int[] res=new int[6];
                for(int i=0;i<6;i++)
                {
                    res[ran.nextInt(6)]++;
                }
                point="";
                for(int i:res)
                {
                    point+=i+" ";
                }
                pointTextField.setText(point);
                if(res[3]==4&&res[0]==2)
                {
                    gradeTextField.setText("状元插金花");
                }
                else if(res[0]==6)
                {
                    gradeTextField.setText("遍地锦");
                }
                else if(res[1]==6||res[2]==6||res[4]==6||res[5]==6)
                {
                    gradeTextField.setText("六勃黑");
                }
                else if(res[3]==6)
                {
                    gradeTextField.setText("六勃红");
                }
                else if(res[3]==5)
                {
                    gradeTextField.setText("五王");
                }
                else if(res[1]==5||res[2]==5||res[4]==5||res[5]==5)
                {
                    if(res[3]==1)
                    {
                        gradeTextField.setText("五子带一秀");
                    }
                    else
                    {
                        gradeTextField.setText("五子");
                    }
                }
                else if(res[3]==4)
                {
                    gradeTextField.setText("四红");
                }
                else if(res[0]==1&&res[1]==1&&res[2]==1&&res[3]==1&&res[4]==1&&res[5]==1)
                {
                    gradeTextField.setText("对堂");
                }
                else if(res[3]==3)
                {
                    gradeTextField.setText("三红");
                }
                else if(res[0]==4||res[1]==4||res[2]==4||res[4]==4||res[5]==4)
                {
                    if(res[3]==1)
                    {
                        gradeTextField.setText("四进带一秀");
                    }
                    else if(res[3]==2)
                    {
                        gradeTextField.setText("四进带二举");
                    }
                    else
                    {
                        gradeTextField.setText("四进");
                    }
                }
                else if(res[3]==2)
                {
                    gradeTextField.setText("二举");
                }
                else if(res[3]==1)
                {
                    gradeTextField.setText("一秀");
                }
                else
                {
                    gradeTextField.setText("你没有获奖");
                }
            }
        });
    }
}