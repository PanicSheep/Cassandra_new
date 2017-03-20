#include "path.h"

CPath::CPath(std::string path)
{
	const std::string NETWORK_SEPARATOR = FOLDER_SEPARATOR + FOLDER_SEPARATOR;
	if (path.substr(1, 1) == ":" || path.substr(0, NETWORK_SEPARATOR.length()) == NETWORK_SEPARATOR || path.substr(0, 1) == ".") // absolute path
		m_fullpath = path;
	else // relative path
	{
		m_fullpath = gConfigurations.GetConfiguration("ProgramFolderPath") + path;
		
		std::string token = FOLDER_SEPARATOR + "..";
		std::size_t pos1;
		while ((pos1 = m_fullpath.find(token)) != std::string::npos)
		{
			std::size_t pos2 = m_fullpath.rfind(FOLDER_SEPARATOR, pos1 - 1);
			m_fullpath = m_fullpath.substr(0, pos2) + m_fullpath.substr(pos1 + token.length());
		}
	}
}

std::string CPath::GetRelativeFolderPath() const
{
	assert(IsFolder());
	throw "not implemented yet";
}

std::string CPath::GetRelativeFilePath() const
{
	assert(IsFile());
	throw "not implemented yet";
}

std::string CPath::GetAbsoluteFolderPath() const
{
	assert(IsFolder());
	return m_fullpath;
}

std::string CPath::GetAbsoluteFilePath() const
{
	assert(IsFile());
	return m_fullpath;
}

std::string CPath::GetFileName() const
{
	assert(IsFile());
	return m_fullpath.substr(m_fullpath.rfind(FOLDER_SEPARATOR) + 1);
}

std::string CPath::GetFolderName() const
{
	assert(IsFolder());
	std::size_t pos = m_fullpath.rfind(FOLDER_SEPARATOR, m_fullpath.length() - 2);
	return m_fullpath.substr(pos + 1, m_fullpath.length() - pos - 2);
}

std::string CPath::GetFileNameExtension() const
{
	assert(IsFile());
	return m_fullpath.substr(m_fullpath.rfind('.') + 1);
}

bool CPath::IsFile() const
{
	return m_fullpath.substr(m_fullpath.length() - 1) != FOLDER_SEPARATOR;
}

bool CPath::IsFolder() const
{
	return m_fullpath.substr(m_fullpath.length() - 1) == FOLDER_SEPARATOR;
}