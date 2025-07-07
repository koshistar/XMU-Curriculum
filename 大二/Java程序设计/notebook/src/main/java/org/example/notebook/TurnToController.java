package org.example.notebook;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;

import java.net.URL;
import java.util.ResourceBundle;

public class TurnToController implements Initializable {
    @FXML private TextField turnToTextField;

    private Global global;
    private NotebookController controller;


    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        global=(Global) MainApplication.globals.get("global");
        controller=(NotebookController) MainApplication.controllers.get(NotebookController.class.getSimpleName());
    }

    @FXML private void onTurnToConfirm()
    {
        global.turnTo=Integer.parseInt(turnToTextField.getText());
        controller.onTurnTo();
        controller.onTurnToClose();
    }
    @FXML private void onTurnToCancel()
    {
        controller.onTurnToClose();
    }
}
