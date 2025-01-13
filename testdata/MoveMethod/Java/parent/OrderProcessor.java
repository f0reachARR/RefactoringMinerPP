import java.util.List;

public class OrderProcessor {

    private List<Item> items;

    public void processOrder() {
        double total = calculateTotal();
        // Further order processing
    }

    public double calculateTotal() {
        double total = 0;
        for (Item item : items) {
            total += item.getPrice();
        }
        return total;
    }
}

class Item {
    private double price;

    public double getPrice() {
        return price;
    }
}
