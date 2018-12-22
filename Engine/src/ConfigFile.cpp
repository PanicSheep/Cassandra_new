#include "configfile.h"
#include <algorithm>
#include <fstream>

CConfigurations::CConfigurations(const CPath & ConfigFile)
{
	Load(ConfigFile);
}

void CConfigurations::Load(const CPath& ConfigFile)
{
	std::ifstream file;
	file.open(ConfigFile.GetAbsoluteFilePath());
	if (!file)
		throw std::runtime_error("Could not open file: '" + ConfigFile.GetAbsoluteFilePath() + "'.");

	const std::string delimiter = " = ";
	std::string line;
	while (std::getline(file, line))
	{
		const std::size_t begin_of_delimiter = line.find(delimiter);
		const std::size_t end_of_delimiter = begin_of_delimiter + delimiter.length();
		const std::size_t end_of_value = line.find('\r', end_of_delimiter);

		const std::string key = line.substr(0, begin_of_delimiter);
		const std::string value = line.substr(end_of_delimiter, end_of_value - end_of_delimiter);

		m_conf[key] = value;
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
