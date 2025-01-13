public class Animal {
    protected String name;

    public Animal(String name) {
        this.name = name;
    }
}

public class Dog extends Animal {

    public Dog(String name) {
        super(name);
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
