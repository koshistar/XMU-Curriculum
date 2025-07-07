public class Employee {
    private String ID;
    private String name;
    private int amount;
    public Employee(String ID,String name,int amount)
    {
        this.ID=ID;
        this.name=name;
        this.amount=amount;
    }

    public void setID(String ID) {
        this.ID = ID;
    }

    public void setAmount(int amount) {
        this.amount = amount;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public int getAmount() {
        return amount;
    }

    public String getID() {
        return ID;
    }

    public int getExpense() {
        return getAmount();
    }

    @Override
    public String toString() {
        return "已支付"+ID+" "+name+" "+amount;
    }
}
