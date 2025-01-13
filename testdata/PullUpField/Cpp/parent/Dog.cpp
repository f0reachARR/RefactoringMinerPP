#include <iostream>
#include <string>

class Dog {
protected:
    std::string name;

public:
    Dog(const std::string& name) : name(name) {}

    void bark() {
        std::cout << name << " is barking!" << std::endl;
    }
};

class Poodle : public Dog {
public:
    Poodle(const std::string& name) : Dog(name) {}
};
