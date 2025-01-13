// File: ClassExample.java
public class ChildClass extends ParentClass {
    public void childMethod() {
        System.out.println("Child method");
    }
    
    public void commonMethod() {
        System.out.println("Common method in Child");
    }
}

class ParentClass {
    // No commonMethod in ParentClass
}
