#pragma once
#include "Path.h"
#include <string>
#include <unordered_map>

class CConfigurations
{
	std::unordered_map<std::string, std::string> m_conf;
public:
	CConfigurations() = default;
	CConfigurations(const CPath& ConfigFile);

	void Load(const CPath& ConfigFile);

	bool Has(const std::string& key) const;
	std::string Get(const std::string& key) const;
	void Set(const std::string& key, std::string&& value);
};
