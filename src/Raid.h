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
#include <errno.h>
#include <sys/statvfs.h>
#include <exception>

#include "Command.h"

class Raid {

	// methods
	public:
	Raid();
	Raid(std::string name, std::string mountPoint, std::string path);
	int diskManipulation(const std::string disk, const std::string mode);
	int diskDetection(std::string disk);
	int smartTest(std::string disk, std::string state);
	int rebuildState(double &recovery, double &finish, double &speed);
	int statMem(int &Aspace, int &Tspace);


	// attributes
	private:
	std::string 				_name;
	std::string					_mountPoint;
	std::string					_path;
	Command						_cmd;
};

#endif /* Raid_H_ */
