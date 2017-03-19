#pragma once
#include "macros_hell.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>

class CConfigurations
{
	std::unordered_map<std::string, std::string> m_ConfigMap;
public:
	int Verbosity;
	
	CConfigurations(); // No settings
	CConfigurations(const std::string& ProgramFilePath); // Default settings
	CConfigurations(const std::string& ProgramFilePath, const std::string& ConfigFilePath); // Settings from file
	
	bool HasConfiguration(const std::string& key) const;
	std::string GetConfiguration(const std::string& key) const;
private:
	bool TryReadConfigFile(const std::string& ConfigFilePath);
};

extern CConfigurations gConfigurations;