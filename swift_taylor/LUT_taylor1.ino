//Dreamed by Gemini 

#define TABLE_SIZE 256 // Adjust table size as needed

// Pre-computed sine values for the look-up table
float sineTable[TABLE_SIZE];

// Pre-computed factorials for Taylor series optimization
float factorialTable[10]; // Assuming maximum order of 9

void setup() {
  // Fill the sine table with pre-computed values
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = radians(360.0 * i / (TABLE_SIZE - 1));
    sineTable[i] = sin(angle);
  }

  // Fill the factorial table
  factorialTable[0] = 1.0;
  for (int i = 1; i < 10; i++) {
    factorialTable[i] = factorialTable[i - 1] * i;
  }

  Serial.begin(9600);
}

float sine(float angle) {
  // Normalize angle to the range [0, 2*PI)
  angle = fmod(angle, 2 * PI);
  if (angle < 0) angle += 2 * PI;

  // Calculate table index with interpolation
  float index = angle * (TABLE_SIZE - 1) / (2 * PI);
  int lowerIndex = int(floor(index));
  float interpolationFactor = index - lowerIndex;

  // Use linear interpolation from the look-up table
  float tableValue = sineTable[lowerIndex] * (1 - interpolationFactor) + sineTable[lowerIndex + 1] * interpolationFactor;

  // Calculate Taylor series terms up to a certain order (replace 3 with desired order)
  float taylorTerm = 0.0;
  for (int i = 1; i <= 3; i++) {
    float power = pow(-1.0, i + 1);
    float term = power * pow(angle, 2 * i) / factorialTable[2 * i];
    taylorTerm += term;
  }

  // Combine look-up table value and Taylor series approximation
  return tableValue + taylorTerm;
}

void loop() {
  float inputAngle = radians(map(analogRead(A0), 0, 1023, 0, 360)); // Read angle from analog input
  float sineValue = sine(inputAngle);

  Serial.print("Angle: ");
  Serial.print(degrees(inputAngle));
  Serial.print(", Sine: ");
  Serial.println(sineValue);

  delay(100);
}
