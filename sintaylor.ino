// Include the library for the OLED display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the pins for the display
#define OLED_MOSI 9
#define OLED_CLK 10
#define OLED_DC 11
#define OLED_CS 12
#define OLED_RESET 13

// Create an object for the display
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


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

void setup() {
  // Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Sine wave approximation");
  display.display();
}

void loop() {
  // Draw a sine wave using different number of terms in the series
  for (int n = 1; n <= 10; n++) {
    // Clear the previous wave
    display.fillRect(0, 16, 128, 48, BLACK);
    // Display the number of terms
    display.setCursor(0, 16);
    display.print("n = ");
    display.println(n);
    // Draw the wave
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
      display.drawPixel(x, y_pixel, WHITE);
    }
    // Update the display
    display.display();
    // Wait for 2 seconds
    delay(2000);
  }
}
