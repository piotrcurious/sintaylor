
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

// A function that maps a double value from one range to another
double dmap(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  // Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Interference pattern");
  display.display();
  // Initialize the random seed
  randomSeed(analogRead(0));
}

void loop() {
  // Draw an interference pattern using the sine function
  // Define the frequencies and phases of the waves in x and y directions
  double fx = 0.1; // Frequency in x direction
  double fy = 0.2; // Frequency in y direction
  double px = PI / 4; // Phase in x direction
  double py = PI / 2; // Phase in y direction
  // Loop through each pixel on the display
  for (int x = 0; x < 128; x++) {
    for (int y = 0; y < 64; y++) {
      // Calculate the angle for each wave
      double ax = dmap(x, 0, 127, 0, 2 * PI);
      double ay = dmap(y, 0, 63, 0, 2 * PI);
      // Calculate the sine value for each wave
      double sx = sinTaylor(ax * fx + px, 10);
      double sy = sinTaylor(ay * fy + py, 10);
      // Calculate the magnitude of the interference
      double mag = sqrt(sx * sx + sy * sy);
      // Map the magnitude to a probability threshold
      double prob = dmap(mag, 0, 1.414, 0, 1);
      // Draw a pixel with a random chance based on the probability
      if (random(0, 100) < prob * 100) {
        display.drawPixel(x, y, WHITE);
      } else {
        display.drawPixel(x, y, BLACK);
      }
    }
  }
  // Update the display
  display.display();
  // Wait for 5 seconds
  delay(5000);
}
