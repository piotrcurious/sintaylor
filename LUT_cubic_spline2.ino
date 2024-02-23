//Dreamed by Gemini

#define TABLE_SIZE 256 // Size of lookup table (adjust as needed)
#define PI 3.14159265359

// Pre-computed sine values for lookup table
float sineTable[TABLE_SIZE];

void setup() {
  Serial.begin(9600);

  // Fill lookup table with sine values (0 to 2*PI)
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * 2 * PI / (TABLE_SIZE - 1);
    sineTable[i] = sin(angle);
  }
}

float sine(float angle) {
  // Normalize angle to range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate integer and fractional parts of the angle
  int index = int(angle * (TABLE_SIZE - 1) / (2 * PI));
  float frac = angle - ((float)index * 2 * PI) / (TABLE_SIZE - 1);

  // Cubic spline interpolation
  float a = sineTable[index];
  float b = sineTable[index + 1];
  float c = (index < TABLE_SIZE - 2) ? sineTable[index + 2] : a; // Handle edge case
  float d = (index > 1) ? sineTable[index - 1] : b; // Handle edge case
  float h1 = frac;
  float h2 = 1 - frac;
  float h1c = 3 * h1 * h1 - 2 * h1 * h1 * h1;
  float h2c = 3 * h2 * h2 - 2 * h2 * h2 * h2;
  float value = a * h2c + b * h1c + c * h1 * h2 * h2 - d * h1 * h1 * h2;

  // Taylor series correction for small angles (optional)
  if (abs(angle) < 0.5) {
    value += angle * angle * angle / 6.0; // Add cubic term
    value -= angle * angle * angle * angle * angle / 120.0; // Add quintic term for even better accuracy
  }

  return value;
}

void loop() {
  float angle = radians(map(analogRead(A0), 0, 1023, 0, 360)); // Read angle from analog input
  float sineValue = sine(angle);

  Serial.print("Angle: ");
  Serial.print(degrees(angle));
  Serial.print(" degrees, Sine: ");
  Serial.println(sineValue);

  delay(100);
}
