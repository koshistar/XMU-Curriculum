package com.example.lab11_2;

import java.io.Serializable;

public class Student implements Serializable {
    private String id;
    private String name;
    private String phone;
    private String email;
    private String photoPath;

    public Student(String id,String  name,String phone,String email,String photoPath)
    {
        this.id=id;
        this.name=name;
        this.phone=phone;
        this.email=email;
        this.photoPath=photoPath;
    }
    public String getId(){return id;}
    public void setId(String id){this.id=id;}
    public String getName(){return name;}
    public void setName(String name){this.name=name;}
    public String getPhone(){return phone;}
    public void setPhone(String phone){this.phone=phone;}
    public String getEmail(){return email;}
    public void setEmail(String email){this.email=email;}
    public String getPhotoPath(){return photoPath;}
    public void setPhotoPath(String photoPath){this.photoPath=photoPath;}
}
