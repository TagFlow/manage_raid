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

#include "Raid.h"
//#include "spdlog/spdlog.h"
#include "configuration/configuration.h"

using namespace std;

int main(int argc, char*argv[]) {

	vector<string> 	options;
	Configuration	config;
	string 			raidDisk, disk, state, raidName, raidMount, logFile;
	int 			i;

	// Read config file
	config.Load("raid.conf");
	if (!(config.Get("RAID_NAME", 	raidName)	&&
		  config.Get("RAID_MOUNT", 	raidMount)	&&
		  config.Get("LOG_FILE",	logFile)))	{

		cerr << "Error : missing parameter in configuration file." << endl;
		return EXIT_FAILURE;
	}

	Raid md0(raidName, raidMount);

	for(i=0;i<argc;i++) options.push_back(argv[i]);

	if(argc>1){
		if(options[1] == "--tagflow"){
			for(i=1;i<argc;i++){
				if(options[i] == "--size"){
					int Aspace, Tspace;
					md0.statMem(Aspace, Tspace);
					cout << "Space state :" << endl;
					cout << "available space : " << Aspace << "GB" << endl;
					cout << "total space     : " << Tspace << "GB" << endl;
				}

				if(options[i] == "--reconstruction-state"){
					double recovery, finish, speed;
					md0.rebuildState(recovery, finish, speed);
					cout << "rebuild state :" << endl;
					cout << "recovery = " << recovery << endl;
					cout << "finish   = " << finish << endl;
					cout << "speed    = " << speed << endl;
				}
			}
		}
		else{

			if(options[1] == "Fail"){
				raidDisk 	= options[2];
				disk		= options[3];

				md0.diskManipulation(disk, "remove");
				md0.smartTest(disk, state);
				if(state == ""){	// not defect disk
					// md0.diskManipulation(disk, "format");
					md0.diskManipulation(disk, "add");
				}
				else{				// defect disk
					// !!! NOTIFICATION DANS LE LOG !!!
					do{				// wait that the disk is change
						 sleep(30);
					}while(md0.diskDetection(disk));

					// md0.diskManipulation(disk, "format");
					md0.diskManipulation(disk, "add");
				}
			}
			/*if(options[1] == "RebuildStarted"){
				fichier << "rebuild started" << endl;
				// !!! MOTIFICATION DANS LE LOG !!!
			}*/

			if(options[1] == "RebuildFinished"){
				// !!! MOTIFICATION DANS LE LOG !!!
			}

		}
		return EXIT_SUCCESS;
	}
	else{
		cerr << "Error : need argument";
		return EXIT_FAILURE;
	}


}

