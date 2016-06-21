//============================================================================
// Name        : main.cpp
// Author      : Maxime Letourneau
// Version     : 1.0
// Description : Raid manager for mdadm
//============================================================================

#include <iostream>
#include <string>
#include <stdlib.h>
#include "Disk.h"

using namespace std;

int main(int argc, char*argv[]) {

	Disk md0("md0", "clean", "/media/tagflow", -1);

	int Tspace=0;
	int Aspace=0;

	md0.statMem(Aspace,Tspace);

	cout << "total space : " << Tspace << " GB" << endl;
	cout << "available space : " << Aspace << " GB" << endl;
	return 0;
}

