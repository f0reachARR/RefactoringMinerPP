import java.util.List;

public class Order {
    private List<Item> items;

    public double calculateTotal() {
        double total = 0;
        for (Item item : items) {
            total += item.getPrice();
        }
        return total * 1.1;  // Apply tax
    }
}
