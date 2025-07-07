public class Car implements CarbonFootprint{
    //开车的二氧化碳排放量（Kg）=油耗公升数×0.785；
    private double oil;
    private double constant=0.875;
    public Car(double oil)
    {
        this.oil=oil;
    }
    @Override
    public double getCarbonFootprint() {
        return oil*constant;
    }
    @Override
    public String toString() {
        return "该汽车的耗油公升数为"+oil+"，碳足迹为"+getCarbonFootprint();
    }
}
