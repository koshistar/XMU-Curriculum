module org.example.notebook {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.desktop;
    requires jdk.compiler;


    opens org.example.notebook to javafx.fxml;
    exports org.example.notebook;
}