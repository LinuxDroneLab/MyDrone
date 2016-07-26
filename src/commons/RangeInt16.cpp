/*
 * RangeInt.cpp
 *
 *  Created on: 23 lug 2016
 *      Author: andrea
 */

#include <commons/RangeInt16.h>
#include <cmath>

RangeInt16::RangeInt16(int16_t min, int16_t max) : min(min), max(max) {

}

RangeInt16::~RangeInt16() {
	// TODO Auto-generated destructor stub
}
float RangeInt16::getCenter() {
    return ((float)max + float(min))/2.0f;
}
int16_t RangeInt16::getLength() {
	return max - min;
}

int16_t RangeInt16::constraint(int16_t v) {
	return v < min ? min : (v > max ? max : v);
}
int16_t RangeInt16::convert(int16_t v, RangeInt16& range) {
	float vf = float(range.constraint(v));
	return int16_t(std::round((vf - range.getCenter())/(float)range.getLength() * this->getLength() + this->getCenter()));
}
bool RangeInt16::contains(int16_t v) {
	return v >= min && v <= max;
}
