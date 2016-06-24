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

void Raid::diskAdd(string disk){


}

void Raid::diskDelete(string disk){
	string command;
	vector<string> arg;
	string output;

	command = "mdadm";
	arg.push_back("/dev/" + _name + " --remove /dev/" + disk); // param 1
	execCmd(command, arg, output);

	cout << "output :" << endl << output << endl;

}

void Raid::smartTest(){


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

int Raid::execCmd(const string cmd, vector<string> arg, string &output){
	pid_t pid;
	vector<char*> argSend;
	int descriptor[2];
	int status;
	int doCounter = 0;
	char message[MESSAGE_SIZE];
	unsigned int i = 0;

	pipe(descriptor);

	do{
		pid = fork();
		doCounter++;
	}while((pid == -1) && (errno == EAGAIN) && (doCounter <= 10));

	if(pid == -1){
		perror("fork : ");
		return EXIT_FAILURE;
	}

	if(pid == 0){	// son process

		close(descriptor[0]);

		dup2(descriptor[1],STDOUT_FILENO); // link stdout to descriptor
		dup2(descriptor[1],STDERR_FILENO); // link stderr to descriptor

		argSend.push_back((char*)cmd.c_str());
		for(i=0;i<arg.size();i++){
			argSend.push_back((char*)arg.at(i).c_str());
		}
		argSend.push_back((char *)NULL);

		execvp(cmd.c_str(), &argSend[0]);

	}
	else{			// father process

		close(descriptor[1]);
		if(wait(&status) == -1){ // wait end of son's process
			perror("wait : ");
			return EXIT_FAILURE;
		}

		if(WIFSIGNALED(status)){
			cout << "error : son's process fails" << endl;
			return EXIT_FAILURE;
		}

		read(descriptor[0], message, sizeof(message));
		output = message;

	}

	return EXIT_SUCCESS;
}




