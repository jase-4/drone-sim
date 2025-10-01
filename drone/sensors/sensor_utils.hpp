// sensor_utils.h
#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <random>
#include <glm/glm.hpp>

class SensorUtils {
public:
    inline static std::random_device rd;
    inline static std::mt19937 gen;

    // Add inline here
    inline static glm::vec3 generateRandomNoise(float noiseLevel) {
        std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
        return glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    inline static float generateRandomNoiseFloat(float noiseLevel) {
        std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
        return dist(gen);
    }
};

#endif // SENSOR_UTILS_H
