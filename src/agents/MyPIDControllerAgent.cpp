/*
 * MyPIDControllerAgent.cpp
 *
 *  Created on: 26 dic 2015
 *      Author: andrea
 */
#include <cmath>

#include <agents/MyPIDControllerAgent.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/log/trivial.hpp>
#include <events/MyIMUSample.h>
#include <events/MyBaroSample.h>
#include <events/MyOutMotors.h>
#include <events/MyYPRError.h>
#include <events/MyRCSample.h>
#include <iostream>

#define ROLL_POS 0
#define PITCH_POS 1
#define YAW_POS 2
#define THRUST_POS 3


RangeInt16 MyPIDControllerAgent::TARGET_RANGES[] = {RangeInt16(-62, 62), // roll
		                                            RangeInt16(-62, 62), // pitch
		                                            RangeInt16(-125, 125), // yaw
		                                            RangeInt16(1000, 2000) // thrust
};
ValueInt16 MyPIDControllerAgent::TARGET_VALUES[] = {ValueInt16(0, MyPIDControllerAgent::TARGET_RANGES[ROLL_POS]),
													ValueInt16(0, MyPIDControllerAgent::TARGET_RANGES[PITCH_POS]),
													ValueInt16(0, MyPIDControllerAgent::TARGET_RANGES[YAW_POS]),
													ValueInt16(0, MyPIDControllerAgent::TARGET_RANGES[THRUST_POS]),
};

using namespace std;

MyPIDControllerAgent::MyPIDControllerAgent(boost::shared_ptr<MyEventBus> bus,
		vector<MyEvent::EventType> acceptedEventTypes) :
		MyAgent(bus, acceptedEventTypes), initialized(false), yawCurr(0), pitchCurr(0), rollCurr(0), yawErr(0.01f, 20, 10, 4), pitchErr(
				0.01f, 10, 10, 4), rollErr(0.01f, 10, 10, 4) {
	keRoll = 1.2f;
	keIRoll = 0.5f;
	keDRoll = 0.3f;

	kePitch = 1.2f;
	keIPitch = 0.5f;
	keDPitch = 0.3f;

	keYaw = 0.0f;
	keIYaw = 0.00f;
	keDYaw = 0.000f;

}

MyPIDControllerAgent::~MyPIDControllerAgent() {
}
void MyPIDControllerAgent::initialize() {
	initialized = true;
}

