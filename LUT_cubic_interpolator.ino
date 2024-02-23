//Dreamed by Gemini

#define TABLE_SIZE 128 // Adjust table size as needed
#define PI 3.14159265359

// Pre-computed sine values for cubic spline interpolation
float sineTable[TABLE_SIZE];
float sineDeriv1[TABLE_SIZE]; // First derivative values at table points
float sineDeriv2[TABLE_SIZE]; // Second derivative values at table points

void setup() {
  Serial.begin(9600);

  // Fill lookup table with sine values (0 to 2*PI) and derivatives
  float angleStep = 2 * PI / (TABLE_SIZE - 1);
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * angleStep;
    sineTable[i] = sin(angle);
    sineDeriv1[i] = cos(angle);
    sineDeriv2[i] = -sin(angle);
  }

  // Calculate second derivatives at table endpoints for spline boundary conditions
  sineDeriv2[0] = sineDeriv2[TABLE_SIZE - 1] = 0.0;
}

float sine(float angle) {
  // Normalize angle to range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate integer and fractional parts of the angle
  int index = int(angle * (TABLE_SIZE - 1) / (2 * PI));
  float frac = angle - ((float)index * 2 * PI) / (TABLE_SIZE - 1);

  // Perform cubic spline interpolation
  float a0 = sineTable[index];
  float a1 = sineDeriv1[index];
  float a2 = (3 * sineTable[index + 1] - 3 * sineTable[index] - 2 * sineDeriv1[index] - sineDeriv2[index]) / angleStep;
  float a3 = (2 * sineTable[index] - 2 * sineTable[index + 1] + sineDeriv1[index] + sineDeriv2[index]) / (angleStep * angleStep);

  float value = a0 + a1 * frac + a2 * frac * frac + a3 * frac * frac * frac;

  // Use Taylor series for small angles (optional)
  if (abs(angle) < 0.5) {
    value += angle * angle * angle / 6.0; // Add cubic term for better accuracy at small angles
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
