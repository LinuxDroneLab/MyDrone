/*
 * MyIMUAgent.h
 *
 *  Created on: 23 dic 2015
 *      Author: andrea
 */

#ifndef AGENTS_MYIMUAGENT_H_
#define AGENTS_MYIMUAGENT_H_

#include <imu/MPU6050.h>
#include <baro/BMP085.h>
#include <agents/MyAgent.h>

class MyIMUAgent: public MyAgent {
public:
	MyIMUAgent(boost::shared_ptr<MyEventBus> bus,  vector<MyEvent::EventType> acceptedEventTypes);
	virtual ~MyIMUAgent();
protected:
	virtual void processEvent(boost::shared_ptr<MyEvent> event);

private:
	bool initialized;
	void initialize();
	MPU6050 imu;
	BMP085 bmp;
	long prevMicros;
};

#endif /* AGENTS_MYIMUAGENT_H_ */
