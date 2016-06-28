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

#define RAID_NAME "md0"
#define RAID_MOUNT "/media/sharebox/9a62d70a-c1ff-44a5-aff0-4d3bd6fa9c28"


using namespace std;

int main(int argc, char*argv[]) {

	vector<string> options;
	Raid md0(RAID_NAME, "clean", RAID_MOUNT, -1);
	string raidDisk, disk, state;
	int i;

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
		raidDisk 	= options[2];
		disk		= options[3];

		if(options[1] == "Fail"){
			md0.diskManipulation(disk, "remove");
			md0.smartTest(disk, state);
			if(state == ""){	// not defect disk
				// !!! AJOUTER LE FORMATTAGE DU DISQUE !!!
				md0.diskManipulation(disk, "add");
			}
			else{				// defect disk
				// !!! NOTIFICATION DANS LE LOG !!!
				// !!! ATTENTE CHANGEMENT DISQUE !!!
				md0.diskManipulation(disk, "add");
			}
		}
		if(options[1] == "RebuildFinished"){
			// !!! MOTIFICATION DANS LE LOG !!!
		}

	}

	return 0;
}

