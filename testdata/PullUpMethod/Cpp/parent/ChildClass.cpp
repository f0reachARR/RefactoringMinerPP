// File: ClassExample.cpp
#include <iostream>

class ParentClass {
    // No commonMethod in ParentClass
};

class ChildClass : public ParentClass {
public:
    void childMethod() {
        std::cout << "Child method" << std::endl;
    }

    void commonMethod() {
        std::cout << "Common method in Child" << std::endl;
    }
};
