// Arduino code implementing CORDIC approximation of sine function
// with angle in 0 to 2*PI range

// Define the number of iterations for the CORDIC algorithm
#define N 16

// Define the scaling factor for the input angle
#define K 1.64676025812107

// Define the lookup table of arctangent values
const float arctan[N] = {
  0.78539816339745, 0.46364760900081, 0.24497866312686, 0.12435499454676,
  0.06241880999596, 0.03123983343027, 0.01562372862048, 0.00781234106010,
  0.00390623013197, 0.00195312251648, 0.00097656218956, 0.00048828121119,
  0.00024414062015, 0.00012207031189, 0.00006103515617, 0.00003051757812
};

// Define the function to compute the sine of an angle using CORDIC
float cordic_sin(float theta) {
  // Normalize the angle to the range [0, 2*PI)
  theta = fmod(theta, 2 * PI);
  if (theta < 0) {
    theta += 2 * PI;
  }

  // Initialize the CORDIC variables
  float x = 1 / K;
  float y = 0;
  float z = theta;

  // Perform the CORDIC iterations
  for (int i = 0; i < N; i++) {
    float dx, dy, dz;
    if (z >= 0) {
      dx = -y / pow(2, i);
      dy = x / pow(2, i);
      dz = -arctan[i];
    } else {
      dx = y / pow(2, i);
      dy = -x / pow(2, i);
      dz = arctan[i];
    }
    x += dx;
    y += dy;
    z += dz;
  }

  // Return the y coordinate as the sine value
  return y;
}
