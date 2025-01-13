// Necessary imports
public class User {
    private String name; // The field to be moved

    public User(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}

public class Address {
    private String city;

    public Address(String city) {
        this.city = city;
    }

    public String getCity() {
        return city;
    }
}
