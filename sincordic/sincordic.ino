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


// Define some constants
//#define PI 3.14159265358979323846
#define K1 0.6072529350088812561694 // 1/k for CORDIC
#define N 32 // Number of iterations for CORDIC and Taylor series
#define MUL 1073741824.0 // Scaling factor for fixed point arithmetic

// Precompute the CORDIC angles and factorials
int cordic_angles[N] = {0x3243F6A8, 0x1DAC6705, 0x0FADBAFC, 0x07F56EA6, 0x03FEAB76, 0x01FFD55B, 0x00FFFAAA, 0x007FFF55, 0x003FFFEA, 0x001FFFFD, 0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF, 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F, 0x0000000F, 0x00000008, 0x00000004, 0x00000002, 0x00000001, 0x00000000};
double factorials[N] = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0, 362880.0, 3628800.0, 39916800.0, 479001600.0, 6227020800.0, 87178291200.0, 1307674368000.0, 20922789888000.0, 355687428096000.0, 6402373705728000.0, 121645100408832000.0, 2432902008176640000.0, 51090942171709440000.0, 1124000727777607680000.0, 25852016738884976640000.0, 620448401733239439360000.0, 15511210043330985984000000.0, 403291461126605635584000000.0, 10888869450418352160768000000.0, 304888344611713860501504000000.0, 8841761993739701954543616000000.0, 265252859812191058636308480000000.0, 8222838654177922817725562880000000.0};

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
    tz = z - ((cordic_angles[k] ^ d) - d);
    x = tx; y = ty; z = tz;
  }
  // Return the results
  *c = x; *s = y;
}

// A function that combines CORDIC and Taylor series to approximate sin(x)
double sin_approx(double x, int n) { // angle x, iterations n
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
    int x, y;
    // Call the CORDIC function
    cordic(z, &y, &x, N);
    // Adjust the output according to the quadrant of the original angle 
//    if (x < 0) y = -y; 
    // Convert the result to floating point and return   
    return (double) y / MUL;
  }

  // Use CORDIC for the range [-pi, -pi/2]
  else if (x < -PI / 2 ) {
    // Add pi to the angle and negate the result
    x = x + PI;
    // Convert the input to fixed point
    int z = (int) (x * MUL);
    // Initialize the CORDIC variables
    int x, y;
    // Call the CORDIC function
    cordic(z, &y, &x, N);
    // Adjust the output according to the quadrant of the original angle 
//    if (x < 0) y = -y; 
    // Convert the result to floating point and return   
    return -(double) y / MUL;
  }

 
  // Use Taylor series for the range [-pi, -pi/2] and [pi/2, pi]
  else {

//    // Use the identity sin(x) = sin(pi - x) for the range [pi/2, pi]
//    if (x > PI / 2) {
//      x = PI - x;
//    }
//    // Use the identity sin(x) = -sin(-x) for the range [-pi, -pi/2]
//    else if (x < -PI / 2) {
//      x = +x;
//    }

    //Use the identity sin(x) = sin(pi-x) 
    x = PI -x ;
    // Initialize the Taylor series variables
    double result = x;
    double term = x;
    // Perform the Taylor series iterations
    for (int k = 1; k < n / 2; k++) {
      term = -term * x * x / (2 * k * (2 * k + 1));
      result += term;
    }
    // Return the result
    return result;
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
  for (int n = 1; n <= 12; n++) {
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
      double angle = dmap(x, 0, 127, 0, 2 * PI);
   
      // Calculate the y coordinate using the sine function
      double y = sin(angle);
      // Map the y coordinate to the pixel position
      int y_pixel = dmap(y, -1, 1, 63, 17);
      
      // Map the y coordinate to the pixel position
      // bug int y_pixel = map(y, -1, 1, 63, 17);
      // Draw a pixel at the position
//      display.drawPixel(x, y_pixel, 2);// xor so it should cancel out
      display.drawPixel(x, y_pixel, WHITE);// xor so it should cancel out
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
      double y = sinHorner(angle, n);
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
      double angle = dmap(x, 0, 127, 0, 2 * PI);
//      double angle = dmap(x, 0, 127, -PI/2,  PI/2);
//      double angle = dmap(x, 0, 127, -PI, PI);
   
      // Calculate the y coordinate using the sine function
      double y = sin_approx(angle, n);
//      double y = cordic_sin(angle);
//      double y = sin_approx(angle);

      // Map the y coordinate to the pixel position
      int y_pixel = dmap(y, -1, 1, 63, 17);
//      int y_pixel = dmap(y, -2*PI, 2*PI, 63, 17);
      
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
    delay(2000);
  }
}
