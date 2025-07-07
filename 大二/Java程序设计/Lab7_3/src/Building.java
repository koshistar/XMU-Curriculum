public class Building implements CarbonFootprint{
    //家居用电的二氧化碳排放量（Kg）= 耗电度数×0.785×可再生能源电力修正系数；
    private double electricity;
    private final double constant=0.785;
    private double fixCoefficient;
    public Building(double electricity,double fixCoefficient)
    {
        this.electricity=electricity;
        this.fixCoefficient=fixCoefficient;
    }
    @Override
    public double getCarbonFootprint() {
        return electricity*constant*fixCoefficient;
    }

    @Override
    public String toString() {
        return "该家居用电的耗电度数为"+electricity+"，可再生能源电力修正系数为"+fixCoefficient+"，碳足迹为"+getCarbonFootprint();
    }
}
