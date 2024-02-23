// Dreamed by Gemini

#define PI 3.14159265359

// Lookup table size (adjust as needed)
const int TABLE_SIZE = 256;

// Pre-computed sine values (0 to 2*PI)
float sinTable[TABLE_SIZE];

// Pre-computed factorials (0 to 5)
int factorials[6] = {1, 1, 2, 6, 24, 120};

void setup() {
  // Fill sine table with values from 0 to 2*PI
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * (2 * PI) / (TABLE_SIZE - 1);
    sinTable[i] = sin(angle);
  }

  Serial.begin(9600);
}

float sine(float angle) {
  // Normalize angle to range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate integer and fractional parts of the angle
  int intAngle = (int)angle;
  float fracAngle = angle - intAngle;

  // Use linear interpolation for fractional part from sine table
  int index1 = map(intAngle, 0, 2 * PI, 0, TABLE_SIZE - 1);
  int index2 = (index1 + 1) % TABLE_SIZE;
  float sin1 = sinTable[index1];
  float sin2 = sinTable[index2];
  float interpolatedSin = sin1 + (sin2 - sin1) * fracAngle;

  // Calculate Taylor series terms up to 5th degree
  float taylorSin = 0;
  for (int i = 1; i <= 5; i++) {
    float power = pow(-1, i + 1) * pow(fracAngle, i);
    taylorSin += power / factorials[i];
  }

  // Combine interpolated sine and Taylor series terms
  return interpolatedSin + taylorSin;
}

void loop() {
  float angle = radians(map(analogRead(A0), 0, 1023, 0, 360));
  float sineValue = sine(angle);

  Serial.print("Angle: ");
  Serial.print(degrees(angle));
  Serial.print(", Sine: ");
  Serial.println(sineValue);

  delay(100);
}
