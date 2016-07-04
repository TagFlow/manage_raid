/*
 * command.cpp
 *
 *  Created on: 4 juil. 2016
 *      Author: maxime
 */

#include "Command.h"

using namespace std;

Command::Command() {}

int Command::exec(const string cmd, vector<string> arg, string &output, string &error, int &exitStatus){
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
				exit(EXIT_FAILURE);
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
