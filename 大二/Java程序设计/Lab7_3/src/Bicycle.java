public class Bicycle implements  CarbonFootprint{
    //碳足迹 (kgCO2)=骑行距离 (km)×单位距离碳排放因子 (kgCO2/km)
    private double distance;
    private double eliminationCoefficient;
    public Bicycle(double distance,double eliminationCoefficient)
    {
        this.distance=distance;
        this.eliminationCoefficient=eliminationCoefficient;
    }
    @Override
    public double getCarbonFootprint() {
        return distance*eliminationCoefficient;
    }
    @Override
    public String toString() {
        return "该自行车的骑行距离为"+distance+"，单位距离碳排放因子为"+eliminationCoefficient+"，碳足迹为"+getCarbonFootprint();
    }
}
