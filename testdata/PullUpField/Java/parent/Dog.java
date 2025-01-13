public class Dog {
    protected String name;

    public Dog(String name) {
        this.name = name;
    }

    public void bark() {
        System.out.println(name + " is barking!");
    }
}

public class Poodle extends Dog {

    public Poodle(String name) {
        super(name);
    }
}
