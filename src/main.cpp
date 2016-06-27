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

#define RAID_NAME "md0"
#define RAID_MOUNT "/media/tagflow"


using namespace std;

int main(int argc, char*argv[]) {

	Raid md0(RAID_NAME, "clean", RAID_MOUNT, -1);

	/*int Tspace=0;
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
	md0.smartTest("sda", state);

	return 0;
}

