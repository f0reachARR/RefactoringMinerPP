#include <iostream>
#include <vector>
#include <string>

class PriceCalculator {

public:
    double calculateTotal(const std::vector<std::string>& items) {
        double total = 0;
        for (const auto& item : items) {
            total += getItemPrice(item);
        }
        return total;
    }

    double getItemPrice(const std::string& item) {
        // Imagine this fetches price from a database
        return 20.0;
    }
};

class OrderProcessor {

public:
    void processOrder(const std::vector<std::string>& items) {
        double total = priceCalculator.calculateTotal(items);
        applyDiscount(total);
        std::cout << "Total: " << total << std::endl;
    }

private:
    PriceCalculator priceCalculator;

    void applyDiscount(double total) {
        // Apply a discount if total is above a certain amount
        if (total > 100) {
            total *= 0.9;
        }
    }
};
