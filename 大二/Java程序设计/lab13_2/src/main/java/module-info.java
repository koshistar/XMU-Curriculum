module org.example.lab13_2 {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.example.lab13_2 to javafx.fxml;
    exports org.example.lab13_2;
}