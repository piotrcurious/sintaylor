// Define the number of iterations for the CORDIC algorithm
#define N 16

// Define the scaling factor for the CORDIC algorithm
#define K 0.607252935

// Define a table of arctangent values for the CORDIC algorithm
const float arctan[N] = {
  0.785398163, 0.463647609, 0.244978663, 0.124354995, 0.062418810, 0.031239833, 0.015623729, 0.007812341,
  0.003906230, 0.001953123, 0.000976562, 0.000488281, 0.000244141, 0.000122070, 0.000061035, 0.000030518
};

// Define a function to convert radians to degrees
float rad2deg(float rad) {
  return rad * 180 / PI;
}

// Define a function to convert degrees to radians
float deg2rad(float deg) {
  return deg * PI / 180;
}

// Define a function to compute the sine of an angle using the CORDIC algorithm
float cordic_sin(float theta) {
  // Normalize the angle to the range [-PI, PI]
  theta = fmod(theta, 2 * PI);
  if (theta > PI) {
    theta -= 2 * PI;
  }
  else if (theta < -PI) {
    theta += 2 * PI;
  }

  // Initialize the CORDIC variables
  float x = K;
  float y = 0;
  float z = theta;
  float v = 1;

  // Perform the CORDIC iterations
  for (int i = 0; i < N; i++) {
    float d = z >= 0 ? -1 : 1;
    float tx = x - d * v * y;
    float ty = y + d * v * x;
    float tz = z - d * arctan[i];
    x = tx;
    y = ty;
    z = tz;
    v = v / 2;
  }

  // Return the y coordinate as the sine value
  return y;
}

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
}

void loop() {
  // Test the cordic_sin function with some angles
  float angles[] = {0, PI / 6, PI / 4, PI / 3, PI / 2, 2 * PI / 3, 3 * PI / 4, 5 * PI / 6, PI, -PI / 6, -PI / 4, -PI / 3, -PI / 2, -2 * PI / 3, -3 * PI / 4, -5 * PI / 6, -PI};
  for (int i = 0; i < 17; i++) {
    float angle = angles[i];
    float sin_value = cordic_sin(angle);
    Serial.print("The sine of ");
    Serial.print(rad2deg(angle));
    Serial.print(" degrees is ");
    Serial.println(sin_value);
  }
  // Wait for a second before repeating
  delay(1000);
}
