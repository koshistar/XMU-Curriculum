module com.example.lab5_3 {
    requires javafx.controls;
    requires javafx.fxml;
    requires jdk.xml.dom;


    opens com.example.lab5_3 to javafx.fxml;
    exports com.example.lab5_3;
}