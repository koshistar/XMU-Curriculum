public class IllegalNameException extends Exception{
    public IllegalNameException(){
    }
    public IllegalNameException(String message){
        super(message);
    }
    public IllegalNameException(Throwable cause){
        super(cause);
    }
    public IllegalNameException(String message,Throwable cause){
        super(message,cause);
    }
}
