import java.util.List;

public class OrderProcessor {

    private List<Item> items;

    public void processOrder() {
        double total = calculateTotal(items);
        // Further order processing
    }
}

class Item {
    private double price;

    public double getPrice() {
        return price;
    }

    public static double calculateTotal(List<Item> items) {
        double total = 0;
        for (Item item : items) {
            total += item.getPrice();
        }
        return total;
    }
}
