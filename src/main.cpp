//============================================================================
// Name        : main.cpp
// Author      : Maxime Letourneau
// Version     : 1.0
// Description : Raid manager for mdadm
//
// Language	   : C++ 2011
//============================================================================

#include <iostream>
#include <string>
#include <vector>
#include "Raid.h"
#include <getopt.h>

//#include "spdlog/spdlog.h"

#define RAID_NAME "md0"
#define RAID_MOUNT "/media/raid"


using namespace std;

int main(int argc, char*argv[]) {

	vector<string> options;
	Raid md0(RAID_NAME, "clean", RAID_MOUNT, -1);
	string raidDisk, disk, state;
	int i;

	ofstream fichier("/home/sharebox/test.txt", ios::out|ios::trunc);
	fichier << "fichier ouvert" << endl;

	for(i=0;i<argc;i++){
		fichier << "arg : " << i << " = " << argv[i] << endl;

	}

	for(i=0;i<argc;i++) options.push_back(argv[i]);

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

			fichier << "remove : " << md0.diskManipulation(disk, "remove") << endl;
			fichier << "smart test : " << md0.smartTest(disk, state) << endl;
			if(state == ""){	// not defect disk
				fichier << "not defect disk" << endl;
				// md0.diskManipulation(disk, "format");
				fichier << "add : " << md0.diskManipulation(disk, "add") << endl;
			}
			else{				// defect disk
				fichier << "defect disk" << endl;
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
			fichier << "rebuild finished" << endl;
			// !!! MOTIFICATION DANS LE LOG !!!
		}

	}

	return EXIT_SUCCESS;
}

