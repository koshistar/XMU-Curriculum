package com.example.lab4_4;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextInputDialog;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Optional;
import java.util.Random;

public class HelloApplication extends Application {
    private final int width=1920;
    private final int height=1080;
    private final int maxNum=20;
    private int curcnt=0;
    private final String inputError="输入错误";
    @Override
    public void start(Stage stage) throws IOException {
        Canvas canvas=new Canvas(width,height);
        GraphicsContext gc=canvas.getGraphicsContext2D();
        StackPane pane=new StackPane();
        pane.getChildren().add(canvas);
        Scene scene=new Scene(pane,width,height);
        scene.addEventHandler(KeyEvent.KEY_PRESSED,event->{
            if(event.getCode()== KeyCode.ESCAPE)
            {
                stage.close();
            }
        });
        stage.setTitle("Lab4_4");
        stage.setScene(scene);
        stage.show();
        DrawShapes(gc);
    }

    private void DrawShapes(GraphicsContext gc)
    {
        Random ran=new Random();
        while(curcnt<maxNum)
        {
            switch (ran.nextInt(3))
            {
                case 0:DrawLine(gc);break;
                case 1:DrawRectangle(gc);break;
                case 2:DrawEllipse(gc);break;
            }
            curcnt++;
        }
    }
    private void DrawLine(GraphicsContext gc)
    {
        TextInputDialog dialog=new TextInputDialog();
        dialog.setHeaderText("请输入两个顶点的坐标(x1,y1,x2,y2):");
        Optional<String> res=dialog.showAndWait();
        if(res.isPresent())
        {
            String[] params=res.get().split(",");
            if(params.length==4)
            {
                try
                {
                    Double x1=Double.parseDouble(params[0].trim());
                    Double y1=Double.parseDouble(params[1].trim());
                    Double x2=Double.parseDouble(params[2].trim());
                    Double y2=Double.parseDouble(params[3].trim());
                    if(x1>=0&&x1<=width&&x2>=0&&x2<=width&&y1>=0&&y1<=height&&y2>=0&&y2<=height)
                    {
                        gc.strokeLine(x1,y1,x2,y2);
                    }
                    else
                    {
                        ShowWarning(inputError,"参数超出范围");
                    }
                }
                catch (NumberFormatException e)
                {
                    ShowWarning(inputError,"请输入浮点数或整数");
                }
            }
            else
            {
                ShowWarning(inputError,"请输入四个参数");
            }
        }
    }
    private void DrawRectangle(GraphicsContext gc)
    {
        TextInputDialog dialog=new TextInputDialog();
        dialog.setHeaderText("请输入矩形的坐标和宽高(x,y,width,height):");
        Optional<String> res= dialog.showAndWait();
        if(res.isPresent())
        {
            String[] params=res.get().split(",");
            if(params.length==4)
            {
                try
                {
                    Double x=Double.parseDouble(params[0].trim());
                    Double y=Double.parseDouble(params[1].trim());
                    Double w=Double.parseDouble(params[2].trim());
                    Double h=Double.parseDouble(params[3].trim());
                    if(x>=0&&x<=width&&y>=0&&y<=height)
                    {
                        gc.strokeRect(x,y,w,h);
                    }
                    else
                    {
                        ShowWarning(inputError,"参数超出范围");
                    }
                }
                catch (Exception e)
                {
                    ShowWarning(inputError,"请输入浮点数或整数");
                }
            }
            else
            {
                ShowWarning(inputError,"请输入四个参数");
            }
        }
    }
    private void DrawEllipse(GraphicsContext gc)
    {
        TextInputDialog dialog=new TextInputDialog();
        dialog.setHeaderText("请输入椭圆的坐标与长短轴(x,y,a,b):");
        Optional<String> res=dialog.showAndWait();
        if(res.isPresent())
        {
            String[] params=res.get().split(",");
            if(params.length==4)
            {
                try
                {
                    Double x=Double.parseDouble(params[0].trim());
                    Double y=Double.parseDouble(params[1].trim());
                    Double a=Double.parseDouble(params[2].trim());
                    Double b=Double.parseDouble(params[3].trim());
                    if(x>=0&&x<=width&&y>=0&&y<=height)
                    {
                        gc.strokeOval(x,y,a,b);
                    }
                    else
                    {
                        ShowWarning(inputError,"参数超出范围");
                    }
                }
                catch (Exception e)
                {
                    ShowWarning(inputError,"请输入浮点数或整数");
                }
            }
            else
            {
                ShowWarning(inputError,"请输入四个参数");
            }
        }
    }
    private void ShowWarning(String title,String content)
    {
        Alert alert=new Alert(Alert.AlertType.WARNING,content, ButtonType.OK);
        alert.setTitle(title);
        alert.showAndWait();
    }
    public static void main(String[] args) {
        launch();
    }
}