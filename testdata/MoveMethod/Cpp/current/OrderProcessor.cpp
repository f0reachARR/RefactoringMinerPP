#include <vector>

class Item {
public:
    double getPrice() const {
        return price;
    }

    static double calculateTotal(const std::vector<Item>& items) {
        double total = 0;
        for (const Item& item : items) {
            total += item.getPrice();
        }
        return total;
    }

private:
    double price;
};

class OrderProcessor {
public:
    void processOrder() {
        double total = Item::calculateTotal(items);
        // Further order processing
    }

private:
    std::vector<Item> items;
};
