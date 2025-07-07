import java.util.ArrayList;

public class Salary extends Expense{
    ArrayList<Employee> employees=new ArrayList<Employee>();
    //初始化员工信息
    public void init()
    {
        employees.add(new Employee("101","张三",8000));
        employees.add(new Employee("102","李四",9000));
    }

    public Salary()
    {
        super();
        init();
    }
    @Override
    public int getExpense() {
        int sum=0;
        for(Employee employee:employees)
        {
            sum+=employee.getExpense();
        }
        return sum;
    }

    @Override
    public String toString() {
        String res="";
        for(int i=0;i<employees.size();i++)
        {
            res+=employees.get(i).toString()+"，"+super.toString();
            //最后一个不换行
            if(i!=employees.size()-1)
                res+="\n";
        }
        return res;
    }
}
