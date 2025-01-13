#include <vector>

class Item {
public:
    double getPrice() const {
        return price;
    }

private:
    double price;
};

class OrderProcessor {
public:
    void processOrder() {
        double total = calculateTotal();
        // Further order processing
    }

private:
    double calculateTotal() {
        double total = 0;
        for (const Item& item : items) {
            total += item.getPrice();
        }
        return total;
    }

    std::vector<Item> items;
};
