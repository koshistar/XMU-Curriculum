public class Employee{
    private String firstName;
    private String lastName;
    private String socialSecurityNumber;
    private CompensationModel c;
    public Employee(String firstName,String lastName,String socialSecurityNumber,CompensationModel c)
    {
        this.firstName=firstName;
        this.lastName=lastName;
        this.socialSecurityNumber=socialSecurityNumber;
        this.c=c;
    }

    public String getFirstName()
    {
        return firstName;
    }
    public String getLastName()
    {
        return lastName;
    }
    public String getSocialSecurityNumber()
    {
        return socialSecurityNumber;
    }

    double getEarnings()
    {
        return c.earnings();
    }

    void setCompensationModel(CompensationModel c)
    {
        this.c=c;
    }
    @Override
    public String toString() {
        return String.format("%s %s%nsocial security number: %s%nearned:$%,.2f%n%n",
                getFirstName(), getLastName(), getSocialSecurityNumber(),getEarnings());
    }
}
