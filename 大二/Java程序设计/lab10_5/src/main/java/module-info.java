module com.example.lab10_5 {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.lab10_5 to javafx.fxml;
    exports com.example.lab10_5;
}