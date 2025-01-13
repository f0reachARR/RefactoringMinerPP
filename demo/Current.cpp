class CircleCalculator{
  inline static const double PI = 3.14159;
public:
  static double calcArea(double radius){
    return PI * radius * radius;
  }
  static double calcSectorArea(double radius,
                               double angle){
    return angle / 360 * calcArea(radius);
  }
  static double calcCircumference(double radius){
    double diameter = radius + radius;
    return PI * diameter;
  }
};