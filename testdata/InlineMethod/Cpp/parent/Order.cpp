#include <vector>

class Item {
public:
    double getPrice() const { return price; }
private:
    double price;
};

class Order {
private:
    std::vector<Item> items;

public:
    double calculateTotal() {
        return sumItems() * 1.1;  // Apply tax
    }

    double sumItems() {
        double total = 0;
        for (const auto& item : items) {
            total += item.getPrice();
        }
        return total;
    }
};
