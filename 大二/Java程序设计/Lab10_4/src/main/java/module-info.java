module com.example.lab10_4 {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.lab10_4 to javafx.fxml;
    exports com.example.lab10_4;
}