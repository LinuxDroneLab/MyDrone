/*
 * MyShutdownCmd.h
 *
 *  Created on: 17 dic 2015
 *      Author: andrea
 */

#ifndef EVENTS_MYSHUTDOWNCMD_H_
#define EVENTS_MYSHUTDOWNCMD_H_

#include <boost/uuid/uuid.hpp>
#include <events/MyCmd.h>

class MyShutdownCmd: public MyCmd {
public:
	explicit MyShutdownCmd(boost::uuids::uuid origin, boost::uuids::uuid destination);
	virtual ~MyShutdownCmd();
	virtual MyEvent::EventType getType() const;
};

#endif /* EVENTS_MYSHUTDOWNCMD_H_ */
