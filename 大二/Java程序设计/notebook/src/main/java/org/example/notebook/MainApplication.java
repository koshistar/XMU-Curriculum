package org.example.notebook;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class MainApplication extends Application {
    public static Map<String,Object> controllers=new HashMap<>();
    public static Map<String,Object> globals=new HashMap<>();

    Global global=new Global();
    @Override
    public void start(Stage stage) throws IOException {
        globals.put("global",global);
        FXMLLoader fxmlLoader = new FXMLLoader(MainApplication.class.getResource("MainMenu.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 800, 600);
        stage.setTitle("无标题 - 记事本");
        stage.setScene(scene);
        stage.getIcons().add(new Image("file:src/main/java/org/example/notebook/icon.png"));
        stage.show();

        NotebookController controller=fxmlLoader.getController();
        controller.setStage(stage);
        controller.zoomInit();
        stage.setOnCloseRequest(event->{
            if(controller.getStatus()!= NotebookController.Status.alreadyServe)
            {
                if(controller.getCurrentPath()==null)
                {
                    Alert alert=new Alert(Alert.AlertType.CONFIRMATION);
                    alert.setTitle("确认");
                    alert.setHeaderText("要将更改保存到 无标题 吗？");
                    Optional<ButtonType> result=alert.showAndWait();

                    if(result.get()==ButtonType.OK)
                    {
                        if(controller.onSaveAsFile())
                        {
                            stage.close();
                        }
                    }
                    else if(result.get()==ButtonType.CANCEL)
                    {
                        stage.close();
                    }
                }
                else
                {
                    Alert alert=new Alert(Alert.AlertType.CONFIRMATION);
                    alert.setTitle("确认");
                    alert.setHeaderText("要将更改保存到 "+controller.getCurrentPath()+" 吗？");
                    Optional<ButtonType> result=alert.showAndWait();

                    if(result.get()==ButtonType.OK)
                    {
                        controller.onSaveFile();
                        stage.close();
                    }
                    else if(result.get()==ButtonType.CANCEL)
                    {
                        stage.close();
                    }
                }
            }
            event.consume();
        });
    }

    public static void main(String[] args) {
        launch();
    }
}