// Dreamed by Gemini
#define PI 3.14159265359

// Lookup table size (adjust as needed)
const int TABLE_SIZE = 256;

// Pre-computed sine values for the lookup table
float sineTable[TABLE_SIZE];

// Pre-computed factorials for Taylor series optimization
float factorialTable[10]; // Up to 9th degree

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Fill the sine lookup table
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * (2 * PI / TABLE_SIZE);
    sineTable[i] = sin(angle);
  }

  // Fill the factorial lookup table
  factorialTable[0] = 1.0f;
  for (int i = 1; i < 10; i++) {
    factorialTable[i] = factorialTable[i-1] * i;
  }
}

float sine(float angle) {
  // Normalize angle to the range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate lookup table index with interpolation
  float index = angle * (TABLE_SIZE - 1) / (2 * PI);
  int indexFloor = (int)floor(index);
  float indexFraction = index - indexFloor;

  // Use linear interpolation between table values
  float value = sineTable[indexFloor] * (1 - indexFraction) + sineTable[indexFloor + 1] * indexFraction;

  // Calculate Taylor series correction for improved accuracy (up to 9th degree)
  float correction = 0.0f;
  for (int i = 1; i < 10; i += 2) {
    float power = pow(-1.0f, (i - 1) / 2.0f);
    correction += power * pow(angle, i) / factorialTable[i];
  }

  // Return the final sine value with correction
  return value + correction;
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
