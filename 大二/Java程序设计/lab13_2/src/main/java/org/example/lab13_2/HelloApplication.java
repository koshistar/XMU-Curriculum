package org.example.lab13_2;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Random;

public class HelloApplication extends Application {
    private Circle ball=new Circle(50,Color.BLUE);
    private double speedX=0;
    private double speedY=0;
    private Random ran=new Random();
    private boolean isMoving;
    @Override
    public void start(Stage stage) throws IOException {
//        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        ball.setVisible(false);
        Pane pane=new Pane(ball);
        Scene scene = new Scene(pane, 600, 400);
        scene.setOnMouseClicked(mouseEvent -> {
            ball.setCenterX(mouseEvent.getX());
            ball.setCenterY(mouseEvent.getY());
            speedX=(ran.nextDouble()*2-1)*5.0;
            speedY=(ran.nextDouble()*2-1)*5.0;
            ball.setVisible(true);
            isMoving=true;
        });
        new AnimationTimer()
        {
            @Override
            public void handle(long l) {
                if(isMoving)
                {
                    ball.setCenterX(ball.getCenterX()+speedX);
                    ball.setCenterY(ball.getCenterY()+speedY);
                    double r=ball.getRadius();
                    double w=scene.getWidth();
                    double h=scene.getHeight();
                    if(ball.getCenterX()<=r||ball.getCenterX()>=w-r)
                    {
                        speedX=-speedX;
                        //防止卡边界
                        if(ball.getCenterX()<=r)
                            ball.setCenterX(r+1);
                        if(ball.getCenterX()>=w-r)
                            ball.setCenterX(w-r-1);
                    }
                    if(ball.getCenterY()<=r||ball.getCenterY()>=h-r)
                    {
                        speedY=-speedY;
                        //防止卡边界
                        if(ball.getCenterY()<=r)
                            ball.setCenterY(r+1);
                        if(ball.getCenterY()>=h-r)
                            ball.setCenterY(h-r-1);
                    }
                }
            }
        }.start();
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}