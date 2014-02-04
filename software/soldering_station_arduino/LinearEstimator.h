
#ifndef __LINEARESTIMATOR_H__
#define __LINEARESTIMATOR_H__

#define B_MULTIPLIER  100

class LinearEstimator {
public:
  LinearEstimator() {};
  
  // Set the A & B parameters
  void setAB(int _a, int _b) { a = _a; b = _b; }
  
  // Get the a parameter (slope)
  int getA() { return a; }
  
  // Get the b parameter (Y axis interception)
  int getB() { return b; }
  
  // Estimate the Y value for a given X value
  int estimateY(int x);
  
  // Estimate the X value for a given Y value
  int estimateX(int y);
  
  // Calculate the parameters a and b based on
  // a set of observation (linear regression)
  void linest(int n, int x[], int y[]);
  
protected:
  int a;
  int b;
};

#endif // __LINEARESTIMATOR_H__
