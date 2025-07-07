package org.example.notebook;

import com.sun.tools.javac.Main;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.ObservableSet;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;

import java.awt.*;
import java.net.URL;
import java.util.ResourceBundle;

public class FontController implements Initializable {
    public Global global;
    public NotebookController controller;
    @FXML private ListView<String> fontListView;
    @FXML private ListView<String> glyphListView;
    @FXML private ListView<String> sizeListView;
    @FXML private TextField fontTextField;
    @FXML private TextField glyphTextField;
    @FXML private TextField sizeTextField;
    @FXML private TextArea fontShowTextArea;

    private static final String ChineseShowText="组一辈子乐队";
    private static final String EnglishShowText="Make a lifetime band";
    private static final String JapaneseShowText="バント、いじょやろ";
    private static final String NumberShowText="114514";

    private ObservableList<String> sizeArray;;
//    private final int[] sizeIntArray={8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,36,48,72}
    private Font font;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        global=(Global) MainApplication.globals.get("global");
        controller=(NotebookController)  MainApplication.controllers.get(NotebookController.class.getSimpleName());

        GraphicsEnvironment e=GraphicsEnvironment.getLocalGraphicsEnvironment();

        ObservableList<String> fontArray=FXCollections.observableArrayList(e.getAvailableFontFamilyNames());
        ObservableList<String> glyphArray= FXCollections.observableArrayList("Light","Regular","Bold");
        sizeArray=FXCollections.observableArrayList("8","9","10","11","12","14","16","18","20","22","24","26","28","36","48","72");

        fontListView.setItems(fontArray);
        glyphListView.setItems(glyphArray);
        sizeListView.setItems(sizeArray);

        fontTextField.textProperty().bind(fontListView.getSelectionModel().selectedItemProperty());
        glyphTextField.textProperty().bind(glyphListView.getSelectionModel().selectedItemProperty());
        sizeTextField.textProperty().bind(sizeListView.getSelectionModel().selectedItemProperty());

        fontListView.getSelectionModel().select(global.font.getName());
        glyphListView.getSelectionModel().select(fontWeightToString(global.fontWeight));
        sizeListView.getSelectionModel().select(String.valueOf(global.size));

        setFont();

        fontListView.getSelectionModel().selectedItemProperty().addListener((observable,oldValue,newValue)->setFont());
        glyphListView.getSelectionModel().selectedItemProperty().addListener((observable,oldValue,newValue)->setFont());
        sizeListView.getSelectionModel().selectedItemProperty().addListener((observable,oldValue,newValue)->setFont());
    }

    private void setFont()
    {
        double fontSize=11;
        if(sizeListView.getSelectionModel().getSelectedItem()!=null)
            fontSize=Double.parseDouble(sizeListView.getSelectionModel().getSelectedItem());
        if(glyphListView.getSelectionModel().getSelectedItem().equals("Regular"))
        {
            font=Font.font(String.valueOf(fontListView.getSelectionModel().getSelectedItem()), FontWeight.NORMAL, FontPosture.REGULAR,fontSize);
            global.fontWeight=FontWeight.NORMAL;
            global.fontPosture=FontPosture.REGULAR;
        }
        if(glyphListView.getSelectionModel().getSelectedItem().equals("Bold"))
        {
            font=Font.font(String.valueOf(fontListView.getSelectionModel().getSelectedItem()), FontWeight.BOLD, FontPosture.REGULAR,fontSize);
            global.fontWeight=FontWeight.BOLD;
            global.fontPosture=FontPosture.REGULAR;
        }
        if(glyphListView.getSelectionModel().getSelectedItem().equals("Light"))
        {
            font=Font.font(String.valueOf(fontListView.getSelectionModel().getSelectedItem()), FontWeight.LIGHT, FontPosture.REGULAR,fontSize);
            global.fontWeight=FontWeight.LIGHT;
            global.fontPosture=FontPosture.REGULAR;
        }

        global.font=font;
        global.fontFamily=fontListView.getSelectionModel().getSelectedItem();
        global.size=fontSize;
        fontShowTextArea.setFont(font);
        fontShowTextArea.setText(ChineseShowText+"\n"+EnglishShowText+"\n"+JapaneseShowText+"\n"+NumberShowText);
    }

    private String fontWeightToString(FontWeight fontWeight)
    {
        switch (fontWeight)
        {
            case FontWeight.NORMAL :return "Regular";
            case FontWeight.BOLD:return "Bold";
            case FontWeight.LIGHT:return "Light";
        }
        return null;
    }
    @FXML void onFontInterfaceCancel()
    {
        controller.fontChooseClose();
    }
    @FXML void onFontInterfaceConfirm()
    {
        controller.fontChooseConfirm();
        controller.fontChooseClose();
    }
}
