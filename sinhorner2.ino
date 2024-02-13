// A function that approximates the sine of an angle x (in radians) using Horner's method
double horner_sin(double x) {
  // Define the coefficients of the Taylor series expansion of sin(x) around 0
  double coeffs[] = {1.0, -1.0/6.0, 1.0/120.0, -1.0/5040.0, 1.0/362880.0};
  // Define the number of terms to use in the approximation
  int n = 5;
  // Initialize the result variable
  double result = 0.0;
  // Loop from the last coefficient to the first
  for (int i = n - 1; i >= 0; i--) {
    // Update the result using Horner's rule
    result = result * x * x + coeffs[i];
  }
  // Multiply the result by x
  result = result * x;
  // Return the result
  return result;
}
