// Define some constants
#define PI 3.14159265358979323846
#define K1 0.6072529350088812561694 // 1/k for CORDIC
#define N 32 // Number of iterations for CORDIC
#define M 16 // Number of iterations for Taylor series
#define MUL 1073741824.0 // Scaling factor for fixed point arithmetic

// A custom modulo function for floating point numbers
float modulo(float x, float y) {
  return x - y * floor(x / y);
}

// Store the CORDIC angles, factorials, and reciprocals of factorials in flash memory
const int cordic_angles[N] PROGMEM = {0x3243F6A8, 0x1DAC6705, 0x0FADBAFC, 0x07F56EA6, 0x03FEAB76, 0x01FFD55B, 0x00FFFAAA, 0x007FFF55, 0x003FFFEA, 0x001FFFFD, 0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF, 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F, 0x0000000F, 0x00000008, 0x00000004, 0x00000002, 0x00000001, 0x00000000};
const float factorials[N] PROGMEM = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0, 362880.0, 3628800.0, 39916800.0, 479001600.0, 6227020800.0, 87178291200.0, 1307674368000.0, 20922789888000.0, 355687428096000.0, 6402373705728000.0, 121645100408832000.0, 2432902008176640000.0, 51090942171709440000.0, 1124000727777607680000.0, 25852016738884976640000.0, 620448401733239439360000.0, 15511210043330985984000000.0, 403291461126605635584000000.0, 10888869450418352160768000000.0, 304888344611713860501504000000.0, 8841761993739701954543616000000.0, 265252859812191058636308480000000.0, 8222838654177922817725562880000000.0};
const float reciprocals[N] PROGMEM = {1.0, 1.0, 0.5, 0.16666666666666666, 0.041666666666666664, 0.008333333333333333, 0.001388888888888889, 0.0001984126984126984, 2.48015873015873e-05, 2.7557319223985893e-06, 2.755731922398589e-07, 2.505210838544172e-08, 2.08767569878681e-09, 1.6059043836821613e-10, 1.1470745597729725e-11, 7.647163731819816e-13, 4.779477332387385e-14, 2.8114572543455206e-15, 1.5619206968586225e-16, 8.22063524662433e-18, 4.110317623312165e-19, 1.9572941063391263e-20, 8.896791392450574e-22, 3.868170170630684e-23, 1.6117375710961184e-24, 6.446950284384474e-26, 2.479596263224797e-27, 9.183689863795546e-29, 3.279889237069838e-30, 1.1309962886447716e-31, 3.7699876288159056e-33, 1.2161250415535173e-34};

// A function that implements the CORDIC algorithm to compute sin and cos of an angle
void cordic(int z, int *s, int *c, int n)
{
  // Initialize the CORDIC variables
  int x = (int) (K1 * MUL);
  int y = 0;
  int d, tx, ty, tz;
  // Perform the CORDIC iterations
  for (int k = 0; k < n; k++) {
    d = z >= 0 ? 0 : -1;
    tx = x - (((y >> k) ^ d) - d);
    ty = y + (((x >> k) ^ d) - d);
    tz = z - ((pgm_read_dword(&cordic_angles[k]) ^ d) - d);
    x = tx; y = ty; z = tz;
  }
  // Return the results
  *c = x; *s = y;
}

// A function that combines CORDIC and Taylor series to approximate sin(x)
float sin_approx(float x) {
  // Normalize the input to the range [-pi, pi]
  x = modulo(x, 2 * PI);
  if (x > PI) {
    x -= 2 * PI;
  } else if (x < -PI) {
    x += 2 * PI;
  }
  
  // Use CORDIC for the range [-pi/2, pi/2]
  if (x >= -PI / 2 && x <= PI / 2) {
    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the CORDIC variables
    int x, y;
    // Call the CORDIC function
    cordic(z, &y, &x, N);
    // Convert the result to floating point and return
    return (float) y / MUL;
  }
  
  // Use CORDIC for the range [-pi, -pi/2]
  else if (x < -PI / 2) {
    // Add pi to the angle and negate the result
    x = x + PI;
    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the CORDIC variables
    int x, y;
    // Call the CORDIC function
    cordic(z, &y, &x, N);
    // Convert the result to floating point and negate it
    return -(float) y / MUL;
  }
  
  // Use Taylor series for the range [pi/2, pi]
  else {
    // Use the identity sin(x) = sin(pi - x)
    x = PI - x;
    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the Taylor series variables
    int result = z;
    int term = z;
    // Perform the Taylor series iterations
    for (int k = 1; k < M / 2; k++) {
      // Multiply by the reciprocal of the factorial instead of dividing
      term = -term * z * z * pgm_read_float(&reciprocals[2 * k * (2 * k + 1)]);
      result += term;
    }
    // Convert the result to floating point and return
    return (float) result / MUL;
  }
}

// A function that prints the sin approximation and the actual sin value for a given angle in degrees
void print_sin(float angle) {
  // Convert the angle to radians
  float rad = angle * PI / 180;
  // Calculate the sin approximation and the actual sin value
  float approx, actual;
  // Check if the angle is a constant value at compile time
  if (__builtin_constant_p(angle)) {
    // Use the sin() function from the math library
    actual = sin(rad);
  } else {
    // Use the sin_approx() function
    actual = sin_approx(rad);
  }
  approx = sin_approx(rad);
  // Print the results
  Serial.print(F("sin("));
  Serial.print(angle);
  Serial.print(F("°) = "));
  Serial.print(approx);
  Serial.print(F(" (approx), "));
  Serial.print(actual);
  Serial.println(F(" (actual)"));
}

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
  // Print some examples
  for (int angle = 0; angle <= 180; angle += 15) {
    print_sin(angle);
  }
}

void loop() {
  // Do nothing
}
