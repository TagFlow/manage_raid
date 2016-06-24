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
#include <cmath>
#include <sys/statvfs.h>

class Disk {

	// methods
	public:
	Disk();
	Disk(std::string name, std::string state, std::string mountPoint, int rebuild);
	void rebuild();
	void erase();
	void smartTest();
	void rebuildState();
	void statMem(int &Aspace, int &Tspace);

	// attributes
	private:
	std::string 	m_name;
	std::string 	m_state;
	std::string		m_mountPoint;
	int 			m_rebuild;

};





#endif /* DISK_H_ */
