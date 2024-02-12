// CORDIC approximation of sine function
// Based on https://en.wikipedia.org/wiki/CORDIC#Example_code_in_C

// Define the number of bits in the fixed-point representation
#define FIXED_POINT_BITS 32

// Define the scaling factor for the fixed-point representation
#define FIXED_POINT_SCALE (1 << (FIXED_POINT_BITS - 1))

// Define the value of pi in the fixed-point representation
#define FIXED_POINT_PI (int32_t)(3.14159265358979323846 * FIXED_POINT_SCALE)

// Define a table of CORDIC angles in the fixed-point representation
// The angles are arctan(2^-i) for i = 0, 1, ..., 31
const int32_t cordic_angles[FIXED_POINT_BITS] = {
    843314857, 497837829, 263043837, 133525159, 67021687, 33543516, 16775851,
    8388437, 4194283, 2097149, 1048576, 524288, 262144, 131072, 65536, 32768,
    16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1
};

// Define a function to convert an angle in radians to the fixed-point representation
int32_t rad_to_fixed(double rad) {
  return (int32_t)(rad * FIXED_POINT_SCALE);
}

// Define a function to convert an angle in the fixed-point representation to radians
double fixed_to_rad(int32_t fixed) {
  return (double)fixed / FIXED_POINT_SCALE;
}

// Define a function to compute the sine of an angle in the fixed-point representation
// using the CORDIC algorithm with a variable iterative depth as a parameter
int32_t cordic_sin(int32_t angle, int depth) {
  // Initialize the CORDIC vector (x, y, z) to (1, 0, angle)
  int32_t x = FIXED_POINT_SCALE;
  int32_t y = 0;
  int32_t z = angle;

  // Loop over the CORDIC angles
  for (int i = 0; i < depth; i++) {
    // Store the current x and y values
    int32_t x_temp = x;
    int32_t y_temp = y;

    // Determine the direction of the rotation
    int d = z >= 0 ? -1 : 1;

    // Perform the CORDIC rotation
    x = x_temp - d * (y_temp >> i);
    y = y_temp + d * (x_temp >> i);
    z = z - d * cordic_angles[i];
  }

  // Return the y value as the sine of the angle
  return y;
}

// Define a function to print the sine of an angle in radians using the CORDIC algorithm
void print_cordic_sin(double rad, int depth) {
  // Convert the angle to the fixed-point representation
  int32_t angle = rad_to_fixed(rad);

  // Compute the sine using the CORDIC algorithm
  int32_t sine = cordic_sin(angle, depth);

  // Convert the sine to a double
  double result = fixed_to_rad(sine);

  // Print the result
  Serial.print("sin(");
  Serial.print(rad, 6);
  Serial.print(") = ");
  Serial.println(result, 6);
}

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
}

void loop() {
  // Print the sine of some angles using different depths
  print_cordic_sin(0.0, 10); // depth = 10
  print_cordic_sin(0.0, 20); // depth = 20
  print_cordic_sin(0.0, 30); // depth = 30
  print_cordic_sin(0.5, 10); // depth = 10
  print_cordic_sin(0.5, 20); // depth = 20
  print_cordic_sin(0.5, 30); // depth = 30
  print_cordic_sin(1.0, 10); // depth = 10
  print_cordic_sin(1.0, 20); // depth = 20
  print_cordic_sin(1.0, 30); // depth = 30

  // Wait for a second
  delay(1000);
}
