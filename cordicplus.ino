// Define some constants
#define PI 3.14159265358979323846
#define K1 0.6072529350088812561694 // 1/k for CORDIC
#define N 32 // Number of iterations for CORDIC and Taylor series
#define MUL 1073741824.0 // Scaling factor for fixed point arithmetic

// Precompute the CORDIC angles and factorials
int cordic_angles[N] = {0x3243F6A8, 0x1DAC6705, 0x0FADBAFC, 0x07F56EA6, 0x03FEAB76, 0x01FFD55B, 0x00FFFAAA, 0x007FFF55, 0x003FFFEA, 0x001FFFFD, 0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF, 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F, 0x0000000F, 0x00000008, 0x00000004, 0x00000002, 0x00000001, 0x00000000};
double factorials[N] = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0, 362880.0, 3628800.0, 39916800.0, 479001600.0, 6227020800.0, 87178291200.0, 1307674368000.0, 20922789888000.0, 355687428096000.0, 6402373705728000.0, 121645100408832000.0, 2432902008176640000.0, 51090942171709440000.0, 1124000727777607680000.0, 25852016738884976640000.0, 620448401733239439360000.0, 15511210043330985984000000.0, 403291461126605635584000000.0, 10888869450418352160768000000.0, 304888344611713860501504000000.0, 8841761993739701954543616000000.0, 265252859812191058636308480000000.0, 8222838654177922817725562880000000.0};

// A function that combines CORDIC and Taylor series to approximate sin(x)
double sin_approx(double x) {
  // Normalize the input to the range [-pi, pi]
  x = fmod(x, 2 * PI);
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
    int x = (int) (K1 * MUL);
    int y = 0;
    int d, tx, ty, tz;
    // Perform the CORDIC iterations
    for (int k = 0; k < N; k++) {
      d = z >= 0 ? 0 : -1;
      tx = x - (((y >> k) ^ d) - d);
      ty = y + (((x >> k) ^ d) - d);
      tz = z - ((cordic_angles[k] ^ d) - d);
      x = tx; y = ty; z = tz;
    }
    // Convert the result to floating point and return
    return (double) y / MUL;
  }
  
  // Use Taylor series for the range [-pi, -pi/2] and [pi/2, pi]
  else {
    // Use the identity sin(x) = sin(pi - x) for the range [pi/2, pi]
    if (x > PI / 2) {
      x = PI - x;
    }
    // Use the identity sin(x) = -sin(-x) for the range [-pi, -pi/2]
    else if (x < -PI / 2) {
      x = -x;
    }
    // Initialize the Taylor series variables
    double result = x;
    double term = x;
    // Perform the Taylor series iterations
    for (int k = 1; k < N / 2; k++) {
      term = -term * x * x / (2 * k * (2 * k + 1));
      result += term;
    }
    // Return the result
    return result;
  }
}

// A function that prints the sin approximation and the actual sin value for a given angle in degrees
void print_sin(double angle) {
  // Convert the angle to radians
  double rad = angle * PI / 180;
  // Calculate the sin approximation and the actual sin value
  double approx = sin_approx(rad);
  double actual = sin(rad);
  // Print the results
  Serial.print("sin(");
  Serial.print(angle);
  Serial.print("°) = ");
  Serial.print(approx);
  Serial.print(" (approx), ");
  Serial.print(actual);
  Serial.println(" (actual)");
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
