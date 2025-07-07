package com.example.lab11_2;

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

    private List<Student> students=new ArrayList<>();
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
        if(!students.isEmpty())
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

        for(Student s:students)
        {
            if(s.getId().equals(id))
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
                photoFileName=id+".jpg";
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
        students.add(student);
        saveData();

        currentIndex=students.size()-1;
        clearFields();
        showAlert("成功","学生信息添加成功", Alert.AlertType.INFORMATION);
    }
    @FXML private void handleDelete()
    {
        if(currentIndex==-1||students.isEmpty())
        {
            showAlert("提示","当前目录没有学生，无法删除", Alert.AlertType.INFORMATION);
            return;
        }

        Optional<ButtonType> result=new Alert(Alert.AlertType.CONFIRMATION,"确定要删除当前学生信息吗",ButtonType.YES,ButtonType.NO).showAndWait();

        if(result.isPresent()&&result.get()==ButtonType.YES)
        {
            Student student=students.get(currentIndex);
            if(student.getPhotoPath()!=null)
            {
                new File(PHOTO_DIRECTORY+student.getPhotoPath()).delete();
            }
            students.remove(currentIndex);
            saveData();
            if(students.isEmpty())
            {
                currentIndex=-1;
                clearFields();
            }
            else
            {
                if(currentIndex>=students.size())
                {
                    currentIndex=students.size()-1;
                }
                displayStudent(currentIndex);
            }
            inSearchMode=false;
        }
    }
    @FXML private void handleUpdate()
    {
        if(currentIndex==-1||students.isEmpty())
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

        Student student=students.get(currentIndex);

        for(int i=0;i<students.size();i++)
        {
            if(i!=currentIndex&&students.get(i).getId().equals(id))
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
                photoFilename=id+".jpg";
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

        saveData();
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
            for(int i=0;i<students.size();i++)
            {
                if(students.get(i).getName().contains(name.trim()))
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
        if(students.isEmpty())
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
                currentIndex=students.size()-1;
                displayStudent(currentIndex);
            }
        }
    }
    @FXML private void handleNext()
    {
        if(students.isEmpty())
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
            if(currentIndex<students.size()-1)
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
        if(!students.isEmpty())
        {
            currentIndex=0;
            displayStudent(currentIndex);
        }
    }

    private void displayStudent(int index)
    {
        if(index<0||index>=students.size())
        {
            return;
        }

        Student student=students.get(index);
        idField.setText(student.getId());
        nameField.setText(student.getName());
        phoneField.setText(student.getPhone());
        emailField.setText(student.getEmail());

        if(student.getPhotoPath()!=null&&!student.getPhotoPath().isEmpty())
        {
            File photoFile=new File(PHOTO_DIRECTORY+student.getPhotoPath());
            if(photoFile.exists())
            {
                displayPhoto(photoFile.getAbsolutePath());
                currentPhotoPath=photoFile.getAbsolutePath();
            }
            else
            {
                photoView.setImage(null);
                currentPhotoPath=null;
            }
        }
        else
        {
            photoView.setImage(null);
            currentPhotoPath=null;
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

    @SuppressWarnings("unchecked")
    private void loadData()
    {
        File file=new File(DATA_FILE);
        if(!file.exists())
        {
            return;
        }

        try(ObjectInputStream ois=new ObjectInputStream(new FileInputStream(file)))
        {
            students=(List<Student>)ois.readObject();
        }
        catch (IOException|ClassNotFoundException e)
        {
            showAlert("错误","加载数据失败"+e.getMessage(),Alert.AlertType.ERROR);
        }
    }
    private void saveData()
    {
        try(ObjectOutputStream oos=new ObjectOutputStream((new FileOutputStream(DATA_FILE))))
        {
            oos.writeObject(students);
        }
        catch (IOException e)
        {
            showAlert("错误","保存数据失败"+e.getMessage(),Alert.AlertType.ERROR);
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