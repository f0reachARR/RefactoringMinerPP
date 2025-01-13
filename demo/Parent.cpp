class Circle{
  double PI = 3.14159;
public:
  double getArea(double r){
    return PI * r * r;
  }
  double calcCircumference(double radius){
    double diameter = radius * radius; //bug
    return PI * diameter;
  }
};