#include <iostream>
#include <string>

namespace com_example {

class Car {
public:
    std::string model;
    int year;

    Car(std::string model, int year) {
        this->model = model;
        this->year = year;
    }

    std::string getModel() {
        return model;
    }

    int getYear() {
        return year;
    }
};

}
