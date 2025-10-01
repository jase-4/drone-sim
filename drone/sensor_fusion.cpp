#include "sensor_fusion.hpp"
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


void SensorFusion::setSensors(const std::vector<std::unique_ptr<SensorBase>>& sensorReferences) {
	for (auto& sensor : sensorReferences) {
		if (auto* gps = dynamic_cast<GPSSensor*>(sensor.get())) {
			gpsPosition = gps->position;
			gpsVelocity = gps->velocity;
		}
		else if (auto* imu = dynamic_cast<IMUSensor*>(sensor.get())) {
			imuAcceleration = imu->acceleration_;
			imuOrientation = imu->orientation_;
		}
	}
}

void SensorFusion::fuse(float dt) {
	glm::vec3 imuVelocity = imuAcceleration * dt;
	glm::vec3 imuPosition = imuVelocity * dt;
	float gpsWeight = 0.8f;
	float imuWeight = 0.2f;
	fusedPosition = gpsPosition;
	fusedVelocity = gpsVelocity;
	fusedOrientation = imuOrientation;
	fusedAcceleration = imuAcceleration;
}

std::ostringstream SensorFusion::publishData() {
	std::ostringstream ss;
      ss << std::fixed << std::showpoint << std::setprecision(6); 

    
	ss << "{" << "\"type\":\"sensor_fusion\",";
	ss << "\"fused_position\":[" << fusedPosition.x << "," << fusedPosition.y << "," << fusedPosition.z << "],";
	ss << "\"fused_velocity\":[" << fusedVelocity.x << "," << fusedVelocity.y << "," << fusedVelocity.z << "],";
	ss << "\"fused_acceleration\":[" << fusedAcceleration.x << "," << fusedAcceleration.y << "," << fusedAcceleration.z << "],";
	ss << "\"fused_orientation\":[" << fusedOrientation.w << "," << fusedOrientation.x << "," << fusedOrientation.y << "," << fusedOrientation.z << "]}";
	return ss;
}
