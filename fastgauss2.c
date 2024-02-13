// A function that returns a fast approximation of a gaussian function
// based on the input x, the mean m, and the standard deviation s
float fastGaussian(float x, float m, float s) {
  // Define some constants
  const float a1 = 0.254829592;
  const float a2 = -0.284496736;
  const float a3 = 1.421413741;
  const float a4 = -1.453152027;
  const float a5 = 1.061405429;
  const float p = 0.3275911;

  // Save the sign of x
  int sign = 1;
  if (x < 0) {
    sign = -1;
  }
  x = abs(x);

  // Compute the approximation
  float t = 1.0 / (1.0 + p * x);
  float y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);

  // Adjust for the mean and standard deviation
  y = y / (s * sqrt(2 * PI));
  y = y * exp(-0.5 * pow((x - m) / s, 2));

  // Return the final result with the sign
  return sign * y;
}
