/*
 * Raid.cpp
 *
 *  Created on: 21 juin 2016
 *      Author: tagflow
 */

#include "Raid.h"

using namespace std;

Raid::Raid() {}
Raid::Raid(string name, string mountPoint, string path) : _name(name), _mountPoint(mountPoint), _path(path) {}

int Raid::diskManipulation(const string disk, const string mode){

	vector<string> arg;
	string command, output, error;
	int exitStatus;

	if(mode == "add"){
		command = "mdadm";
		arg.push_back(_name); 		// param 1
		arg.push_back("--add");		// param 2
		arg.push_back(disk);		// param 3
	}
	else if(mode == "remove"){
		command = "mdadm";
		arg.push_back(_name); 		// param 1
		arg.push_back("--remove");	// param 2
		arg.push_back(disk);		// param 3
	}
	else if(mode == "format"){
		command = "bash";
		arg.push_back(_path + "format.bash");
		arg.push_back(disk);
	}

	sleep(1);	// need to wait that disk is really in fail : mdadm have some lag
	try{
		_cmd.exec(command, arg, output, error, exitStatus);
	}
	catch(exception &e){
		throw;
	}
	return EXIT_SUCCESS;
}

int Raid::diskDetection(string disk){

	vector<string> arg;
	string command, output, error;
	int exitStatus;

	command = "blkid";
	arg.push_back(disk);

	try{
		_cmd.exec(command, arg, output, error, exitStatus);
	}
	catch(exception &e){
		throw;
	}
	return EXIT_SUCCESS;

}
int Raid::smartTest(string disk, string state){

	vector<string> arg;
	string command, output, error;
	int exitStatus;

	state = "";

	command = "smartctl";
	arg.push_back("-t"); 			// param 1
	arg.push_back("short"); 		// param 2
	arg.push_back(disk); 			// param 3

	try{
		_cmd.exec(command, arg, output, error, exitStatus);
	}
	catch(exception &e){
		throw;
	}

	cout << "Please wait 2 minutes for test to complete." << endl;

	sleep(120);

	arg.erase(arg.begin(), arg.end());
	command = "smartctl";
	arg.push_back("-q"); 				// param 1
	arg.push_back("errorsonly");		// param 2
	arg.push_back("-H");				// param 3
	arg.push_back("-l");				// param 4
	arg.push_back("selftest");			// param 5
	arg.push_back(disk);				// param 6

	try{
		_cmd.exec(command, arg, output, error, exitStatus);
	}
	catch(exception &e){
		throw;
	}
	return EXIT_SUCCESS;
}

int Raid::rebuildState(double &recovery, double &finish, double &speed){
// recovery : is the progress [%]
// finish	: is the time that remain [min]
// speed	: is the speed of the rebuild [KByte/sec]

	ifstream file;
	string message;
	file.open ("/proc/mdstat");
	string line;
	string shortName = _name;
	char find = 0;
	recovery = -1.0;
	finish = -1.0;
	speed = -1.0;
	size_t i1, j1, k1;
	size_t i2, j2, k2;
	try{
		if(file){
			while(!file.eof() && find<2){
				getline(file,line);
				shortName.erase(0, shortName.find_last_of("/")+1);
				if(line.find(shortName) != string::npos) find = 1;
				if(find){
					i1=line.find("recovery");
					j1=line.find("finish");
					k1=line.find("speed");
					if(i1 != string::npos && j1 != string::npos && k1 != string::npos){
						i2=line.find("%");
						j2=line.find("min");
						k2=line.find("K/sec");
						recovery = strtod(line.substr(i1+11, i2-(i1+11)).c_str(), NULL);
						finish = strtod(line.substr(j1+7, j2-(i1+7)).c_str(), NULL);
						speed = strtod(line.substr(k1+6, k2-(i1+6)).c_str(), NULL);
						find = 2;
					}
				}
			}
		}
	}
	catch(exception &e){
		throw;
	}

	file.close();
	return EXIT_SUCCESS;

}

int Raid::statMem(int &Aspace, int &Tspace){

	struct statvfs stat;
	unsigned int gb = pow(1024,3);

	try{
		if(statvfs(_mountPoint.c_str(), &stat) != 0){
			throw runtime_error("failed to get information about "+_name);
		}
	}
	catch(exception &e){
		throw;
	}

	Tspace = stat.f_blocks*stat.f_frsize/gb;
	Aspace = stat.f_bfree*stat.f_frsize/gb;
	return EXIT_SUCCESS;

}





