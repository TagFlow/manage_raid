// imported from https://wiki.calculquebec.ca/w/C%2B%2B_:_fichier_de_configuration on 28.06.2016

#ifndef LIB_CONFIGURATION_CONFIGURATION_H_
#define LIB_CONFIGURATION_CONFIGURATION_H_

#include <map>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class Configuration
{
public:
    // clear all values
    void Clear();

    // load a configuration file
    bool Load(const string& File);

    // check if value associated with given key exists
    bool Contains(const string& key) const;

    // get value associated with given key
    bool Get(const string& key, string& value) const;
    bool Get(const string& key, int&    value) const;
    bool Get(const string& key, long&   value) const;
    bool Get(const string& key, double& value) const;
    bool Get(const string& key, bool&   value) const;

private:
    // the container
    map<string,string> data;

    // remove leading and trailing tabs and spaces
    static string Trim(const string& str);
};




#endif /* LIB_CONFIGURATION_CONFIGURATION_H_ */
