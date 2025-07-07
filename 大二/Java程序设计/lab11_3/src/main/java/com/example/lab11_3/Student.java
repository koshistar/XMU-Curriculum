package com.example.lab11_3;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.io.Serializable;

public class Student implements Serializable {
    private String id;
    private String name;
    private String phone;
    private String email;
    private String photoPath;

    public static final int ID_LENGTH=15;
    public static final int NAME_LENGTH=30;
    public static final int PHONE_LENGTH=15;
    public static  final int EMAIL_LENGTH=80;
    public static final int PHOTO_PATH_LENGTH=100;
    public static final int RECORD_LENGTH=(ID_LENGTH+NAME_LENGTH+PHONE_LENGTH+EMAIL_LENGTH+PHOTO_PATH_LENGTH)*2;

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

    public void writeToFile(RandomAccessFile raf) throws IOException
    {
        writeFixedString(raf,id,ID_LENGTH);
        writeFixedString(raf,name,NAME_LENGTH);
        writeFixedString(raf,phone,PHONE_LENGTH);
        writeFixedString(raf,email,EMAIL_LENGTH);
        writeFixedString(raf,photoPath,PHOTO_PATH_LENGTH);
    }
    public static Student readFromFile(RandomAccessFile raf)throws IOException
    {
        String id=readFixedString(raf,ID_LENGTH);
        String name=readFixedString(raf,NAME_LENGTH);
        String phone=readFixedString(raf,PHONE_LENGTH);
        String  email=readFixedString(raf,EMAIL_LENGTH);
        String photoPath=readFixedString(raf,PHOTO_PATH_LENGTH);

        return new Student(id,name,phone,email,photoPath);
    }
    public void writeFixedString(RandomAccessFile raf,String str,int length) throws IOException
    {
        StringBuilder builder=new StringBuilder(str!=null?str:"");
        builder.setLength(length);
        raf.writeChars(builder.toString());
    }
    public static String readFixedString(RandomAccessFile raf,int length) throws IOException
    {
        StringBuilder builder=new StringBuilder();
        for(int i=0;i<length;i++)
        {
            char c=raf.readChar();
            if(c!=0)
            {
                builder.append(c);
            }
        }
        return builder.toString().trim();
    }
}