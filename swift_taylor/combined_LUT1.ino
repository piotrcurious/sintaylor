#define PI 3.14159265359

// Lookup table size (adjust as needed)
const int TABLE_SIZE = 256;

// Pre-computed sine and Taylor series correction values for combined lookup
float combinedLUT[(TABLE_SIZE + 1) * 5]; // Each element stores sin(index * 2*PI/TABLE_SIZE) + correction for i=1,3,5,7,9

// Feature hash size (adjust based on desired cache size)
const int FEATURE_HASH_SIZE = 32;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Fill the combined lookup table
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * (2 * PI / TABLE_SIZE);
    float sineValue = sin(angle);
    float correction = 0.0f;
    for (int j = 1; j < 10; j += 2) {
      float power = pow(-1.0f, (j - 1) / 2.0f);
      correction += power * pow(angle, j) / factorial(j); // Calculate and store correction
    }
    combinedLUT[i * 5] = sineValue;
    combinedLUT[i * 5 + 1] = correction; // Store even terms correction
    combinedLUT[i * 5 + 2] = 0.0f; // Placeholder for odd terms correction
    combinedLUT[i * 5 + 3] = 0.0f; // Placeholders for future optimizations
    combinedLUT[i * 5 + 4] = 0.0f;
  }

  // Pre-calculate odd terms corrections (optional, adjust based on accuracy needs)
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * (2 * PI / TABLE_SIZE);
    float oddCorrection = 0.0f;
    for (int j = 3; j < 10; j += 4) {
      float power = pow(-1.0f, (j - 1) / 2.0f);
      oddCorrection += power * pow(angle, j) / factorial(j);
    }
    combinedLUT[i * 5 + 2] = oddCorrection;
  }
}

float sine(float angle) {
  // Normalize angle to the range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate lookup table index (optimized for combined table)
  int index = (int)(angle * (TABLE_SIZE - 1) / (2 * PI));

  // Use combined lookup table (optimized for speed)
  float sineValue = combinedLUT[index * 5];
  float evenCorrection = combinedLUT[index * 5 + 1];
  float oddCorrection = combinedLUT[index * 5 + 2]; // Use pre-computed odd term correction

  // Return the final sine value with combined correction
  return sineValue + evenCorrection + oddCorrection;
}

// Optimized factorial calculation (optional, choose your preferred method)
int factorial(int n) {
  int result = 1;
  for (int i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}

void loop() {
  // Get angle input from serial
  float angle = Serial.parseFloat();

  // Calculate and print sine value
  float sineValue = sine(angle);
  Serial.print("Sin(");
  Serial.print(angle);
  Serial.print(") = ");
  Serial.println(sineValue);

  // Delay for next input
  delay(1000);
}
