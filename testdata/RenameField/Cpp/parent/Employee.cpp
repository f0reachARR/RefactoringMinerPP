#include <string>
#include <ctime>

class Employee {
private:
    std::string name;
    int age;
    std::tm joinDate;

public:
    Employee(std::string name, int age, std::tm joinDate)
        : name(name), age(age), joinDate(joinDate) {}

    std::string getName() {
        return name;
    }

    int getAge() {
        return age;
    }

    std::tm getJoinDate() {
        return joinDate;
    }
};
