public class IllegalAddressException extends Exception{
    public IllegalAddressException(){
    }
    public IllegalAddressException(String message){
        super(message);
    }
    public IllegalAddressException(Throwable cause){
        super(cause);
    }
    public IllegalAddressException(String message,Throwable cause){
        super(message,cause);
    }
}
