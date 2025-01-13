#include <string>

class User {
public:
    User() {
        // No fields in this class now
    }
};

class Address {
private:
    std::string city;
    std::string name; // Field moved here

public:
    Address(const std::string &city, const std::string &name) : city(city), name(name) {}

    std::string getCity() const {
        return city;
    }

    std::string getName() const {
        return name;
    }
};
