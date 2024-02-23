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

// Quadratic interpolation with Taylor series correction
float sine(float angle) {
  // Normalize angle to range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate integer and fractional parts of the angle
  int index = int(angle * (TABLE_SIZE - 1) / (2 * PI));
  float frac = angle - ((float)index * 2 * PI) / (TABLE_SIZE - 1);

  // Quadratic interpolation using Lagrange polynomials
  float f0 = sineTable[index];
  float f1 = sineTable[index + 1];
  float f2 = (index == TABLE_SIZE - 1) ? sineTable[0] : sineTable[index + 2]; // Wrap around for last point
  float value = f0 * (1 - frac) * (1 - frac) + 2 * frac * (1 - frac) * f1 + frac * frac * f2;

  // Taylor series correction for small angles (optional)
  if (abs(angle) < 0.5) {
    value += angle * angle * angle / 6.0; // Add cubic term
  } else if (abs(angle - PI) < 0.5) {
    // Correction for angles near PI (optional)
    angle = PI - angle;
    value = PI - value - angle * angle * angle / 6.0;
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
