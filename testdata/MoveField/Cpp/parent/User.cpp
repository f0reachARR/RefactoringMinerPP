#include <string>

class User {
private:
    std::string name; // The field to be moved

public:
    User(const std::string &name) : name(name) {}

    std::string getName() const {
        return name;
    }
};

class Address {
private:
    std::string city;

public:
    Address(const std::string &city) : city(city) {}

    std::string getCity() const {
        return city;
    }
};
