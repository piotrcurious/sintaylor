#define PI 3.14159265359

// Lookup table size (adjust as needed)
const int TABLE_SIZE = 256;

// Pre-computed sine values for the lookup table
float sineTable[TABLE_SIZE];

// Feature hash table size (adjust based on memory and accuracy)
const int CACHE_SIZE = 64;

// Feature mask for hashing (adjust based on CACHE_SIZE)
const int FEATURE_MASK = CACHE_SIZE - 1;

// Cache structure
struct CacheEntry {
  int indexFloor;
  float indexFraction;
  float correction;
};

CacheEntry cache[CACHE_SIZE];

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Fill the sine lookup table
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = i * (2 * PI / TABLE_SIZE);
    sineTable[i] = sin(angle);
  }
}

float sine(float angle) {
  // Normalize angle to the range [0, 2*PI)
  angle = fmod(angle, 2 * PI);

  // Calculate lookup table index with interpolation
  float index = angle * (TABLE_SIZE - 1) / (2 * PI);
  int indexFloor = (int)floor(index);
  float indexFraction = index - indexFloor;

  // Calculate feature hash
  int featureHash = ((indexFloor << 8) & FEATURE_MASK) ^ (int)(indexFraction * 255);

  // Check cache for existing correction
  if (cache[featureHash].indexFloor == indexFloor &&
      cache[featureHash].indexFraction == indexFraction) {
    return sineTable[indexFloor] * (1 - indexFraction) +
           sineTable[indexFloor + 1] * indexFraction +
           cache[featureHash].correction;
  }

  // Calculate Taylor series correction for the angle
  float correction = 0.0f;
  for (int i = 1; i < 10; i += 2) {
    float power = pow(-1.0f, (i - 1) / 2.0f);
    correction += power * pow(angle, i) / factorialTable[i];
  }

  // Update cache with new correction
  cache[featureHash].indexFloor = indexFloor;
  cache[featureHash].indexFraction = indexFraction;
  cache[featureHash].correction = correction;

  // Return the final sine value with correction
  return sineTable[indexFloor] * (1 - indexFraction) +
         sineTable[indexFloor + 1] * indexFraction +
         correction;
}

// ... rest of the loop function remains the same ...
