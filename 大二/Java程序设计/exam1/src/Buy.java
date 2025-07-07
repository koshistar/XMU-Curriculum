public class Buy extends Expense{
    private String name;
    private int amount;
    private int per;
    public Buy(String sequence,String name,int amount,int per)
    {
        super(sequence);
        this.name=name;
        this.amount=amount;
        this.per=per;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setAmount(int amount) {
        this.amount = amount;
    }

    public void setPer(int per) {
        this.per = per;
    }

    public String getName() {
        return name;
    }

    public int getAmount() {
        return amount;
    }

    public int getPer() {
        return per;
    }

    @Override
    public int getExpense()
    {
        return per*amount;
    }

    @Override
    public String toString() {
        return "已支付"+name+" 数量："+amount+" 单价："+per+"，"+super.toString();
    }
}
