package com.example.lab5_3;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Slider;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleGroup;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Shape;

public class PainterController {
    // enum representing pen sizes
    private enum PenSize
    {
        SMALL(2),
        MEDIUM(4),
        LARGE(6);

        private final int radius;

        PenSize(int radius)
        {
            this.radius = radius;
        }

        public int getRadius()
        {
            return radius;
        }
    };
    private enum ShapeType{
        CIRCLE,RECTANGLE,LINE
    }

    // instance variables that refer to GUI components
//    @FXML private RadioButton blackRadioButton;
//    @FXML private RadioButton redRadioButton;
//    @FXML private RadioButton greenRadioButton;
//    @FXML private RadioButton blueRadioButton;
    @FXML private Slider redSlider;
    @FXML private Slider greenSlider;
    @FXML private Slider blueSlider;
    @FXML private Slider alphaSlider;
    @FXML private TextField redTextField;
    @FXML private TextField greenTextField;
    @FXML private TextField blueTextField;
    @FXML private TextField alphaTextField;
    @FXML private Rectangle colorRectangle;
    @FXML private RadioButton smallRadioButton;
    @FXML private RadioButton mediumRadioButton;
    @FXML private RadioButton largeRadioButton;
    @FXML private RadioButton circleRadioButton;
    @FXML private RadioButton rectangleRadioButton;
    @FXML private RadioButton lineRadioButton;
    @FXML private AnchorPane drawingAreaAnchorPane;
//    @FXML private ToggleGroup colorToggleGroup;
    @FXML private ToggleGroup sizeToggleGroup;
    @FXML private ToggleGroup shapeToggleGroup;

