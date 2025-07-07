package org.example.notebook;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;

import java.net.URL;
import java.util.ResourceBundle;

public class SearchController implements Initializable {
    @FXML private CheckBox ignoreCheckBox;
    @FXML private TextField searchTextField;
    @FXML private TextField replaceTextField;

    public Global global;
    public NotebookController controller;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        global=(Global) MainApplication.globals.get("global");
        controller=(NotebookController)MainApplication.controllers.get(NotebookController.class.getSimpleName());
    }

    @FXML private void onSearch()
    {
        global.search=searchTextField.getText();
        global.replace=replaceTextField.getText();
        global.isCaseIgnored=!ignoreCheckBox.isSelected();
        controller.onSearch();
    }
    @FXML private void onPreviousSearch()
    {
        global.search=searchTextField.getText();
        global.replace=replaceTextField.getText();
        global.isCaseIgnored=!ignoreCheckBox.isSelected();
        controller.onPreviousSearch();
    }
    @FXML private void onReplace()
    {
        global.search=searchTextField.getText();
        global.replace=replaceTextField.getText();
        global.isCaseIgnored=!ignoreCheckBox.isSelected();
        controller.onReplace();
    }
    @FXML private void onReplaceAll()
    {
        global.search=searchTextField.getText();
        global.replace=replaceTextField.getText();
        global.isCaseIgnored=!ignoreCheckBox.isSelected();
        controller.onReplaceAll();
    }
}
