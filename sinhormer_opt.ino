// A function that approximates the sine of an angle x (in radians) using Horner's method and quadrant mirroring
double horner_sin(double x) {
  // Define the coefficients of the Taylor series expansion of sin(x) around 0
  double coeffs[] = {1.0, -1.0/6.0, 1.0/120.0, -1.0/5040.0, 1.0/362880.0};
  // Define the number of terms to use in the approximation
  int n = 5;
  // Initialize the result variable
  double result = 0.0;
  // Initialize a variable to store the sign of the result
  int sign = 1;
  // Map the angle x to the first quadrant using quadrant mirroring
  if (x < 0) {
    // If x is negative, make it positive
    x = -x;
  }
  if (x > PI) {
    // If x is larger than pi, subtract pi and flip the sign
    x = x - PI;
    sign = -sign;
  }
  if (x > PI / 2) {
    // If x is larger than pi/2, subtract pi/2 and flip the sign
    x = x - PI / 2;
    sign = -sign;
  }
  if (x > PI / 4) {
    // If x is larger than pi/4, use the identity sin(x) = cos(pi/2 - x)
    x = PI / 2 - x;
  }
  // Loop from the last coefficient to the first
  for (int i = n - 1; i >= 0; i--) {
    // Update the result using Horner's rule
    result = result * x * x + coeffs[i];
  }
  // Multiply the result by x and the sign
  result = result * x * sign;
  // Return the result
  return result;
}
