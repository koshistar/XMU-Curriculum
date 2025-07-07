module org.example.lab13_4 {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.example.lab13_4 to javafx.fxml;
    exports org.example.lab13_4;
}