/*
 * Raid.cpp
 *
 *  Created on: 21 juin 2016
 *      Author: tagflow
 */

#include "Raid.h"

using namespace std;

Raid::Raid() : _name("md0"), _state("clean"), _rebuild(-1){}

Raid::Raid(string name, string state, string mountPoint, int rebuild) : _name(name), _state(state), _mountPoint(mountPoint), _rebuild(rebuild){}

void Raid::diskManipulation(const string disk, const string mode){

	vector<string> arg;
	string command, output, error;
	int exitStatus;

	if(mode == "add"){
		command = "mdadm";
		arg.push_back("/dev/" + _name + " --add /dev/" + disk); // param 1
	}
	else if(mode == "remove"){
		command = "mdadm";
		arg.push_back("/dev/" + _name + " --remove /dev/" + disk); // param 1
	}

	execCmd(command, arg, output, error, exitStatus);

	if(exitStatus != 0){
		cout << "Error : " << error << endl;
	}

}

void Raid::diskDetection(vector<string> disk){


}
void Raid::smartTest(string disk, string state){

	vector<string> arg;
	string command, output, error;
	int exitStatus;

	state = "";

	command = "smartctl";
	arg.push_back("-t"); 			// param 1
	arg.push_back("short"); 		// param 2
	arg.push_back("/dev/" + disk); 	// param 3

	execCmd(command, arg, output, error, exitStatus);
	if(exitStatus != 0){
		cout << "Error : " << error << endl;
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
	arg.push_back("/dev/" + disk);		// param 6

	execCmd(command, arg, output, error, exitStatus);
	if(exitStatus != 0){
		state = error;
		cout <<"erreur"<<endl;
	}
}

void Raid::rebuildState(double &recovery, double &finish, double &speed){
// recovery : is the progress [%]
// finish	: is the time that remain [min]
// speed	: is the speed of the rebuild [KByte/sec]

	ifstream file;
	string message;
	//file.open ("/proc/mdstat");
	file.open("/home/tagflow/test");
	string line;
	char find = 0;
	size_t i1, j1, k1;
	size_t i2, j2, k2;
	if(file){
		while(!file.eof() && find<2){
			getline(file,line);
			if(line.find(_name) != string::npos) find = 1;
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
	else cout << "Error : failed to open file /proc/mdstat" << endl;

	file.close();

}

void Raid::statMem(int &Aspace, int &Tspace){

	struct statvfs stat;
	unsigned int gb = pow(1024,3);

	if(statvfs(_mountPoint.c_str(), &stat) != 0){
		cout << "Error : failed to get information about " << _name << endl;
	}

	Tspace = stat.f_blocks*stat.f_frsize/gb;
	Aspace = stat.f_bfree*stat.f_frsize/gb;

}

int Raid::execCmd(const string cmd, vector<string> arg, string &output, string &error, int &exitStatus){
	pid_t pid1;
	vector<char*> argSend;
	int descriptorSTDOut[2];
	int descriptorSTDErr[2];
	int status;
	int doCounter = 0;
	char messageOut[MESSAGE_SIZE] = {""};
	char messageErr[MESSAGE_SIZE] = {""};
	unsigned int i = 0;

	pipe(descriptorSTDOut);
	pipe(descriptorSTDErr);

	argSend.push_back((char*)cmd.c_str());
	for(i=0;i<arg.size();i++){
		argSend.push_back((char*)arg.at(i).c_str());
	}
	argSend.push_back((char *)NULL);

	do{
			pid1 = fork();
			doCounter++;
		}while((pid1 == -1) && (errno == EAGAIN) && (doCounter <= 10));

		if(pid1 == -1){
			perror("fork");
			return EXIT_FAILURE;
		}

	if(pid1 == 0){	// son process

			close(descriptorSTDOut[0]);
			close(descriptorSTDErr[0]);

			dup2(descriptorSTDOut[1],STDOUT_FILENO);
			dup2(descriptorSTDErr[1],STDERR_FILENO);

			if(execvp(cmd.c_str(), &argSend[0]) != 0){
				perror("execvp");
				return EXIT_FAILURE;
			}

	}
	else{			// father process

		close(descriptorSTDOut[1]);
		close(descriptorSTDErr[1]);

		if(wait(&status) == -1){ // wait end of son's process
			perror("wait");
			return EXIT_FAILURE;
		}
		if(WIFSIGNALED(status)){
			cout << "error : son's process fails" << endl;
			return EXIT_FAILURE;
		}

		read(descriptorSTDOut[0], messageOut, sizeof(messageOut));
		output = messageOut;
		read(descriptorSTDErr[0], messageErr, sizeof(messageErr));
		error = messageErr;
		exitStatus = WEXITSTATUS(status);

	}

	return EXIT_SUCCESS;
}




