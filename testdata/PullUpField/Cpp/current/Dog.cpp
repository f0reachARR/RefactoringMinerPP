#include <iostream>
#include <string>

class Animal {
protected:
    std::string name;

public:
    Animal(const std::string& name) : name(name) {}
};

class Dog : public Animal {
public:
    Dog(const std::string& name) : Animal(name) {}

    void bark() {
        std::cout << name << " is barking!" << std::endl;
    }
};

class Poodle : public Dog {
public:
    Poodle(const std::string& name) : Dog(name) {}
};
