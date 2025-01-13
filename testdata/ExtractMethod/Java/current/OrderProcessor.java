import java.util.List;

public class OrderProcessor {

    public void processOrder(List<String> items) {
        double total = calculateTotal(items);
        System.out.println("Total: " + total);
    }

    private double calculateTotal(List<String> items) {
        double total = 0;
        for (String item : items) {
            if (item.equals("itemA")) {
                total += 10;
            } else if (item.equals("itemB")) {
                total += 20;
            } else if (item.equals("itemC")) {
                total += 30;
            }
        }
        return total;
    }
}
