//dreamed by Gemini

#define MAX_FACTORIAL 9 // Change this value to adjust the maximum factorial stored

float factorialTable[MAX_FACTORIAL + 1];

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Fill the factorial lookup table
  factorialTable[0] = 1.0f;
  for (int i = 1; i <= MAX_FACTORIAL; i++) {
    factorialTable[i] = factorialTable[i-1] * i;
  }
}

float factorial(int n) {
  if (n < 0) {
    // Handle negative input (e.g., return error)
    return -1.0f;
  } else if (n <= MAX_FACTORIAL) {
    return factorialTable[n];
  } else {
    // Handle values beyond the table (e.g., calculate using loop)
    float result = 1.0f;
    for (int i = 2; i <= n; i++) {
      result *= i;
    }
    return result;
  }
}
