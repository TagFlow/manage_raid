//============================================================================
// Name        : main.cpp
// Author      : Maxime Letourneau
// Version     : 1.0
// Description : Raid manager for mdadm
//
// Language	   : C++ 2011
//============================================================================

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <boost/program_options.hpp>
#include <exception>

#include "Raid.h"
#include "spdlog/spdlog.h"
#include "configuration/configuration.h"

#include "Command.h"

int progDetected(std::string program);

namespace po =  boost::program_options;

using namespace std;


int main(int argc, char*argv[]) {
	vector<string> 	options;
	Configuration	config;
	unsigned int 	i;
	int 			fail(0), rebuildStarted(0), rebuildFinished(0), Aspace(-1), Tspace(-1);
	shared_ptr<spdlog::logger> log;	// var pointer for the log lib
	string 			raidDisk, disk, state, path;
	string			raidName, raidMount, logFile, logLevel; 	// for config file
	int				logRotOn, logSize, logMaxFile;		// for config file
					/*
					 * logRotOn define if the rotation of log is enabling or not. If =1 -> yes if =0 -> no
					 * logSize in Mo
					 * logMaxFile is the number of file that is keeping when rotation is enabling
					 */

	path = argv[0];
	path.erase(path.end() - (path.length() - path.find_last_of("/")) + 1, path.end()); // delete path usefull

	po::options_description opt{"Options"};
	opt.add_options()
			("help,h", 					"Help screen")
			("size,s", 					"Print in the log file the size of the raid")
			("reconstruction-state,r",	"Print in the log file the state of the reconstruction of disk(s)")
			("std-out,o",					"Print in the standard output the command output of size or/and reconstruction state")
			("mdadm", po::value<vector<string>>()->
			 multitoken()->zero_tokens()->composing(), "Mdadm inputs. They are 3 parameters for this mode.\n"
			 	 	 	 	 	 	 	 	 	 	   "note : the name mdadm is not an argument\n\n"
					 	 	 	 	 	 	 	 	   "$1 : string with Fail or RebuildsStarted or RebuildFinished \n"
					 	 	 	 	 	 	 	 	   "$2 : name of the raid disk (system path)\n"
			 	 	 	 	 	 	 	 	 	 	   "$3 : name of the concerned disk (system path)\n\n"
			 	 	 	 	 	 	 	 	 	 	   "This mode of the program is calling by mdadm itself.");

	po::positional_options_description popt;
	popt.add("mdadm", -1);

	po::command_line_parser parser{argc, argv};
	parser.options(opt).positional(popt).allow_unregistered();
	po::parsed_options parsed_options = parser.run();

	po::variables_map vm;
	po::store(parsed_options, vm);
	po::notify(vm);

	if(vm.count("help")){
		cout << opt << "\n";
		return EXIT_FAILURE;
	}
	else if(vm.count("size") || vm.count("reconstruction-state") || vm.count("std-out")){}
	else if(vm.count("mdadm")){
		for(i=0;i<vm["mdadm"].as< vector<string> >().size();i++){	// scan the vector
			if(vm["mdadm"].as< vector<string> >().at(i).find("Fail") != string::npos) fail = 1;
			if(vm["mdadm"].as< vector<string> >().at(i).find("RebuildStarted") != string::npos) rebuildStarted = 1;
			if(vm["mdadm"].as< vector<string> >().at(i).find("RebuildFinished") != string::npos) rebuildFinished = 1;
			if(vm["mdadm"].as< vector<string> >().at(i).find("/dev") != string::npos){
				if(i==1) raidDisk = vm["mdadm"].as< vector<string> >().at(i);
				else if(i==2) disk = vm["mdadm"].as< vector<string> >().at(i);
			}
		}
	}
	else{	// if no arg
		cout << opt << "\n";
		return EXIT_FAILURE;
	}

	// check that the program is launching by root
	/*if(getuid() != 0) {
		cerr << "Error : must be launch by root" << endl;
		return EXIT_FAILURE;
	}*/

	try{
		if(config.Load(path + "raid.conf") == false)			// load the config file
			throw runtime_error("can't read configuration file");

		if (!(config.Get("RAID_NAME", 		raidName)		&&		// read essential parameter of config file
			  config.Get("RAID_MOUNT", 		raidMount)		&&
			  config.Get("LOG_ROTATE_ON", 	logRotOn)		&&
			  config.Get("LOG_FILE", 		logFile)		&&
			  config.Get("LOG_SIZE", 		logSize)		&&
			  config.Get("LOG_MAX_FILE",	logMaxFile)		&&
			  config.Get("LOG_LEVEL",		logLevel)))

			throw runtime_error("missing parameter in configuration file");
	}
	catch(exception &e){
		log->error("{}", e.what());
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	Raid md0(raidName, raidMount, path);

	// choose log method : with or not a rotation of the log
	if(logRotOn) log = spdlog::rotating_logger_st("manage_raid", logFile, 1024 * 1024 * logSize, logMaxFile, true);
	else log = spdlog::basic_logger_st("manage_raid", logFile, true);

	// log level
	// set default log level
	spdlog::set_level(spdlog::level::info);
	// set log level read in config file
	for(i=0; i<=spdlog::level::off; i++){
		if(logLevel == spdlog::level::to_str((spdlog::level::level_enum) i)) spdlog::set_level((spdlog::level::level_enum)i);
	}

	// verification that all program use is installed
	try{
		progDetected("mdadm");
	}
	catch(exception &e){
		log->error("{}", e.what());
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	try{
		progDetected("smartctl");
	}
	catch(exception &e){
		log->error("{}", e.what());
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}


	 // start managing
	if(vm.count("size")){
		log->info("ask disk raid space");
		try{
			md0.statMem(Aspace, Tspace);
		}
		catch(exception &e){
			log->error("{}", e.what());
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}

		log->info("disk raid space : available space = {} GB       total space = {} GB", Aspace, Tspace);

		if(vm.count("std-out")){
			cout << "Space state :" << endl;
			cout << "available space : " << Aspace << "GB" << endl;
			cout << "total space     : " << Tspace << "GB" << endl;
		}
	}

	if(vm.count("reconstruction-state")){
		log->info("ask reconstruction state");
		double recovery, finish, speed;
		try{
			md0.rebuildState(recovery, finish, speed);
		}
		catch(exception &e){
			log->error("{}", e.what());
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}
		log->info("reconstruction state : rebuild state = {} %       time remaining = {} min       speed = {} Mo/s", recovery, finish, speed);

		if(vm.count("std-out")){
			cout << "Reconstruction state :" << endl;
			cout << "rebuild state = " << recovery << " %" << endl;
			cout << "time remaining = " << finish << " min" << endl;
			cout << "speed = " << speed << " Mo/s" << endl;
		}
	}


	if(fail){
		log->alert("disk {} fail", disk);
		try{
			log->alert("starting removing of the disk in the raid array");
			md0.diskManipulation(disk, "remove"); //log->alert("removing disk fail");
			log->alert("removing disk done");

			log->alert("starting the smart test. Please wait 2 min");
			md0.smartTest(disk, state); //log->alert("smart test fail");
			log->alert("smart test done");

			if(state == ""){	// not defect disk
				log->alert("disk {} is NOT defect", disk);
				log->alert("starting formatting disk");
				md0.diskManipulation(disk, "format");// log->alert("format disk fail");
				log->alert("format disk done");

				log->alert("starting adding disk in the raid array");
				md0.diskManipulation(disk, "add");// log->alert("adding disk fail");
				log->alert("adding disk done");
			}
			else{				// defect disk
				log->emerg("disk {} is defect. Please change the disk", disk);

				do{				// wait that the disk is change
					 sleep(30);
				}while(md0.diskDetection(disk));
				log->emerg("new disk detected");

				log->emerg("starting formatting disk");
				md0.diskManipulation(disk, "format");// log->emerg("format disk fail");
				log->emerg("format disk done");

				log->emerg("starting adding disk in the raid array");
				md0.diskManipulation(disk, "add");// log->emerg("adding disk fail");
				log->emerg("adding disk done");
			}
		}
		catch(exception &e){
			log->error("{}", e.what());
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}
	}
	else if(rebuildStarted){
		log->info("rebuild started");
	}

	else if(rebuildFinished){
		log->info("rebuild finished");
	}

	return EXIT_SUCCESS;
}

int progDetected(std::string program){
	vector<string> arg;
	string output, error;
	int exitStatus;
	Raid tmp;
	try{
		Command::exec(program, arg, output, error, exitStatus);
		if(error.find("execvp") != string::npos)
			throw runtime_error(program+" is not installed");
	}
	catch(exception &e){
		throw;
	}
	return EXIT_SUCCESS;
}