void MyPIDControllerAgent::calcErr(boost::math::quaternion<float> q) {
	float real = q.real();
	float x = q.R_component_2();
	float y = q.R_component_3();
	float z = q.R_component_4();


	yawCurr = std::rint((atan2(2.0f * (real * z + x * y), 1.0f - 2.0f * (y * y + z * z))
			* 57.295779513f));
	pitchCurr = std::rint((asin(2.0 * real * y - 2.0 * z * x) * 57.295779513));
	rollCurr = std::rint((atan2(2.0 * (real * x + y * z), 1.0 - 2.0 * (x * x + y * y))
			* 57.295779513));

	yawErr.push(yawCurr - MyPIDControllerAgent::TARGET_VALUES[YAW_POS].getValue());
	pitchErr.push(pitchCurr - MyPIDControllerAgent::TARGET_VALUES[PITCH_POS].getValue());
	rollErr.push(rollCurr - MyPIDControllerAgent::TARGET_VALUES[ROLL_POS].getValue());
}
void MyPIDControllerAgent::calcCorrection() {
	float eRoll = rollErr.getMean();
	float eIRoll = rollErr.getIntegral();
	float eDRoll = rollErr.getDerivate();

	float ePitch = pitchErr.getMean();
	float eIPitch = pitchErr.getIntegral();
	float eDPitch = pitchErr.getDerivate();

	float eYaw = yawErr.getMean();
	float eIYaw = yawErr.getIntegral();
	float eDYaw = yawErr.getDerivate();

	float corrRoll = keRoll * eRoll + keIRoll*eIRoll + keDRoll * eDRoll;
	float corrPitch = kePitch * ePitch + keIPitch*eIPitch + keDPitch * eDPitch;
	float corrYaw = keYaw * eYaw + keIYaw*eIYaw + keDYaw * eDYaw;

	int32_t front = std::rint((float(MyPIDControllerAgent::TARGET_VALUES[THRUST_POS].getValue()) + corrPitch - corrYaw)*1000.0f);
	int32_t rear = std::rint((float(MyPIDControllerAgent::TARGET_VALUES[THRUST_POS].getValue()) - corrPitch - corrYaw)*1000.0f);
	int32_t left = std::rint((float(MyPIDControllerAgent::TARGET_VALUES[THRUST_POS].getValue()) - corrRoll + corrYaw)*1000.0f);
	int32_t right = std::rint((float(MyPIDControllerAgent::TARGET_VALUES[THRUST_POS].getValue()) + corrRoll + corrYaw)*1000.0f);

//	printf("corrRoll:%5.5f, corrPitch:%5.5f, corrYaw:%5.5f, keIR/P=%2.2f, keDR/P=%2.2f, eRoll=%2.2f, ePitch=%2.2f, eIRoll=%2.2f, eIPitch=%2.2f, eDRoll=%2.2f, eDPitch=%2.2f \n",
//			corrRoll, corrPitch, corrYaw, keIRoll, keDRoll, eRoll, ePitch, eIRoll, eIPitch, eDRoll, eDPitch);

	// level motors on [1000000, 2000000]
	int32_t minMotors = 9999999;
	if(front < minMotors) {
		minMotors = front;
	}
	if(rear < minMotors) {
		minMotors = rear;
	}
	if(left < minMotors) {
		minMotors = left;
	}
	if(right < minMotors) {
		minMotors = right;
	}
	int32_t diff = 0;
	if(minMotors < 1000000) {
		diff = 1000000 - minMotors;
	}
	front = std::min(2000000, front + diff);
	rear = std::min(2000000, rear + diff);
	left = std::min(2000000, left + diff);
	right = std::min(2000000, right + diff);

	int32_t maxMotors = 0;
	if(front > maxMotors) {
		maxMotors = front;
	}
	if(rear > maxMotors) {
		maxMotors = rear;
	}
	if(left > maxMotors) {
		maxMotors = left;
	}
	if(right > maxMotors) {
		maxMotors = right;
	}
	diff = 0;
	if(maxMotors > 2000000) {
		diff = maxMotors - 2000000;
	}
	front = std::max(1000000, front - diff);
	rear = std::max(1000000, rear - diff);
	left = std::max(1000000, left - diff);
	right = std::max(1000000, right - diff);

	{ // out error event
		boost::shared_ptr<MyYPRError> evOut(boost::make_shared<MyYPRError>(this->getUuid(), yawCurr, pitchCurr, rollCurr, MyPIDControllerAgent::TARGET_VALUES[YAW_POS].getValue(), MyPIDControllerAgent::TARGET_VALUES[PITCH_POS].getValue(), MyPIDControllerAgent::TARGET_VALUES[ROLL_POS].getValue(), eRoll, eIRoll, eDRoll,
				ePitch, eIPitch, eDPitch,
				eYaw, eIYaw, eDYaw));
		m_signal(evOut);
	}
	{// out state motors
		boost::shared_ptr<MyOutMotors> evOut(boost::make_shared<MyOutMotors>(this->getUuid(), front, rear, left, right));
		m_signal(evOut);
	}
}
void MyPIDControllerAgent::processEvent(boost::shared_ptr<MyEvent> event) {
	if (!initialized) {
		initialize();
	}
	if (this->getState() == MyAgentState::Active) {
		if (event->getType() == MyEvent::EventType::IMUSample) {
			boost::shared_ptr<MyIMUSample> imuSample =
					boost::static_pointer_cast<MyIMUSample>(event);

			// FIXME: solo per test. Uso come target il primo sample
			boost::math::quaternion<float> q = imuSample->getQuaternion();
			calcErr(q);
			calcCorrection();
		} else if(event->getType() == MyEvent::EventType::RCSample) {
			boost::shared_ptr<MyRCSample> rcSample =
					boost::static_pointer_cast<MyRCSample>(event);
			keIRoll = keIPitch = (1.0f + (*rcSample).getAux1Percent())/2.0f;
			keDRoll = keDPitch = (1.0f + (*rcSample).getAux2Percent())/2.0f;

			MyPIDControllerAgent::TARGET_VALUES[ROLL_POS].setPercentValue((*rcSample).getRollPercent());
			MyPIDControllerAgent::TARGET_VALUES[PITCH_POS].setPercentValue((*rcSample).getPitchPercent());
			MyPIDControllerAgent::TARGET_VALUES[YAW_POS].setPercentValue((*rcSample).getYawPercent());
			MyPIDControllerAgent::TARGET_VALUES[THRUST_POS].setPercentValue((*rcSample).getThrustPercent());

		}
	}
}
