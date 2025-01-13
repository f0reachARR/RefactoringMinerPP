#include <string>
#include <iostream>

class Temperature {
private:
    int value; // Temperature in Celsius

public:
    Temperature(int value) : value(value) {}

    int getValue() const {
        return value;
    }

    void setValue(int value) {
        this->value = value;
    }

    std::string toString() const {
        return "Temperature: " + std::to_string(value) + "°C";
    }
};
