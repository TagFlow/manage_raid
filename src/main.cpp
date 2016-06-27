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
	int i;

	for(i=0;i<argc;i++) options.push_back(argv[i]);

	if(options[1] == "--tagflow"){
		if(options[2] == "--size" || options[3] == "--size"){
			int Aspace, Tspace;
			md0.statMem(Aspace, Tspace);
			cout << "Space state :" << endl;
			cout << "available space : " << Aspace << "GB" << endl;
			cout << "total space     : " << Tspace << "GB" << endl;
		}

		if(options[2] == "--reconstruction-state" || options[3] == "--reconstruction-state"){
			double recovery, finish, speed;
			md0.rebuildState(recovery, finish, speed);
			cout << "rebuild state :" << endl;
			cout << "recovery = " << recovery << endl;
			cout << "finish   = " << finish << endl;
			cout << "speed    = " << speed << endl;
		}
	}
	else{



	}

/*

	int Tspace=0;
	int Aspace=0;

	md0.statMem(Aspace,Tspace);

	cout << "total space : " << Tspace << " GB" << endl;
	cout << "available space : " << Aspace << " GB" << endl;*/

	/*double recovery, finish, speed;

	md0.rebuildState(recovery, finish, speed);
	cout << "recovery = " << recovery << endl;
	cout << "finish = " << finish << endl;
	cout << "speed = " << speed << endl;*/

	string state;
	//md0.smartTest("sda", state);

	return 0;
}

