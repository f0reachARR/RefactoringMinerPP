// Necessary imports
public class User {
    public User() {
        // No fields in this class now
    }
}

public class Address {
    private String city;
    private String name; // Field moved here

    public Address(String city, String name) {
        this.city = city;
        this.name = name;
    }

    public String getCity() {
        return city;
    }

    public String getName() {
        return name;
    }
}
