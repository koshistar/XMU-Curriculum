import java.time.LocalDate;

public class Expense {
    private String sequence;
    private int year;
    private int month;

    //Salary无发票编号，使用无参构造
    public Expense()
    {
        sequence=null;
        LocalDate localDate=LocalDate.now();
        year=localDate.getYear();
        month=localDate.getMonthValue();
    }
    //Rent和Buy有发票编号，使用传参构造
    public Expense(String sequence)
    {
        this.sequence=sequence;
        //获取当前时间
        LocalDate localDate=LocalDate.now();
        year=localDate.getYear();
        month=localDate.getMonthValue();
    }

    //获取花费，需要子类进行重写
    public int getExpense()
    {
        return 0;
    }
    @Override
    public String toString() {
        if(sequence!=null)
            return "支付时间"+year+"年"+month+"月，关联发票号"+sequence;
        else
            return "支付时间"+year+"年"+month+"月";
    }
}
