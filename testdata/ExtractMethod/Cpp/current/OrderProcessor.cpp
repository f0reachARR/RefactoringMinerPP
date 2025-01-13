#include <iostream>
#include <vector>
#include <string>

class OrderProcessor {
public:
    void processOrder(const std::vector<std::string>& items) {
        double total = calculateTotal(items);
        std::cout << "Total: " << total << std::endl;
    }

private:
    double calculateTotal(const std::vector<std::string>& items) {
        double total = 0;
        for (const std::string& item : items) {
            if (item == "itemA") {
                total += 10;
            } else if (item == "itemB") {
                total += 20;
            } else if (item == "itemC") {
                total += 30;
            }
        }
        return total;
    }
};
