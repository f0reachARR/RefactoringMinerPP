import java.util.Date;

public class Employee {
    private String employeeName;  // Field renamed from 'name' to 'employeeName'
    private int age;
    private Date joinDate;

    public Employee(String employeeName, int age, Date joinDate) {
        this.employeeName = employeeName;
        this.age = age;
        this.joinDate = joinDate;
    }

    public String getEmployeeName() {  // Getter method renamed to reflect the new field name
        return employeeName;
    }

    public int getAge() {
        return age;
    }

    public Date getJoinDate() {
        return joinDate;
    }
}
