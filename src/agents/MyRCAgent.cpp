/*
 * MyRCAgent.cpp
 *
 *  Created on: 23 lug 2016
 *      Author: andrea
 */

#include <agents/MyRCAgent.h>
#include <events/MyRCSample.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/log/trivial.hpp>
#include <boost/math/quaternion.hpp>
#include <pthread.h>



MyRCAgent::MyRCAgent(boost::shared_ptr<MyEventBus> bus,  vector<MyEvent::EventType> acceptedEventTypes)  : MyAgent(bus, acceptedEventTypes), initialized(false), readerThread(NULL) {
	this->thrust=0;
	this->roll=0;
	this->pitch=0;
	this->yaw=0;
	this->aux1=0;
	this->aux2=0;
}

MyRCAgent::~MyRCAgent() {
}

void MyRCAgent::initialize() {
	if(!initialized) {
		if(this->rcReader.initialize((void*)this)) {
			// TODO: refactoring for circular reference with rcReader ...
			readerThread = new boost::thread(boost::ref(rcReader));
			initialized = true;
		}
	}
}

void MyRCAgent::setRCSample(float thrust, float roll, float pitch, float yaw, float aux1, float aux2) {
	this->thrust=thrust;
	this->roll=roll;
	this->pitch=pitch;
	this->yaw=yaw;
	this->aux1=aux1;
	this->aux2=aux2;
}

void MyRCAgent::processEvent(boost::shared_ptr<MyEvent> event) {
	if(!initialized) {
		initialize();
	}
	if(this->getState() == MyAgentState::Active) {
		if(event->getType() == MyEvent::EventType::Tick) {
			boost::shared_ptr<MyEvent> evOut(boost::make_shared<MyRCSample>(this->getUuid(), thrust, roll, pitch, yaw, aux1, aux2));
			m_signal(evOut);
			// TODO:
			//  1) check if m_signal is threadsafe ...
			//  2) interpret signals from RC as commands (ARM/DISARM etc ...) and emit correspondent events
		}
	}
}
