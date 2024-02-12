// A function that implements the CORDIC algorithm to calculate the sine and cosine of an angle x (in radians)
// The parameter n is the number of iterations to use in the algorithm
// The function returns a struct containing the sine and cosine values
struct cordic_result {
  double sin;
  double cos;
};

struct cordic_result cordic(double x, int n) {
  // Define some constants for the algorithm
  const double K = 0.607252935; // The constant scale factor
  const double angles[] = {0.785398163, 0.463647609, 0.244978663, 0.124354995, 0.062418810, 0.031239833, 0.015623729, 0.007812341, 0.003906230, 0.001953123, 0.000976562, 0.000488281, 0.000244141, 0.000122070, 0.000061035, 0.000030518, 0.000015259, 0.000007629, 0.000003815, 0.000001907, 0.000000954, 0.000000477, 0.000000238, 0.000000119, 0.000000060, 0.000000030, 0.000000015, 0.000000007, 0.000000004, 0.000000002, 0.000000001}; // The table of elementary angles
  // Initialize the variables for the algorithm
  double x0 = K; // The initial x coordinate
  double y0 = 0; // The initial y coordinate
  double z0 = x; // The initial angle
  double x1, y1, z1; // The updated variables
  int d; // The direction of rotation
  // Create a struct to store the result
  struct cordic_result result;
  // Loop through the iterations
  for (int i = 0; i < n; i++) {
    // Determine the direction of rotation
    if (z0 < 0) {
      d = -1;
    } else {
      d = 1;
    }
    // Update the variables using the CORDIC formula
    x1 = x0 - d * (y0 >> i);
    y1 = y0 + d * (x0 >> i);
    z1 = z0 - d * angles[i];
    // Assign the updated variables to the previous ones
    x0 = x1;
    y0 = y1;
    z0 = z1;
  }
  // Assign the final values to the result struct
  result.sin = y0;
  result.cos = x0;
  // Return the result struct
  return result;
}

void setup() {
  Serial.begin(9600); // Start serial communication
}

void loop() {
  // Test the function with some values
  struct cordic_result r; // A struct to store the result
  r = cordic(0, 10); // Calculate the sine and cosine of 0 radians using 10 iterations
  Serial.print("sin(0) = ");
  Serial.println(r.sin); // Should be 0
  Serial.print("cos(0) = ");
  Serial.println(r.cos); // Should be 1
  r = cordic(PI / 6, 10); // Calculate the sine and cosine of pi/6 radians using 10 iterations
  Serial.print("sin(pi/6) = ");
  Serial.println(r.sin); // Should be 0.5
  Serial.print("cos(pi/6) = ");
  Serial.println(r.cos); // Should be 0.866
  r = cordic(PI / 2, 10); // Calculate the sine and cosine of pi/2 radians using 10 iterations
  Serial.print("sin(pi/2) = ");
  Serial.println(r.sin); // Should be 1
  Serial.print("cos(pi/2) = ");
  Serial.println(r.cos); // Should be 0
  r = cordic(PI, 10); // Calculate the sine and cosine of pi radians using 10 iterations
  Serial.print("sin(pi) = ");
  Serial.println(r.sin); // Should be 0
  Serial.print("cos(pi) = ");
  Serial.println(r.cos); // Should be -1
  r = cordic(3 * PI / 2, 10); // Calculate the sine and cosine of 3*pi/2 radians using 10 iterations
  Serial.print("sin(3*pi/2) = ");
  Serial.println(r.sin); // Should be -1
  Serial.print("cos(3*pi/2) = ");
  Serial.println(r.cos); // Should be 0
  r = cordic(2 * PI, 10); // Calculate the sine and cosine of 2*pi radians using 10 iterations
  Serial.print("sin(2*pi) = ");
  Serial.println(r.sin); // Should be 0
  Serial.print("cos(2*pi) = ");
  Serial.println(r.cos); // Should be 1
  delay(5000); // Wait for 5 seconds
}
