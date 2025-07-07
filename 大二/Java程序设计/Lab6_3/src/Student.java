public class Student {
    String name;
    String address;
    public void setName(String name) throws IllegalNameException {
        try {
            if (name.length() < 1 || name.length() > 5) {
                throw new IllegalNameException("Illegal Name");
            }
            else
            {
                this.name=name;
            }
        }
        catch (IllegalNameException e)
        {
            throw e;
        }
    }

    public void setAddress(String address) throws IllegalAddressException
    {
        try
        {
            for(int i=0;i<address.length();i++)
            {
                if(address.charAt(i)=='省'||address.charAt(i)=='市')
                {
                    this.address=address;
                    return;
                }
            }
            throw new IllegalAddressException("Illegal  address.");
        }
        catch (IllegalAddressException e)
        {
            throw e;
        }
    }
}
