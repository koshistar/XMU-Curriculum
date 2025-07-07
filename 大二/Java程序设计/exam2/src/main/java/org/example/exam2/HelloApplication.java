package org.example.exam2;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Optional;

public class HelloApplication extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 600, 400);
        stage.setTitle("第二次上机考试");
        stage.setScene(scene);
        stage.show();

        HelloController controller=fxmlLoader.getController();
        //用户关闭窗口，询问用户是否将最新内容保存至文件，并关闭流
        stage.setOnCloseRequest(event->{
            if(controller.getIsModified()) {
                Alert alert=new Alert(Alert.AlertType.CONFIRMATION);
                alert.setTitle("确认");
                alert.setHeaderText("确认要保存吗");
                alert.setContentText("你有未保存的修改");
                Optional<ButtonType> result=alert.showAndWait();

                if(result.get()==ButtonType.OK)
                {
                    try {
                        controller.onSave();
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
                else if(result.get()==ButtonType.CANCEL)
                {
                    stage.close();
                }
            }
        });
    }

    public static void main(String[] args) {
        launch();
    }
}