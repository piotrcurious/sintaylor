// A function that approximates the sine of an angle x (in radians) using a Horner's method
// The parameter n is the number of terms to use in the series
double sinHorner(double x, int n) {
  double sum = 0; // The sum of the series
  double term = 1; // The current term in the series
  int sign = -1; // The sign of the current term
  for (int i = n; i >= 1; i--) {
    term = 1 - x * x * term / ((2 * i) * (2 * i + 1)); // Update the term using the formula
    sum = x * term + sign * sum; // Add the term to the sum
    sign = -sign; // Flip the sign for the next term
  }
  return sum; // Return the final sum
}
