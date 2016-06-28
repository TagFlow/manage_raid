// imported from https://wiki.calculquebec.ca/w/C%2B%2B_:_fichier_de_configuration on 28.06.2016

#ifndef LIB_CONFIGURATION_CONFIGURATION_H_
#define LIB_CONFIGURATION_CONFIGURATION_H_

#include <map>
#include <string>
#include <fstream>
#include <iostream>

class Configuration
{
public:
    // clear all values
    void Clear();

    // load a configuration file
    bool Load(const std::string& File);

    // check if value associated with given key exists
    bool Contains(const std::string& key) const;

    // get value associated with given key
    bool Get(const std::string& key, std::string& value) const;
    bool Get(const std::string& key, int&    value) const;
    bool Get(const std::string& key, long&   value) const;
    bool Get(const std::string& key, double& value) const;
    bool Get(const std::string& key, bool&   value) const;

private:
    // the container
    std::map<std::string,std::string> data;

    // remove leading and trailing tabs and spaces
    static std::string Trim(const std::string& str);
};




#endif /* LIB_CONFIGURATION_CONFIGURATION_H_ */
