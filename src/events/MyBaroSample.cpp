/*
 * MyBaroSample.cpp
 *
 *  Created on: 17 dic 2015
 *      Author: andrea
 */

#include <commons/MyPriority.h>
#include <events/MyBaroSample.h>

MyBaroSample::MyBaroSample(boost::uuids::uuid origin, float altitude, float estimatedAltitude,
		int32_t pressure, int32_t seeLevelPressure, float temperature, uint32_t rawPressure, uint16_t rawTemperature, uint16_t dtime) :
		MyEvent(origin), altitude(altitude), estimatedAltitude(estimatedAltitude), pressure(pressure), seeLevelPressure(
				seeLevelPressure), temperature(temperature), rawPressure(rawPressure), rawTemperature(rawTemperature), dTimeMillis(dtime) {
	this->setPriority(MyPriority::BAROMETER_SAMPLE_PRIORITY);
}

MyBaroSample::~MyBaroSample() {
	// TODO Auto-generated destructor stub
}

MyEvent::EventType MyBaroSample::getType() const {
	return MyEvent::EventType::BaroSample;
}

float MyBaroSample::getAltitude() const {
	return this->altitude;
}

float MyBaroSample::getEstimatedAltitude() const {
    return this->estimatedAltitude;
}

float MyBaroSample::getTemperature() const {
	return this->temperature;
}
int32_t MyBaroSample::getPressure() const {
	return this->pressure;
}
int32_t MyBaroSample::getSeeLevelPressure() const {
	return this->seeLevelPressure;
}
uint32_t MyBaroSample::getRawPressure() const {
    return this->rawPressure;
}
uint16_t MyBaroSample::getDTimeMillis() const {
    return this->dTimeMillis;
}
uint16_t MyBaroSample::getRawTemperature() const {
    return this->rawTemperature;
}
