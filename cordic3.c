// CORDIC approximation of sine function
// angle: input angle in radians, between 0 and 2*PI
// depth: number of iterations, between 1 and 15
// returns: approximate value of sin(angle)

float cordic_sin(float angle, int depth) {
  // CORDIC gain factor
  const float K = 0.607252935;
  // CORDIC table of elementary angles (radians)
  const float A[] = {0.785398163, 0.463647609, 0.244978663, 0.124354995, 0.062418810, 0.031239833, 0.015623729, 0.007812341, 0.003906230, 0.001953123, 0.000976562, 0.000488281, 0.000244141, 0.000122070, 0.000061035};
  // CORDIC table of inverse powers of two
  const float P[] = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125, 0.0009765625, 0.00048828125, 0.000244140625, 0.0001220703125, 0.00006103515625};
  // initialize x and y coordinates
  float x = K;
  float y = 0.0;
  // loop through the iterations
  for (int i = 0; i < depth; i++) {
    // determine the direction of rotation
    int d = angle < 0 ? -1 : 1;
    // update the angle
    angle = angle - d * A[i];
    // update the coordinates using CORDIC rotation formulas
    float x_new = x - d * y * P[i];
    float y_new = y + d * x * P[i];
    x = x_new;
    y = y_new;
  }
  // return the y coordinate as the sine value
  return y;
}
