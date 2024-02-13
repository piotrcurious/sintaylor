// Include the library for the OLED display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display dimensions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

/*
// Define the pins for the display
#define OLED_MOSI 9
#define OLED_CLK 10
#define OLED_DC 11
#define OLED_CS 12
#define OLED_RESET 13

// Create an object for the display
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
*/

// Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     D2
#define OLED_CS     D8
#define OLED_RESET  D3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);


//for time accounting
uint32_t start_classic = 0 ; 
uint32_t stop_classic = 0 ; 

uint32_t start_taylor = 0 ; 
uint32_t stop_taylor = 0 ; 

uint32_t start_horner = 0 ; 
uint32_t stop_horner = 0 ; 

uint32_t start_cordic = 0 ; 
uint32_t stop_cordic = 0 ; 


// A function that maps a double value from one range to another
double dmap(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// A function that approximates the sine of an angle x (in radians) using a Taylor series expansion
// The parameter n is the number of terms to use in the series
double sinTaylor(double x, int n) {
  double sum = 0; // The sum of the series
  double term = x; // The current term in the series
  int sign = 1; // The sign of the current term
  for (int i = 1; i <= n; i++) {
    sum += sign * term; // Add the term to the sum
    term *= x * x / ((2 * i) * (2 * i + 1)); // Update the term using the formula
    sign = -sign; // Flip the sign for the next term
  }
  return sum; // Return the final sum
}
// A function that approximates the sine of an angle x (in radians) using Horner's method and quadrant mirroring
// n is the number of terms to use in the approximation
double sin_horner(double x, int n) {
  // Define the coefficients of the Taylor series expansion of sin(x) around 0
  double coeffs[] = {1.0, -1.0/6.0, 1.0/120.0, -1.0/5040.0, 1.0/362880.0, -1.0/39916800.0};
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
  // Loop from the last coefficient to the first, up to n terms
  for (int i = n - 1; i >= 0; i--) {
    // Update the result using Horner's rule
    result = result * x * x + coeffs[i];
  }
  // Multiply the result by x and the sign
  result = result * x * sign;
  // Return the result
  return result;
}


//HYBRID CORDIC section

// Define some constants
//#define PI 3.14159265358979323846
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
//const float factorials[N] PROGMEM = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0, 362880.0, 3628800.0, 39916800.0, 479001600.0, 6227020800.0, 87178291200.0, 1307674368000.0, 20922789888000.0, 355687428096000.0, 6402373705728000.0, 121645100408832000.0, 2432902008176640000.0, 51090942171709440000.0, 1124000727777607680000.0, 25852016738884976640000.0, 620448401733239439360000.0, 15511210043330985984000000.0, 403291461126605635584000000.0, 10888869450418352160768000000.0, 304888344611713860501504000000.0, 8841761993739701954543616000000.0, 265252859812191058636308480000000.0, 8222838654177922817725562880000000.0};
//const float reciprocals[N] PROGMEM = {1.0, 1.0, 0.5, 0.16666666666666666, 0.041666666666666664, 0.008333333333333333, 0.001388888888888889, 0.0001984126984126984, 2.48015873015873e-05, 2.7557319223985893e-06, 2.755731922398589e-07, 2.505210838544172e-08, 2.08767569878681e-09, 1.6059043836821613e-10, 1.1470745597729725e-11, 7.647163731819816e-13, 4.779477332387385e-14, 2.8114572543455206e-15, 1.5619206968586225e-16, 8.22063524662433e-18, 4.110317623312165e-19, 1.9572941063391263e-20, 8.896791392450574e-22, 3.868170170630684e-23, 1.6117375710961184e-24, 6.446950284384474e-26, 2.479596263224797e-27, 9.183689863795546e-29, 3.279889237069838e-30, 1.1309962886447716e-31, 3.7699876288159056e-33, 1.2161250415535173e-34};

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
//  *c = x; *s = y;
  *s = y; // return only sine 

}

// A function that combines CORDIC and Taylor series to approximate sin(x)
float sin_approx(float x, int n) {
  // Normalize the input to the range [-pi, pi]
//  x = modulo(x, 2 * PI);
  x = fmod(x, 2 * PI); // this is actually faster

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
    cordic(z, &y, &x, n);
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
    cordic(z, &y, &x, n);
    // Convert the result to floating point and negate it
    return -(float) y / MUL;
  }
  
  // Use Taylor series for the range [pi/2, pi]

/*
  else {
    // Use the identity sin(x) = sin(pi - x)
    x = PI - x;
    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the Taylor series variables
    int result = z;
    int term = z;
    // Perform the Taylor series iterations
    for (int k = 1; k < n / 2; k++) {
      // Multiply by the reciprocal of the factorial instead of dividing
      term = -term * z * z * pgm_read_float(&reciprocals[2 * k * (2 * k + 1)]);
      result += term;
    }
    // Convert the result to floating point and return
    return (float) result / MUL;
  }
*/

/*
 else {
    // Use the identity sin(x) = sin(pi - x)
    x = PI - x;
    // Initialize the Taylor series variables
    float result = x;
    float term = x;
    // Perform the Taylor series iterations
    for (int k = 1; k < n / 2; k++) {
      term = -term * x * x / (2 * k * (2 * k + 1));
      result += term;
    }
    // Return the result
    return result;
  }
*/

    else {
    // Add pi to the angle and negate the result
//    x = x + PI;
    x = PI -x ;

    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the CORDIC variables
    int x, y;
    // Call the CORDIC function
    cordic(z, &y, &x, n);
    // Convert the result to floating point and negate it
    return (float) y / MUL;
  }


}

