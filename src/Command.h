/*
 * command.h
 *
 *  Created on: 4 juil. 2016
 *      Author: maxime
 */

#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_

#include <string>
#include <errno.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <exception>

#define MESSAGE_SIZE 1024

class Command {
	public:
	Command();
	static int exec(const std::string cmd, std::vector<std::string> arg, std::string &output, std::string &error, int &exitStatus);
};

#endif /* SRC_COMMAND_H_ */
