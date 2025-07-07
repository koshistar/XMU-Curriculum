module com.example.lab11_3 {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.lab11_3 to javafx.fxml;
    exports com.example.lab11_3;
}