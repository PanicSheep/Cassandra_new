#include "configfile.h"

CConfigurations gConfigurations = CConfigurations();



CConfigurations::CConfigurations()
{
	Verbosity = 1;
}

CConfigurations::CConfigurations(const std::string& ProgramFilePath)
{
	Verbosity = 1;
	
	m_ConfigMap["ProgramFilePath"] = ProgramFilePath;
	m_ConfigMap["ProgramFolderPath"] = ProgramFilePath.substr(0, ProgramFilePath.rfind(FOLDER_SEPARATOR) + 1);
	
	std::string ConfigFilePath = m_ConfigMap["ProgramFolderPath"] + "config.ini";
	if (TryReadConfigFile(ConfigFilePath))
		m_ConfigMap["ConfigFilePath"] = ConfigFilePath;
}

CConfigurations::CConfigurations(const std::string& ProgramFilePath, const std::string& ConfigFilePath)
{
	Verbosity = 1;
	
	m_ConfigMap["ProgramFilePath"] = ProgramFilePath;
	m_ConfigMap["ProgramFolderPath"] = ProgramFilePath.substr(0, ProgramFilePath.rfind(FOLDER_SEPARATOR) + 1);
	
	if (TryReadConfigFile(ConfigFilePath))
		m_ConfigMap["ConfigFilePath"] = ConfigFilePath;
}

bool CConfigurations::TryReadConfigFile(const std::string& ConfigFilePath)
{
	std::size_t pos;
	std::string s, key;
	std::ifstream file;
	
	file.open(ConfigFilePath);
	if(!file)
		return false;

	while (std::getline(file, s))
	{
		pos = s.find(" = ");
		key = s.substr(0, pos);
		s.erase(0, pos + 3);

		s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
		m_ConfigMap[key] = s;
	}

	file.close();
	return true;
}

bool CConfigurations::HasConfiguration(const std::string& key) const
{
	return m_ConfigMap.count(key) > 0;
}

std::string CConfigurations::GetConfiguration(const std::string& key) const
{
	return m_ConfigMap.at(key);
}
