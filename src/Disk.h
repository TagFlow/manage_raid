/*
 * Disk.h
 *
 *  Created on: 21 juin 2016
 *      Author: tagflow
 */

#ifndef DISK_H_
#define DISK_H_

#include <string>
#include <iostream>

class Disk {

	// methods
	public:
//	Disk();
	Disk(std::string name, std::string state, int rebuild);
	void rebuild();
	void erase();
	void smartTest();
	void rebuildState();

	// attributes
	private:
	std::string 	m_name;
	std::string 	m_state;
	int 			m_rebuild;

};





#endif /* DISK_H_ */
