/*
 * Disk.cpp
 *
 *  Created on: 21 juin 2016
 *      Author: tagflow
 */

#include "Disk.h"

using namespace std;

Disk::Disk() : m_name("mdx"), m_state("clean"), m_rebuild(-1){}

Disk::Disk(string name, string state, string mountPoint, int rebuild) : m_name(name), m_state(state), m_mountPoint(mountPoint), m_rebuild(rebuild){}

void Disk::rebuild(){


}

void Disk::erase(){


}

void Disk::smartTest(){


}

void Disk::rebuildState(){


}

void Disk::statMem(int &Aspace, int &Tspace){

	struct statvfs stat;
	unsigned int gb = pow(1024,3);

	if(statvfs(m_mountPoint.c_str(), &stat) != 0){
		cout << "error : failed to get information about " << m_name << endl;
	}

	Tspace = stat.f_blocks*stat.f_frsize/gb;
	Aspace = stat.f_bfree*stat.f_frsize/gb;

}






