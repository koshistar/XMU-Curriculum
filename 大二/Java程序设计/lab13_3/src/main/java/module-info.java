module org.example.lab13_3 {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.example.lab13_3 to javafx.fxml;
    exports org.example.lab13_3;
}