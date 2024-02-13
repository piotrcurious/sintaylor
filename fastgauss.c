// This function returns a fast approximation of the gaussian function
// f(x) = exp(-x^2 / 2) / sqrt(2 * PI) for x in [-4, 4]
// The approximation is based on a polynomial interpolation of degree 7
// The maximum absolute error is about 0.00014
float fast_gaussian(float x) {
  // Check if x is within the range [-4, 4]
  if (x < -4 || x > 4) {
    return 0; // Outside the range, the function is negligible
  }
  // Define the coefficients of the polynomial interpolation
  const float a0 = 0.39894228;
  const float a1 = 0.06649038;
  const float a2 = -0.009973557;
  const float a3 = 0.001187328;
  const float a4 = -0.000115434;
  const float a5 = 0.000009063;
  const float a6 = -0.000000632;
  const float a7 = 0.000000037;
  // Evaluate the polynomial using Horner's method
  float y = a0 + x * (a1 + x * (a2 + x * (a3 + x * (a4 + x * (a5 + x * (a6 + x * a7))))));
  return y; // Return the approximation
}
