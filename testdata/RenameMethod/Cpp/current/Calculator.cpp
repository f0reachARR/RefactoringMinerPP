#include <iostream>
#include <vector>

class Calculator {
public:
    int computeSum(const std::vector<int>& numbers) {
        int sum = 0;
        for (int number : numbers) {
            sum += number;
        }
        return sum;
    }
};
