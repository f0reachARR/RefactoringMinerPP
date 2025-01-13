#include <string>
#include <ctime>

class Employee {
private:
    std::string employeeName;  // Field renamed from 'name' to 'employeeName'
    int age;
    std::tm joinDate;

public:
    Employee(std::string employeeName, int age, std::tm joinDate)
        : employeeName(employeeName), age(age), joinDate(joinDate) {}

    std::string getEmployeeName() {  // Getter method renamed to reflect the new field name
        return employeeName;
    }

    int getAge() {
        return age;
    }

    std::tm getJoinDate() {
        return joinDate;
    }
};
