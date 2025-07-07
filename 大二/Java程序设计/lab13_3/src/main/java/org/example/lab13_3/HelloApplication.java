package org.example.lab13_3;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class HelloApplication extends Application {
    private ArrayList<Circle> circles=new ArrayList<Circle>();
    private ArrayList<Double> speedXs=new ArrayList<Double>();
    private ArrayList<Double> speedYs=new ArrayList<Double>();
    private Random ran=new Random();
//    private boolean isMoving;
    @Override
    public void start(Stage stage) throws IOException {
        //        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Pane pane=new Pane();
        Scene scene = new Scene(pane, 600, 400);
        scene.setOnMouseClicked(mouseEvent -> {
            Circle ball=new Circle(ran.nextDouble()*50,Color.color(ran.nextDouble(),ran.nextDouble(),ran.nextDouble()));
            ball.setCenterX(mouseEvent.getX());
            ball.setCenterY(mouseEvent.getY());
            double speedX=(ran.nextDouble()*2-1)*5.0;
            double speedY=(ran.nextDouble()*2-1)*5.0;
            pane.getChildren().add(ball);
            circles.add(ball);
            speedXs.add(speedX);
            speedYs.add(speedY);
//            isMoving=true;
        });
        new AnimationTimer()
        {
            @Override
            public void handle(long l) {
//                if(isMoving)
//                {
                    for(int i=0;i<circles.size();i++) {
                        Circle ball=circles.get(i);
                        ball.setCenterX(ball.getCenterX() + speedXs.get(i));
                        ball.setCenterY(ball.getCenterY() + speedYs.get(i));
                        double r = ball.getRadius();
                        double w = scene.getWidth();
                        double h = scene.getHeight();
                        if (ball.getCenterX() <= r || ball.getCenterX() >= w - r) {
                            speedXs.set(i,-speedXs.get(i));
                            //防止卡边界
                            if (ball.getCenterX() <= r)
                                ball.setCenterX(r + 1);
                            if (ball.getCenterX() >= w - r)
                                ball.setCenterX(w - r - 1);
                        }
                        if (ball.getCenterY() <= r || ball.getCenterY() >= h - r) {
                            speedYs.set(i,-speedYs.get(i));
                            //防止卡边界
                            if (ball.getCenterY() <= r)
                                ball.setCenterY(r + 1);
                            if (ball.getCenterY() >= h - r)
                                ball.setCenterY(h - r - 1);
                        }
                    }
                }
//            }
        }.start();
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}