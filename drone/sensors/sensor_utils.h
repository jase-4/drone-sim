#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <random>
#include <glm/glm.hpp>

class SensorUtils {
public:
    // Static initialization of the random number generator and distribution
    static std::random_device rd;
    static std::mt19937 gen;
    
    // Function to generate a random glm::vec3 noise (for 3D vector noise)
    static glm::vec3 generateRandomNoise(float noiseLevel) {
        std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
        return glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    // Function to generate a random float noise (for single-value noise)
    static float generateRandomNoiseFloat(float noiseLevel) {
        std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
        return dist(gen);
    }
};

// Initialize static members
std::random_device SensorUtils::rd;
std::mt19937 SensorUtils::gen(SensorUtils::rd());

#endif // SENSOR_UTILS_H
