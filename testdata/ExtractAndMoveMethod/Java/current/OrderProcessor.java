import java.util.List;

public class OrderProcessor {

    private PriceCalculator priceCalculator = new PriceCalculator();

    public void processOrder(List<String> items) {
        double total = priceCalculator.calculateTotal(items);
        applyDiscount(total);
        System.out.println("Total: " + total);
    }

    private void applyDiscount(double total) {
        // Apply a discount if total is above a certain amount
        if (total > 100) {
            total *= 0.9;
        }
    }
}

public class PriceCalculator {

    public double calculateTotal(List<String> items) {
        double total = 0;
        for (String item : items) {
            total += getItemPrice(item);
        }
        return total;
    }

    public double getItemPrice(String item) {
        // Imagine this fetches price from a database
        return 20.0;
    }
}