void setup() {
  // Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  // Draw a sine wave using different number of terms in the series
  for (int n = 1; n <= N; n++) {
    // Clear the previous wave
//    display.fillRect(0, 16, 128, 48, BLACK);
    display.clearDisplay();
    // Display the number of terms
    display.setCursor(0, 0);
    display.print("n = ");
    display.print(n);

      start_classic = micros();
// classic sine for reference
        for (int x = 0; x < 128; x++) {
      // Map the x coordinate to the angle in radians
      // bug double angle = map(x, 0, 127, 0, 2 * PI);
      // Map the x coordinate to the angle in radians
//      double angle = dmap(x, 0, 127, 0, 2 * PI);
      float angle = fmap(x, 0, 127, 0, 2 * PI);
   
      // Calculate the y coordinate using the sine function
//      double y = sin(angle);
      float y = sin(angle);

      // Map the y coordinate to the pixel position
//      int y_pixel = dmap(y, -1, 1, 63, 17);
      int y_pixel = fmap(y, -1, 1, 63, 17);

      
      // Map the y coordinate to the pixel position
      // bug int y_pixel = map(y, -1, 1, 63, 17);
      // Draw a pixel at the position
//      display.drawPixel(x, y_pixel, 2);// xor so it should cancel out
//      display.drawPixel(x, y_pixel, WHITE);// xor so it should cancel out
      display.drawPixel(x, y_pixel, BLACK);// black - computed but invisible

    }
    stop_classic = micros();

    start_taylor = micros();
    // Draw the taylor series approx sine
    for (int x = 0; x < 128; x++) {
      // Map the x coordinate to the angle in radians
      // bug double angle = map(x, 0, 127, 0, 2 * PI);
      // Map the x coordinate to the angle in radians
      double angle = dmap(x, 0, 127, 0, 2 * PI);
   
      // Calculate the y coordinate using the sine function
      double y = sinTaylor(angle, n);
      // Map the y coordinate to the pixel position
      int y_pixel = dmap(y, -1, 1, 63, 17);
      
      // Map the y coordinate to the pixel position
      // bug int y_pixel = map(y, -1, 1, 63, 17);
      // Draw a pixel at the position
//      display.drawPixel(x, y_pixel, BLACK); // commented out - not computed
//      display.drawPixel(x, y_pixel, BLACK);

    }
    stop_taylor = micros();

    start_horner = micros();
    // Draw the horner series approx sine
    for (int x = 0; x < 128; x++) {
      // Map the x coordinate to the angle in radians
      // bug double angle = map(x, 0, 127, 0, 2 * PI);
      // Map the x coordinate to the angle in radians
      double angle = dmap(x, 0, 127, 0, 2 * PI);
//      double angle = dmap(x, 0, 127, -PI/2,+PI/2);
   
      // Calculate the y coordinate using the sine function
      double y = sin_horner(angle, n);
      // Map the y coordinate to the pixel position
      int y_pixel = dmap(y, -1, 1, 63, 17);
      
      // Map the y coordinate to the pixel position
      // bug int y_pixel = map(y, -1, 1, 63, 17);
      // Draw a pixel at the position
//      display.drawPixel(x, y_pixel, WHITE);
    }
    stop_horner = micros();

    start_cordic = micros();
    // Draw the hybrid cordic/taylor series approx sine
    for (int x = 0; x < 128; x++) {
      // Map the x coordinate to the angle in radians
      // bug double angle = map(x, 0, 127, 0, 2 * PI);
      // Map the x coordinate to the angle in radians
//      double angle = dmap(x, 0, 127, 0, 2 * PI);
      float angle = fmap(x, 0, 127, 0, 2 * PI);

//      double angle = dmap(x, 0, 127, -PI/2,  PI/2);
//      double angle = dmap(x, 0, 127, -PI, PI);
   
      // Calculate the y coordinate using the sine function

//      double y = sin_approx(angle, n);
      float y = sin_approx(angle, n);

      // Map the y coordinate to the pixel position
//      int y_pixel = dmap(y, -1, 1, 63, 17);
      int y_pixel = fmap(y, -1, 1, 63, 17);
      
      // Map the y coordinate to the pixel position
      // bug int y_pixel = map(y, -1, 1, 63, 17);
      // Draw a pixel at the position
//      display.drawPixel(x, y_pixel, 2); // XOR to see error
      display.drawPixel(x, y_pixel, WHITE); // 

    }
    stop_cordic = micros();


    display.setCursor(8*6, 0);
    display.print("clasic:");
    display.println(stop_classic-start_classic);

    display.setCursor(8*6, 8);
    display.print("taylor:");
    display.println(stop_taylor-start_taylor);

    display.setCursor(8*6, 8+8);
    display.print("horner:");
    display.println(stop_horner-start_horner);

    display.setCursor(8*6, 8+8+8);
    display.print("CORDIC:");
    display.println(stop_cordic-start_cordic);

    // Update the display
    display.display();

    // Wait for 2 seconds
    delay(20);
  }
}
