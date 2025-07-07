import java.util.Random;

public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        Random ran=new Random();
        Employee salariedEmployee=new Employee("John", "Smith", "111-11-1111", new SalariedCompensationModel(800.00));
        Employee hourlyEmployee=new Employee("Karen", "Price", "222-22-2222",new HourlyCompensationModel(16.75,40));
        Employee commissionEmployee=new Employee("Sue", "Jones", "333-33-3333",new CommissionCompensationModel(10000, .06));
        Employee basePlusCommissionEmployee=new Employee( "Bob", "Lewis", "444-44-4444",new BasePlusCommissionCompensationMode( 5000, .04, 300));
        System.out.println(salariedEmployee);
        System.out.println(hourlyEmployee);
        System.out.println(commissionEmployee);
        System.out.println(basePlusCommissionEmployee);
        salariedEmployee.setCompensationModel(new SalariedCompensationModel(ran.nextFloat()*1000));
        hourlyEmployee.setCompensationModel(new HourlyCompensationModel(ran.nextFloat()*100,ran.nextFloat()*168));
        commissionEmployee.setCompensationModel(new CommissionCompensationModel(ran.nextFloat()*100,ran.nextFloat()));
        basePlusCommissionEmployee.setCompensationModel(new BasePlusCommissionCompensationMode(ran.nextFloat()*100,ran.nextFloat(),ran.nextFloat()*100));
        System.out.println(salariedEmployee);
        System.out.println(hourlyEmployee);
        System.out.println(commissionEmployee);
        System.out.println(basePlusCommissionEmployee);
    }
}