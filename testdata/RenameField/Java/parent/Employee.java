import java.util.Date;

public class Employee {
    private String name;
    private int age;
    private Date joinDate;

    public Employee(String name, int age, Date joinDate) {
        this.name = name;
        this.age = age;
        this.joinDate = joinDate;
    }

    public String getName() {
        return name;
    }

    public int getAge() {
        return age;
    }

    public Date getJoinDate() {
        return joinDate;
    }
}
