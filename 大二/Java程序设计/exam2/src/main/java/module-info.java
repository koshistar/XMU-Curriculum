module org.example.exam2 {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.example.exam2 to javafx.fxml;
    exports org.example.exam2;
}