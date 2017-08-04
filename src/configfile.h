#pragma once
#include "macros_hell.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>

namespace Configfile
{
	void Initialize(const std::string& ProgramFilePath);
	void Initialize(const std::string& ProgramFilePath, const std::string& ConfigFilePath);
}

class CConfigurations
{
	std::unordered_map<std::string, std::string> m_ConfigMap;
public:
	// Verbosity == -2:         mute. Don't output anything, not evan errors.
	// Verbosity == -1:        quiet. Only output summary and errors.
	// Verbosity ==  0:       normal. Output status progress, summary and errors.
	// Verbosity == +1:      verbose. 
	// Verbosity == +2: very verbose. 
	int Verbosity;
	
	CConfigurations(); // No settings
	CConfigurations(const std::string& ProgramFilePath); // Default settings
	CConfigurations(const std::string& ProgramFilePath, const std::string& ConfigFilePath); // Settings from file
	
	bool 			HasConfiguration(const std::string& key) const;
	std::string 	GetConfiguration(const std::string& key) const;
	void 			SetConfiguration(const std::string& key, std::string value);

	void	 		Save(const std::string& ConfigFilePath) const;
private:
	bool 			TryReadConfigFile(const std::string& ConfigFilePath);
};

extern CConfigurations gConfigurations;
