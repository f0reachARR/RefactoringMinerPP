public class Temperature {
    private int value; // Temperature in Celsius

    public Temperature(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        this.value = value;
    }

    public String toString() {
        return "Temperature: " + value + "°C";
    }
}
