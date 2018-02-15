#include "configfile.h"
#include <fstream>
#include <algorithm>

CConfigurations Configurations;

void CConfigurations::Load(const CPath& ConfigFile)
{
	std::ifstream file;
	file.open(ConfigFile.GetFullFileName());
	if (!file)
		throw std::runtime_error("Could not open file: '" + ConfigFile.GetFullFileName() + "'.");

	const std::string token = " = ";
	std::string line;
	while (std::getline(file, line))
	{
		std::size_t pos = line.find(token);
		std::string key = line.substr(0, pos);
		line.erase(0, pos + token.length());
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

		m_conf[key] = line;
	}

	file.close();
}

bool CConfigurations::Has(const std::string& key) const
{
	return m_conf.count(key) > 0;
}

std::string CConfigurations::Get(const std::string& key) const
{
	return m_conf.at(key);
}

void CConfigurations::Set(const std::string& key, std::string&& value)
{
	m_conf[key] = std::move(value);
}
