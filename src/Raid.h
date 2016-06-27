/*
 * Raid.h
 *
 *  Created on: 21 juin 2016
 *      Author: tagflow
 */

#ifndef Raid_H_
#define Raid_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/statvfs.h>


#define MESSAGE_SIZE 1024

class Raid {

	// methods
	public:
	Raid();
	Raid(std::string name, std::string state, std::string mountPoint, int rebuild);
	void diskManipulation(const std::string disk, const std::string mode);
	void smartTest();
	void rebuildState(double &recovery, double &finish, double &speed);
	void statMem(int &Aspace, int &Tspace);
	private:
	int execCmd(const std::string cmd, std::vector<std::string> arg, std::string &output, std::string &error, int &exitStatus);

	// attributes
	private:
	std::string 				_name;
	std::string 				_state;
	std::string					_mountPoint;
	int 						_rebuild;



};





#endif /* Raid_H_ */
