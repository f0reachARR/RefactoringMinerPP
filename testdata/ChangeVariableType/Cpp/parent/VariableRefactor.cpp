#include <iostream>

class VariableRefactor {
public:
    void calculate() {
        int result = 10;
        result = result * 2;
        std::cout << result << std::endl;
    }
};
