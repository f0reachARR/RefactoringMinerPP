#include <string>
#include <iostream>

class Temperature {
private:
    std::string value; // Temperature in Celsius as a String

public:
    Temperature(const std::string& value) : value(value) {}

    std::string getValue() const {
        return value;
    }

    void setValue(const std::string& value) {
        this->value = value;
    }

    std::string toString() const {
        return "Temperature: " + value + "°C";
    }
};
