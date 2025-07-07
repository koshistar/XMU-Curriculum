package org.example.notebook;

import com.sun.tools.javac.Main;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.scene.text.Font;

import javax.swing.*;
import java.awt.print.PageFormat;
import java.awt.print.PrinterJob;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Optional;
import java.util.ResourceBundle;

public class NotebookController extends Stage implements Initializable {
    @FXML private Stage stage;
    @FXML private Stage fontStage;
    @FXML private Stage turnToStage;

    private Global global;
    private Font font;
    private double size=20;
    private int zoomPercentage=100;

    public enum Status{createNew,alreadyChange,alreadyServe}
    private Status status=Status.createNew;

    @FXML private TextArea textArea;

    //状态栏
    @FXML private HBox stateBar;
    @FXML private Label cursorPositionLabel;
    @FXML private Label charCountLabel;
    @FXML private Label zoomLabel;
    @FXML private Label platformLabel;
    @FXML private Label encodingLabel;

    //文件菜单
    private String currentPath;
    private File currentFile;
    private final FileChooser fileChooser=new FileChooser();
    @FXML private MenuItem newFileMenuItem;
    @FXML private MenuItem openFileMenuItem;
    @FXML private MenuItem saveFileMenuItem;
    @FXML private MenuItem saveAsMenuItem;
    @FXML private MenuItem pageSetMenuItem;
    @FXML private MenuItem exitMenuItem;

    //编辑菜单
    @FXML private MenuItem undoMenuItem;
    @FXML private MenuItem redoMenuItem;
    @FXML private MenuItem cutMenuItem;
    @FXML private MenuItem copyMenuItem;
    @FXML private MenuItem pasteMenuItem;
    @FXML private MenuItem deleteMenuItem;
    @FXML private MenuItem findMenuItem;
    @FXML private MenuItem findNextMenuItem;
    @FXML private MenuItem findPreMenuItem;
    @FXML private MenuItem replaceMenuItem;
    @FXML private MenuItem gotoMenuItem;
    @FXML private MenuItem selectAllMenuItem;
    @FXML private MenuItem timeMenuItem;
    @FXML private MenuItem fontMenuItem;

    //右键菜单
    @FXML ContextMenu contextMenu;
    @FXML MenuItem undoContextMenuItem;
    @FXML MenuItem redoContextMenuItem;
    @FXML MenuItem cutContextMenuItem;
    @FXML MenuItem copyContextMenuItem;
    @FXML MenuItem pasteContextMenuItem;
    @FXML MenuItem deleteContextMenuItem;
    @FXML MenuItem selectAllContextMenuItem;

    //查看菜单
    @FXML private MenuItem zoomInMenuItem;
    @FXML private MenuItem zoomOutMenuItem;
    @FXML private CheckMenuItem wrapMenu;
    @FXML private CheckMenuItem stateMenu;
    @FXML private MenuItem helpMenuItem;
    private int startIndex=0;
    private int position=0;

    @Override public void initialize(URL location, ResourceBundle resources)
    {
        global=(Global) MainApplication.globals.get("global");
        MainApplication.controllers.put(this.getClass().getSimpleName(),this);

        font=Font.font("微软雅黑", FontWeight.NORMAL, FontPosture.REGULAR,size);
        global.font=font;
        global.fontWeight=FontWeight.NORMAL;
        global.size=11;
        global.fontPosture=FontPosture.REGULAR;
        global.fontFamily=font.getFamily();
        textArea.setFont(font);

        zoomLabel.setText(String.valueOf(zoomPercentage)+"%");

        platformLabel.setText(System.getProperty("os.name"));
        textArea.textProperty().addListener((observable,oldValue,newValue)->{updateCharCount(newValue.length());});
        textArea.caretPositionProperty().addListener((observable,oldValue,newValue)->{updateCursorPosition();});
        updateCharCount(textArea.getText().length());
        updateCursorPosition();

        //和textArea的滑条有冲突
        textArea.setOnScroll(event->{
            if(event.isControlDown()&&event.getDeltaY()<0)
            {
                zoomOut();
            }
            else if(event.isControlDown()&&event.getDeltaY()>0)
            {
                zoomIn();
            }
        });

        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("TextFiles","*.txt"),
                new FileChooser.ExtensionFilter("All Files","*.*")
        );

