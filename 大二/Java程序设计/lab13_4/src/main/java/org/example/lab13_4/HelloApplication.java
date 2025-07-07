package org.example.lab13_4;

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
import java.util.concurrent.*;

public class HelloApplication extends Application {
    private Circle ball=new Circle(50, Color.BLUE);
    private double speedX=0;
    private double speedY=0;
    private Random ran=new Random();
    private ExecutorService executorService= Executors.newSingleThreadExecutor();
    private Future<?> animationFuture;
    @Override
    public void start(Stage stage) throws IOException {
//        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        ball.setVisible(false);
        Pane pane=new Pane(ball);
        Scene scene = new Scene(pane, 600, 400);
        scene.setOnMouseClicked(mouseEvent -> {
            if(animationFuture!=null)
            {
                animationFuture.cancel(true);
            }
            ball.setCenterX(mouseEvent.getX());
            ball.setCenterY(mouseEvent.getY());
            speedX=(ran.nextDouble()*2-1)*5.0;
            speedY=(ran.nextDouble()*2-1)*5.0;
            ball.setVisible(true);

            animationFuture=executorService.submit(()->{
                    while(!Thread.currentThread().isInterrupted())
                    {
                        try {
                            ball.setCenterX(ball.getCenterX() + speedX);
                            ball.setCenterY(ball.getCenterY() + speedY);
                            double r = ball.getRadius();
                            double w = scene.getWidth();
                            double h = scene.getHeight();
                            if (ball.getCenterX() <= r || ball.getCenterX() >= w - r) {
                                speedX = -speedX;
                                //防止卡边界
                                if (ball.getCenterX() <= r)
                                    ball.setCenterX(r + 1);
                                if (ball.getCenterX() >= w - r)
                                    ball.setCenterX(w - r - 1);
                            }
                            if (ball.getCenterY() <= r || ball.getCenterY() >= h - r) {
                                speedY = -speedY;
                                //防止卡边界
                                if (ball.getCenterY() <= r)
                                    ball.setCenterY(r + 1);
                                if (ball.getCenterY() >= h - r)
                                    ball.setCenterY(h - r - 1);
                            }
                            TimeUnit.MILLISECONDS.sleep(16);
                        }catch (InterruptedException e)
                        {
                            Thread.currentThread().interrupt();
                        }
                    }
            });
        });
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    @Override
    public void stop() throws Exception {
        executorService.shutdownNow();
        try {
            if(!executorService.awaitTermination(1, TimeUnit.SECONDS))
            {
                executorService.shutdownNow();
            }
        }catch (InterruptedException e)
        {
            executorService.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }

    public static void main(String[] args) {
        launch();
    }
}