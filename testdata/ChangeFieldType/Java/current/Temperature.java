public class Temperature {
    private String value; // Temperature in Celsius as a String

    public Temperature(String value) {
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public String toString() {
        return "Temperature: " + value + "°C";
    }
}
