public class Rent extends Expense{
    private String description;
    private int amount;
    public Rent(String sequence,String description,int amount)
    {
        super(sequence);
        this.amount=amount;
        this.description=description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public void setAmount(int amount) {
        this.amount = amount;
    }

    public String getDescription() {
        return description;
    }

    public int getAmount() {
        return amount;
    }

    @Override
    public int getExpense() {
        return getAmount();
    }

    @Override
    public String toString() {
        return "已支付"+description+"，金额"+amount+"，"+super.toString();
    }
}
