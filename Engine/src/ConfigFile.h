#pragma once
#include <unordered_map>
#include <string>
#include "Path.h"

class CConfigurations
{
	std::unordered_map<std::string, std::string> m_conf;
public:
	CConfigurations() {}

	void			Load(const CPath& ConfigFile);

	bool 			Has(const std::string& key) const;
	std::string 	Get(const std::string& key) const;
	void 			Set(const std::string& key, std::string&& value);
private:
};

extern CConfigurations Configurations;