        findMenuItem.setDisable(true);
        replaceMenuItem.setDisable(true);
        undoMenuItem.setDisable(true);
        textArea.textProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observableValue, String s, String t1) {
                if(textArea.getLength()>0)
                {
                    findMenuItem.setDisable(false);
                    findNextMenuItem.setDisable(false);
                    findPreMenuItem.setDisable(false);
                    replaceMenuItem.setDisable(false);
                }
                else
                {
                    findMenuItem.setDisable(true);
                    findNextMenuItem.setDisable(true);
                    findPreMenuItem.setDisable(false);
                    replaceMenuItem.setDisable(true);
                }
//                undoMenuItem.setDisable(false);
                position=textArea.getCaretPosition();
            }
        });

        undoMenuItem.disableProperty().bind(textArea.undoableProperty().not());
        undoContextMenuItem.disableProperty().bind(textArea.undoableProperty().not());
        redoMenuItem.disableProperty().bind(textArea.redoableProperty().not());
        redoContextMenuItem.disableProperty().bind(textArea.redoableProperty().not());
        cutMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        cutContextMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        copyMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        copyContextMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        pasteMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        pasteContextMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        deleteMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
        deleteContextMenuItem.disableProperty().bind(textArea.selectedTextProperty().isEmpty());
    }

    public void setStage(Stage stage)
    {
        this.stage=stage;
    }
    public Status getStatus()
    {
        return status;
    }
    public String getCurrentPath()
    {
        return currentPath;
    }

    private void updateCharCount(int count)
    {
        charCountLabel.setText(count+"个字符");
    }
    private void updateCursorPosition()
    {
        String text=textArea.getText().substring(0,textArea.getCaretPosition());
        int line=text.split("\n").length;
        int column=textArea.getCaretPosition()-text.lastIndexOf('\n');
        cursorPositionLabel.setText("行 "+line+"，列 "+column);
    }

    @FXML private void onNewFile()
    {
        Alert alert=showAlert("新建","是否要新建文件？","当前未保存的内容将会丢失", Alert.AlertType.CONFIRMATION);
        Optional<ButtonType> result=alert.showAndWait();
        if(result.get()==ButtonType.OK)
        {
//            saveAdvance();
            onSaveAsFile();
            textArea.clear();
            stage.setTitle("无标题 - 记事本");
            status=Status.createNew;
            currentFile=null;
            currentPath=null;
        }
        else if(result.get()==ButtonType.CANCEL)
        {
            alert.close();
        }
    }
    @FXML private void onOpenFile()
    {
//        saveAdvance();
        currentFile=fileChooser.showOpenDialog(null);
        if(currentFile!=null)
        {
            status=Status.alreadyChange;
            currentPath=currentFile.getAbsolutePath();
            stage.setTitle(currentFile.getName());
            textArea.setText(FileTools.readFile(currentFile));
        }
    }
    @FXML public void onSaveFile()
    {
        status=Status.alreadyServe;
        if(currentFile!=null)
        {
            FileTools.writeFile(currentFile,textArea.getText());
        }
        else
        {
            currentFile=fileChooser.showSaveDialog(null);
            if(currentFile!=null)
            {
                FileTools.writeFile(currentFile,textArea.getText());
                currentPath=currentFile.getAbsolutePath();
            }
        }
    }
    @FXML public boolean onSaveAsFile()
    {
        currentFile=fileChooser.showSaveDialog(null);
        if(currentFile!=null)
        {
            FileTools.writeFile(currentFile,textArea.getText());
            currentPath=currentFile.getAbsolutePath();
            status=Status.alreadyServe;
            stage.setTitle(currentFile.getName());
            return true;
        }
        else
            return false;//没有选择文件不进行保存
    }
    //只是一个菜单，没有具体实现功能
    @FXML private void pageSet()
    {
        PageFormat pf=new PageFormat();
        PrinterJob.getPrinterJob().pageDialog(pf);
    }
    @FXML private void onExit(ActionEvent event)
    {
        if(status!=Status.alreadyServe)
        {
            if(currentPath==null)
            {
                Alert alert=showAlert("提示","要将更改保存到 无标题 吗 ?","是否保存文件", Alert.AlertType.CONFIRMATION);;
                Optional<ButtonType> result=alert.showAndWait();

                if(result.get()==ButtonType.OK)
                {
                    if(onSaveAsFile())
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
                Alert alert=showAlert("提示","要将更改保存到 "+currentPath+" 吗 ?","是否保存文件", Alert.AlertType.CONFIRMATION);;
                Optional<ButtonType> result=alert.showAndWait();

                if(result.get()==ButtonType.OK)
                {
                    onSaveFile();
                    stage.close();
                }
                else if(result.get()==ButtonType.CANCEL)
                {
                    stage.close();
                }
            }
        }
    }

    private void saveAdvance()
    {
        if(currentFile!=null&&textArea.getLength()>0)
        {
            FileTools.writeFile(currentFile,textArea.getText());
        }
        else if(currentFile==null&&textArea.getLength()>0)
        {
            fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("TextFiles","*.txt"));
            fileChooser.setTitle("保存目录");
            currentFile=fileChooser.showSaveDialog(null);
            if(currentFile!=null)
            {
                FileTools.writeFile(currentFile,textArea.getText());
            }
        }
    }

    @FXML private void onUndo(ActionEvent event)
    {
        textArea.undo();
    }
    @FXML private void onRedo(ActionEvent event)
    {
        textArea.redo();
    }
    @FXML private void onCut(ActionEvent event)
    {
        textArea.cut();
    }
    @FXML private void onCopy(ActionEvent event)
    {
        textArea.copy();
    }
    @FXML private void onPaste(ActionEvent event)
    {
        textArea.paste();
    }
    @FXML private void onDelete(ActionEvent event)
    {
        try
        {
            textArea.replaceSelection("");
        }
        catch (Exception e)
        {
            ;
        }
    }
    @FXML private void onSelectAll(ActionEvent event)
    {
        textArea.selectAll();
    }

    @FXML private void onFontChoose()
    {
        Parent fontChooseRoot=null;
        try
        {
            fontChooseRoot=FXMLLoader.load(getClass().getResource("Font.fxml"));
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }

        Scene fontChooseScene=new Scene(fontChooseRoot);
        fontStage=new Stage();
        fontStage.setTitle("字体");
        fontStage.setResizable(false);
        fontStage.setScene(fontChooseScene);
        fontStage.show();
    }
    public void fontChooseClose()
    {
        fontStage.close();
    }
    public void fontChooseConfirm()
    {
        font=global.font;
        size=global.size;
        textArea.setFont(font);
    }

    @FXML private void onFind() throws IOException {
        Parent findRoot= FXMLLoader.load(getClass().getResource("Search.fxml"));
        Scene findScene=new Scene(findRoot);

        Stage findStage=new Stage();
        findStage.setScene(findScene);
        findStage.setTitle("查找与替换");
        findStage.show();
    }
    @FXML private void onFindNext() throws IOException {
        if(global.search==null||global.search.length()==0)
        {
            onFind();
        }
        else
        {
            onSearch();
        }
    }
    @FXML private void onFindPre() throws IOException {
        if(global.search==null||global.search.length()==0)
        {
            onFind();
        }
        else
        {
            onPreviousSearch();
        }
    }
    public void onSearch()
    {
        int i=0,j=0;
        int findStartPos=textArea.getCaretPosition();
        String string1,string2;
        string1=textArea.getText();
        string2=global.search;
        if(string2.length()==0)
        {
            showAlert("提示","查找内容不能为空", Alert.AlertType.WARNING);
            return;
        }
        if(findStartPos>=string1.length())//避免出现bug
        {
            findStartPos=0;
        }
        if(textArea.getSelectedText().isEmpty())
        {
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.indexOf(caseIgnoredString2,findStartPos);
            }
            else
            {
                i=string1.indexOf(string2,findStartPos);
            }
        }
        else
        {
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.indexOf(caseIgnoredString2,findStartPos-caseIgnoredString2.length()+1);
            }
            else
            {
                i=string1.indexOf(string2,findStartPos-string2.length()+1);
            }
        }

        if(i>-1)
        {
            textArea.positionCaret(i);
            j=string2.length();
            textArea.selectRange(i,i+j);
        }
        else {
            showAlert("提示","找不到\""+string2+"\"", Alert.AlertType.INFORMATION);
        }
    }
    public void onPreviousSearch()
    {
        int i=0,j=0;
        int findStartPos=textArea.getCaretPosition();
        String string0,string1,string2;
        string0=textArea.getText();
        string2=global.search;
        if(string2.length()==0)
        {
            showAlert("提示","查找内容不能为空", Alert.AlertType.WARNING);
            return;
        }
        string1=string0.substring(0,findStartPos);
        if(textArea.getSelectedText().isEmpty())
        {
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.lastIndexOf(caseIgnoredString2);
            }
            else
            {
                i=string1.lastIndexOf(string2);
            }
        }
        else
        {
            string1=string0.substring(0,string1.length()-string2.length());
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.lastIndexOf(caseIgnoredString2);
            }
            else
            {
                i=string1.lastIndexOf(string2);
            }
        }

        //查找失败
        if(i>-1)
        {
            textArea.positionCaret(i);
            j=string2.length();
            textArea.selectRange(i,i+j);
        }
        else {
            showAlert("提示","找不到\""+string2+"\"", Alert.AlertType.INFORMATION);
        }
    }
    public void onReplace()
    {
        int i=0;
        int findStartPos=textArea.getCaretPosition();
        String string1,string2,string3;
        string1=textArea.getText();
        string2=global.search;
        string3=global.replace;
        if(string2.length()==0)
        {
            showAlert("提示","查找内容不能为空", Alert.AlertType.WARNING);
            return;
        }
        if(findStartPos>=string1.length())
        {
            findStartPos=0;
        }
        if(textArea.getSelectedText().isEmpty())
        {
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.indexOf(caseIgnoredString2,findStartPos);
            }
            else
            {
                i=string1.indexOf(string2,findStartPos);
            }
        }
        else
        {
            if(global.isCaseIgnored)
            {
                String caseIgnoredString1=string1.toLowerCase();
                String caseIgnoredString2=string2.toLowerCase();
                i=caseIgnoredString1.indexOf(caseIgnoredString2,findStartPos-caseIgnoredString2.length());
            }
            else
            {
                i=string1.indexOf(string2,findStartPos-string2.length());
            }
        }

        if(i>-1)
        {
            textArea.positionCaret(i);
            textArea.selectRange(i,i+string2.length());
            try
            {
                if(string3.length()==0&&!textArea.getSelectedText().isEmpty())
                {
                    textArea.replaceSelection("");
                }
                if(string3.length()>0&&!textArea.getSelectedText().isEmpty())
                {
                    textArea.replaceSelection(string3);
                }
                status=Status.alreadyChange;
            }
            catch (Exception e) {
                ;
            }
        }
        else {
            showAlert("提示","找不到\""+string2+"\"", Alert.AlertType.INFORMATION);
        }
    }
    public void onReplaceAll()
    {
        int i=0,cnt=0;
        textArea.positionCaret(0);
        while(i>-1) {
            int findStartPos = textArea.getCaretPosition();
            String string1, string2, string3;
            string1 = textArea.getText();
            string2 = global.search;
            string3 = global.replace;
            if (string2.length() == 0) {
                showAlert("提示", "查找内容不能为空", Alert.AlertType.WARNING);
                return;
            }
            if (textArea.getSelectedText().isEmpty()) {
                if (global.isCaseIgnored) {
                    String caseIgnoredString1 = string1.toLowerCase();
                    String caseIgnoredString2 = string2.toLowerCase();
                    i = caseIgnoredString1.indexOf(caseIgnoredString2, findStartPos);
                } else {
                    i = string1.indexOf(string2, findStartPos);
                }
            } else {
                if (global.isCaseIgnored) {
                    String caseIgnoredString1 = string1.toLowerCase();
                    String caseIgnoredString2 = string2.toLowerCase();
                    i = caseIgnoredString1.indexOf(caseIgnoredString2, findStartPos - caseIgnoredString2.length() + 1);
                } else {
                    i = string1.indexOf(string2, findStartPos - string2.length() + 1);
                }
            }

            if (i > -1) {
                textArea.positionCaret(i);
                textArea.selectRange(i, i + string2.length());
            } else if(cnt==0)
            {
                showAlert("提示","找不到相关内容", Alert.AlertType.INFORMATION);
            }
            if(string3.length()==0&&!textArea.getSelectedText().isEmpty())
            {
                textArea.replaceSelection("");
                cnt++;
                status=Status.alreadyChange;
            }
            if(string3.length()>0&&!textArea.getSelectedText().isEmpty())
            {
                textArea.replaceSelection(string3);
                cnt++;
                status=Status.alreadyChange;
            }
        }
    }

    @FXML private void onGoto() throws IOException {
        Parent turnToRoot=FXMLLoader.load(getClass().getResource("TurnTo.fxml"));
        Scene turnToScene=new Scene(turnToRoot);
        turnToStage=new Stage();
        turnToStage.setScene(turnToScene);
        turnToStage.setTitle("转到行");
        turnToStage.setResizable(false);
        turnToStage.show();
    }
    public void onTurnTo()
    {
        int turnTo= global.turnTo;
        int line;
        String string1,string2;
        String[] strings;
        string1=textArea.getText();
        strings=string1.split("\n");
        line=strings.length+1;
        textArea.positionCaret(0);

        if(turnTo>=line)
        {
            showAlert("提示","行数超过当前最大行数", Alert.AlertType.INFORMATION);
            return;
        }
        int i=0;
        int findStartPos=textArea.getCaretPosition();
        while(findStartPos<=string1.length())
        {
            findStartPos=textArea.getCaretPosition();
            textArea.selectRange(0,findStartPos);
            string2=textArea.getSelectedText();
            i=string2.split("\n").length;
            if(i==turnTo)
            {
                textArea.positionCaret(findStartPos);
                break;
            }
            findStartPos++;
            textArea.positionCaret(findStartPos);
        }
    }
    public void onTurnToClose()
    {
        turnToStage.close();
    }

    @FXML private void onTime()
    {
        GregorianCalendar calendar=new GregorianCalendar();

        textArea.insertText(textArea.getCaretPosition(),(calendar.get(Calendar.HOUR_OF_DAY)+":"+calendar.get(Calendar.MINUTE)+" "+calendar.get(Calendar.YEAR)+"/"+(calendar.get(Calendar.MONTH)+1)+"/"+calendar.get(Calendar.DAY_OF_MONTH)));
    }
    @FXML private void onState()
    {
        if(stateMenu.isSelected())
            stateBar.setVisible(true);
        else
            stateBar.setVisible(false);
    }
    @FXML private void onWrap()
    {
        if(wrapMenu.isSelected())
            textArea.setWrapText(true);
        else
            textArea.setWrapText(false);
    }

    @FXML private void zoomIn()
    {
        size++;
        zoomPercentage+=10;
        if(size>=72)
        {
            size=72;
        }
        if(zoomPercentage>=500)
        {
            zoomPercentage=500;
        }
        font=Font.font(global.fontFamily,global.fontWeight,global.fontPosture,size);
        textArea.setFont(font);
        zoomLabel.setText(String.valueOf(zoomPercentage)+"%");
    }
    @FXML private void zoomOut()
    {
        size--;
        zoomPercentage-=10;
        if(size<=1)
        {
            size=1;
        }
        if(zoomPercentage<=10)
        {
            zoomPercentage=10;
        }
        font=Font.font(global.fontFamily,global.fontWeight,global.fontPosture,size);
        textArea.setFont(font);
        zoomLabel.setText(String.valueOf(zoomPercentage)+"%");
    }
    @FXML public void zoomInit()
    {
        size=11;
        zoomPercentage=100;
        font=Font.font(global.fontFamily,global.fontWeight,global.fontPosture,size);
        textArea.setFont(font);
        zoomLabel.setText(String.valueOf(zoomPercentage)+"%");
    }

    @FXML private void onHelp()
    {
        Alert alert=showAlert("帮助","Death Note v2025.5.1","全世界最弱小的什么也做不到的\n有一白个理由去死却依然活着的\nXMU_37220232203780_数字媒体寄术_马鑫", Alert.AlertType.INFORMATION);
        alert.showAndWait();
    }

    private void showAlert(String title, String message, Alert.AlertType type)
    {
        Alert alert=new Alert(type);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
    private Alert showAlert(String title,String header,String message, Alert.AlertType type)
    {
        Alert alert=new Alert(type);
        alert.setTitle(title);
        alert.setHeaderText(header);
        alert.setContentText(message);
//        alert.showAndWait();
        return alert;
    }
}