    // instance variables for managing Painter state
    private PenSize radius = PenSize.MEDIUM; // radius of circle
//    private Paint brushColor = Color.BLACK; // drawing color
    private ShapeType currentShape=ShapeType.CIRCLE;
    private double startX,startY;
    private Shape currentDrawingShape;
    private int red=0;
    private int green=0;
    private int blue=0;
    private double alpha=1.0;
    private Paint currentColor;
    // set user data for the RadioButtons
    public void initialize()
    {
        // user data on a control can be any Object
//        blackRadioButton.setUserData(Color.BLACK);
//        redRadioButton.setUserData(Color.RED);
//        greenRadioButton.setUserData(Color.GREEN);
//        blueRadioButton.setUserData(Color.BLUE);
        redTextField.textProperty().bind(redSlider.valueProperty().asString("%.0f"));
        greenTextField.textProperty().bind(greenSlider.valueProperty().asString("%.0f"));
        blueTextField.textProperty().bind(blueSlider.valueProperty().asString("%.0f"));
        alphaTextField.textProperty().bind(alphaSlider.valueProperty().asString("%.2f"));
        redSlider.valueProperty().addListener(
                new ChangeListener<Number>() {
                    @Override
                    public void changed(ObservableValue<? extends Number> observableValue, Number number, Number t1) {
                        red=t1.intValue();
                        colorRectangle.setFill(Color.rgb(red,green,blue,alpha));
                        currentColor=Color.rgb(red,green,blue,alpha);
                    }
                }
        );
        greenSlider.valueProperty().addListener(
                new ChangeListener<Number>() {
                    @Override
                    public void changed(ObservableValue<? extends Number> observableValue, Number number, Number t1) {
                        green=t1.intValue();
                        colorRectangle.setFill(Color.rgb(red,green,blue,alpha));
                        currentColor=Color.rgb(red,green,blue,alpha);
                    }
                }
        );
        blueSlider.valueProperty().addListener(
                new ChangeListener<Number>() {
                    @Override
                    public void changed(ObservableValue<? extends Number> observableValue, Number number, Number t1) {
                        blue=t1.intValue();
                        colorRectangle.setFill(Color.rgb(red,green,blue,alpha));
                        currentColor=Color.rgb(red,green,blue,alpha);
                    }
                }
        );
        alphaSlider.valueProperty().addListener(
                new ChangeListener<Number>() {
                    @Override
                    public void changed(ObservableValue<? extends Number> observableValue, Number number, Number t1) {
                        alpha=t1.doubleValue();
                        colorRectangle.setFill(Color.rgb(red,green,blue,alpha));
                        currentColor=Color.rgb(red,green,blue,alpha);
                    }
                }
        );
        currentColor=Color.rgb(red,green,blue,alpha);
        smallRadioButton.setUserData(PenSize.SMALL);
        mediumRadioButton.setUserData(PenSize.MEDIUM);
        largeRadioButton.setUserData(PenSize.LARGE);
        circleRadioButton.setUserData(ShapeType.CIRCLE);
        rectangleRadioButton.setUserData(ShapeType.RECTANGLE);
        lineRadioButton.setUserData(ShapeType.LINE);
    }
    @FXML
    private void drawingAreaMousePressed(MouseEvent e)
    {
        startX=e.getX();
        startY=e.getY();
        switch (currentShape)
        {
            case CIRCLE :
                currentDrawingShape=new Circle(startX,startY,radius.getRadius(),currentColor);
                break;
            case RECTANGLE:
                currentDrawingShape=new Rectangle(startX,startY,0,0);
                currentDrawingShape.setFill(currentColor);
                break;
            case LINE:
                currentDrawingShape=new Line(startX,startY,startX,startY);
                currentDrawingShape.setStroke(currentColor);
                currentDrawingShape.setStrokeWidth(radius.getRadius());
                break;
        }
        drawingAreaAnchorPane.getChildren().add(currentDrawingShape);
    }
    // handles drawingArea's onMouseDragged MouseEvent
    @FXML
    private void drawingAreaMouseDragged(MouseEvent e)
    {
        double currentX=e.getX();
        double currentY=e.getY();
        switch(currentShape)
        {
            case CIRCLE :
                Circle circle=(Circle) currentDrawingShape;
                circle.setRadius(calculateDistance(startX,startY,currentX,currentY));
                break;
            case RECTANGLE:
                Rectangle rect=(Rectangle) currentDrawingShape;
                rect.setX(Math.min(startX,currentX));
                rect.setY(Math.min(startY,currentY));
                rect.setWidth(Math.abs(currentX-startX));
                rect.setHeight(Math.abs(currentY-startY));
                break;
            case LINE:
                Line line=(Line) currentDrawingShape;
                line.setEndX(currentX);
                line.setEndY(currentY);
                break;
        }
//        Circle newCircle = new Circle(e.getX(), e.getY(),
//                radius.getRadius(), brushColor);
//        drawingAreaAnchorPane.getChildren().add(newCircle);
    }

    private double calculateDistance(double x1,double y1,double x2,double y2)
    {
        return Math.sqrt(Math.pow(x2-x1,2)+Math.pow(y2-y1,2));
    }
    // handles color RadioButton's ActionEvents
//    @FXML
//    private void colorRadioButtonSelected(ActionEvent e)
//    {
//        // user data for each color RadioButton is the corresponding Color
//        brushColor =
//                (Color) colorToggleGroup.getSelectedToggle().getUserData();
//    }

    // handles size RadioButton's ActionEvents
    @FXML
    private void sizeRadioButtonSelected(ActionEvent e)
    {
        // user data for each size RadioButton is the corresponding PenSize
        radius =
                (PenSize) sizeToggleGroup.getSelectedToggle().getUserData();
    }

    @FXML
    private void shapeRadioButtonSelected(ActionEvent e)
    {
        currentShape=(ShapeType) shapeToggleGroup.getSelectedToggle().getUserData();
    }
    // handles Undo Button's ActionEvents
    @FXML
    private void undoButtonPressed(ActionEvent event)
    {
        int count = drawingAreaAnchorPane.getChildren().size();

        // if there are any shapes remove the last one added
        if (count > 0)
            drawingAreaAnchorPane.getChildren().remove(count - 1);
    }

    // handles Clear Button's ActionEvents
    @FXML
    private void clearButtonPressed(ActionEvent event)
    {
        drawingAreaAnchorPane.getChildren().clear(); // clear the canvas
    }
}