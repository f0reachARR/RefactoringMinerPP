// File: ClassExample.cpp
#include <iostream>

class ParentClass {
public:
    void commonMethod() {
        std::cout << "Common method in Parent" << std::endl;
    }
};

class ChildClass : public ParentClass {
public:
    void childMethod() {
        std::cout << "Child method" << std::endl;
    }
};
