package com.example.lab11_3;

import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.FileChooser;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class HelloController {
    private static final String DATA_FILE="student.dat";
    private static final String PHOTO_DIRECTORY="student_photos/";

    private List<Long> recordPositions=new ArrayList<>();//存储每条记录的起始位置
    private int currentIndex=-1;
    private List<Integer> searchResults=new ArrayList<>();
    private boolean inSearchMode=false;

    @FXML private TextField idField;
    @FXML private TextField nameField;
    @FXML private  TextField phoneField;
    @FXML private  TextField emailField;
    @FXML private ImageView photoView;

    private String currentPhotoPath;

    @FXML public void initialize()
    {
        loadData();
        if(!recordPositions.isEmpty())
        {
            currentIndex=0;
            displayStudent(currentIndex);
        }
    }

    @FXML private void handleSelectPhoto()
    {
        FileChooser fileChooser=new FileChooser();
        fileChooser.setTitle("选择学生照片");
        fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("JPG Images","*.jpg","*.jpeg"));
        fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("PNG Images","*.png"));

        File selectedFile=fileChooser.showOpenDialog(photoView.getScene().getWindow());
        if(selectedFile!=null)
        {
            currentPhotoPath=selectedFile.getAbsolutePath();
            displayPhoto(currentPhotoPath);
        }
    }
    @FXML private void handleAdd()
    {
        String id=idField.getText().trim();
        String name=nameField.getText().trim();
        String phone=phoneField.getText().trim();
        String email=emailField.getText().trim();

        if(id.isEmpty()||name.isEmpty())
        {
            showAlert("错误","学号、姓名不能为空", Alert.AlertType.ERROR);
            return;
        }

        for(int i=0;i<recordPositions.size();i++)
        {
            Student s=readStudent(i);
            if(s!=null&&s.getId().equals(id))
            {
                showAlert("错误","该学号已存在", Alert.AlertType.ERROR);
                return;
            }
        }
        String photoFileName=null;
        if(currentPhotoPath!=null&&!currentPhotoPath.isEmpty())
        {
            try{
                File source=new File(currentPhotoPath);
                photoFileName=id+getFileExtension(source.getName());
                File destination=new File(PHOTO_DIRECTORY+photoFileName);
                new File(PHOTO_DIRECTORY).mkdirs();
                Files.copy(source.toPath(),destination.toPath(), StandardCopyOption.REPLACE_EXISTING);
            }
            catch (IOException e)
            {
                showAlert("错误","照片保存失败："+e.getMessage(), Alert.AlertType.ERROR);
                return;
            }
        }
        Student student=new Student(id,name,phone,email,photoFileName);
        currentIndex=recordPositions.size();
        saveStudent(student);

        currentIndex=recordPositions.size()-1;
        clearFields();
        showAlert("成功","学生信息添加成功", Alert.AlertType.INFORMATION);
    }
    private String getFileExtension(String fileName)
    {
        int dotIndex=fileName.lastIndexOf('.');
        return (dotIndex==-1)?"":fileName.substring(dotIndex);
    }
    @FXML private void handleDelete()
    {
        if(currentIndex==-1||recordPositions.isEmpty())
        {
            showAlert("提示","当前目录没有学生，无法删除", Alert.AlertType.INFORMATION);
            return;
        }

        Optional<ButtonType> result=new Alert(Alert.AlertType.CONFIRMATION,"确定要删除当前学生信息吗",ButtonType.YES,ButtonType.NO).showAndWait();

        if(result.isPresent()&&result.get()==ButtonType.YES)
        {
            Student student=readStudent(currentIndex);
            if(student!=null&&student.getPhotoPath()!=null)
            {
                new File(PHOTO_DIRECTORY+student.getPhotoPath()).delete();
            }
            deleteStudent(currentIndex);
            loadData();
            if(recordPositions.isEmpty())
            {
                currentIndex=-1;
                clearFields();
            }
            else
            {
                if(currentIndex>=recordPositions.size())
                {
                    currentIndex=recordPositions.size()-1;
                }
                displayStudent(currentIndex);
            }
            inSearchMode=false;
        }
    }
    @FXML private void handleUpdate()
    {
        if(currentIndex==-1||recordPositions.isEmpty())
        {
            showAlert("提示","没有可修改的学生信息",Alert.AlertType.INFORMATION);
            return;
        }

        String id=idField.getText().trim();
        String name=nameField.getText().trim();
        String phone=phoneField.getText().trim();
        String email=emailField.getText().trim();

        if(id.isEmpty()||name.isEmpty())
        {
            showAlert("错误","学号、姓名不能为空",Alert.AlertType.ERROR);
            return;
        }

        Student student=readStudent(currentIndex);

        for(int i=0;i<recordPositions.size();i++)
        {
            Student s=readStudent(i);
            if(i!=currentIndex&&s!=null&&s.getId().equals(id))
            {
                showAlert("错误","已有学生使用该学号", Alert.AlertType.ERROR);
                return;
            }
        }

        String photoFilename=student.getPhotoPath();
        if(currentPhotoPath!=null&&!currentPhotoPath.isEmpty())
        {
            try{
                if(photoFilename!=null)
                {
                    new File(PHOTO_DIRECTORY+photoFilename).delete();
                }

                File source=new File(currentPhotoPath);
                photoFilename=id+getFileExtension(source.getName());
                File destination=new File(PHOTO_DIRECTORY+photoFilename);
                Files.copy(source.toPath(),destination.toPath(),StandardCopyOption.REPLACE_EXISTING);
            }
            catch (IOException e)
            {
                showAlert("错误","照片更新失败："+e.getMessage(), Alert.AlertType.ERROR);
                return;
            }
        }
        student.setId(id);
        student.setName(name);
        student.setPhone(phone);
        student.setEmail(email);
        student.setPhotoPath(photoFilename);

        saveStudent(student);
        showAlert("成功","学生信息修改成功", Alert.AlertType.INFORMATION);
    }
    @FXML private void handleSearch()
    {
        TextInputDialog dialog=new TextInputDialog();
        dialog.setTitle("查询学生");
        dialog.setContentText("姓名：");

        Optional<String> result=dialog.showAndWait();
        result.ifPresent(name->{
            if(name.trim().isEmpty()){
                return ;
            }
            searchResults.clear();
            for(int i=0;i<recordPositions.size();i++)
            {
                Student s=readStudent(i);
                if(s!=null&&s.getName().contains(name.trim()))
                {
                    searchResults.add(i);
                }
            }
            if(searchResults.isEmpty())
            {
                showAlert("查询失败","没有找到匹配的学生", Alert.AlertType.INFORMATION);
            }
            else
            {
                inSearchMode=true;
                currentIndex=searchResults.get(0);
                displayStudent(currentIndex);
            }
        });
    }
    @FXML private void handlePrevious()
    {
        if(recordPositions.isEmpty())
        {
            return;
        }
        if(inSearchMode)
        {
            int position=searchResults.indexOf(currentIndex);
            if(position>0)
            {
                currentIndex=searchResults.get(position-1);
                displayStudent(currentIndex);
            }
            else {
                currentIndex = searchResults.size() - 1;
                displayStudent(currentIndex);
            }
        }
        else
        {
            if(currentIndex>0)
            {
                currentIndex--;
                displayStudent(currentIndex);
            }
            else
            {
                currentIndex=recordPositions.size()-1;
                displayStudent(currentIndex);
            }
        }
    }
    @FXML private void handleNext()
    {
        if(recordPositions.isEmpty())
        {
            return;
        }
        if(inSearchMode)
        {
            int pos=searchResults.indexOf(currentIndex);
            if(pos<searchResults.size()-1)
            {
                currentIndex=searchResults.get(pos+1);
                displayStudent(currentIndex);
            }
            else
            {
                currentIndex=0;
                displayStudent(currentIndex);
            }
        }
        else
        {
            if(currentIndex<recordPositions.size()-1)
            {
                currentIndex++;
                displayStudent(currentIndex);
            }
            else
            {
                currentIndex=0;
                displayStudent(currentIndex);
            }
        }
    }
    @FXML private void handleShowAll()
    {
        inSearchMode=false;
        if(!recordPositions.isEmpty())
        {
            currentIndex=0;
            displayStudent(currentIndex);
        }
    }

    private void displayStudent(int index)
    {
        if(index<0||index>=recordPositions.size())
        {
            clearFields();
            return;
        }

        Student student=readStudent(index);
        if(student!=null) {
            idField.setText(student.getId());
            nameField.setText(student.getName());
            phoneField.setText(student.getPhone());
            emailField.setText(student.getEmail());

            if (student.getPhotoPath() != null && !student.getPhotoPath().isEmpty()) {
                File photoFile = new File(PHOTO_DIRECTORY + student.getPhotoPath());
                if (photoFile.exists()) {
                    displayPhoto(photoFile.getAbsolutePath());
                    currentPhotoPath = photoFile.getAbsolutePath();
                } else {
                    photoView.setImage(null);
                    currentPhotoPath = null;
                }
            } else {
                photoView.setImage(null);
                currentPhotoPath = null;
            }
        }
    }
    private void displayPhoto(String path)
    {
        try{
            Image image=new Image(new FileInputStream(path));
            photoView.setImage(image);
        }
        catch (FileNotFoundException e)
        {
            photoView.setImage(null);
        }
    }
    private void clearFields()
    {
        idField.clear();
        nameField.clear();
        phoneField.clear();
        emailField.clear();
        photoView.setImage(null);
        currentPhotoPath=null;
    }

    private void loadData()
    {
        recordPositions.clear();
        File file=new File(DATA_FILE);
        if(!file.exists())
        {
            return;
        }
        try(RandomAccessFile raf=new RandomAccessFile(file,"r"))
        {
            long pos=0;
            while (pos<raf.length())
            {
                recordPositions.add(pos);
                pos+=Student.RECORD_LENGTH;
                if(pos<raf.length())
                {
                    raf.seek(pos);
                }
            }
        }
        catch (IOException e)
        {
            showAlert("错误","加载数据失败："+e.getMessage(), Alert.AlertType.ERROR);
        }
    }
    private void saveStudent(Student student)
    {
        try(RandomAccessFile raf=new RandomAccessFile(DATA_FILE,"rw"))
        {
            if(currentIndex==-1||currentIndex>=recordPositions.size())
            {
                long position=raf.length();
                raf.seek(position);
                student.writeToFile(raf);
                recordPositions.add(position);
                currentIndex=recordPositions.size()-1;
            }
            else
            {
                raf.seek(recordPositions.get(currentIndex));
                student.writeToFile(raf);
            }
        }
        catch (IOException e)
        {
            showAlert("错误","存储数据失败："+e.getMessage(), Alert.AlertType.ERROR);
        }
    }
    private Student readStudent(int index)
    {
        if(index<0||index>=recordPositions.size())
            return null;
        try(RandomAccessFile raf=new RandomAccessFile(DATA_FILE,"r"))
        {
            raf.seek(recordPositions.get(index));
            return Student.readFromFile(raf);
        }
        catch (IOException e)
        {
            showAlert("错误","读取数据失败："+e.getMessage(), Alert.AlertType.ERROR);
            return null;
        }
    }
    private void deleteStudent(int index)
    {
        if(index<0||index>=recordPositions.size())
                return;
        recordPositions.remove(index);
        try{
            File tempFile=new File(DATA_FILE+".tmp");
            try(RandomAccessFile oldRaf=new RandomAccessFile(DATA_FILE,"r");RandomAccessFile newRaf=new RandomAccessFile(tempFile,"rw"))
            {
                for(long pos:recordPositions)
                {
                    oldRaf.seek(pos);
                    Student student=Student.readFromFile(oldRaf);
                    newRaf.seek(newRaf.length());
                    student.writeToFile(newRaf);
                }
            }
            Files.move(tempFile.toPath(),new File(DATA_FILE).toPath(),StandardCopyOption.REPLACE_EXISTING);
        }
        catch (IOException e)
        {
            showAlert("错误","删除数据失败："+e.getMessage(), Alert.AlertType.ERROR);
        }
    }

    private void showAlert(String title, String message, Alert.AlertType type)
    {
        Alert alert=new Alert(type);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
}