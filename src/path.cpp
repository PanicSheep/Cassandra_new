#include "path.h"

bool StartsWith(const std::string& string, const std::string& substring)
{
	return string.substr(0, substring.length()) == substring;
}

bool IsAbsolutePath(const std::string& path)
{
	const std::string NETWORK_SEPARATOR = FOLDER_SEPARATOR + FOLDER_SEPARATOR;

	if (path.substr(1, 1) == ":")
		return true;
	if (StartsWith(path, NETWORK_SEPARATOR))
		return true;
	if (StartsWith(path, "~"))
		return true;
	return false;
}

CPath::CPath(std::string path)
{
	replace_all(path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	if (IsAbsolutePath(path))
		m_fullpath = path;
	else // relative path
	{
		m_fullpath = GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + path;
		ProcessFolderUps();
	}
}

std::string CPath::GetRelativeFolderPath() const
{
	assert(IsFolder());
	return GetRelativePath();
}

std::string CPath::GetRelativeFilePath() const
{
	assert(IsFile());
	return GetRelativePath();
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

std::string CPath::GetRawFileName() const
{
	assert(IsFile());
	const auto FolderSeparatorPos = m_fullpath.rfind(FOLDER_SEPARATOR);
	const auto DotPos = m_fullpath.rfind(".");
	if ((DotPos > FolderSeparatorPos) && (DotPos != std::string::npos))
		return m_fullpath.substr(FolderSeparatorPos + 1, DotPos - FolderSeparatorPos - 1);
	else
		return m_fullpath.substr(FolderSeparatorPos + 1);
}

std::string CPath::GetFullFileName() const
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

std::string CPath::GetExtension() const
{
	assert(IsFile());
	return m_fullpath.substr(m_fullpath.rfind('.') + 1);
}

bool CPath::IsFile() const
{
	if (m_fullpath.length() > 0)
		return m_fullpath.substr(m_fullpath.length() - 1) != FOLDER_SEPARATOR;
	else
		return false;
}

bool CPath::IsFolder() const
{
	if (m_fullpath.length() > 0)
		return m_fullpath.substr(m_fullpath.length() - 1) == FOLDER_SEPARATOR;
	else
		return false;
}

void CPath::ProcessFolderUps()
{
	std::string Token = FOLDER_SEPARATOR + "..";
	std::size_t TokenPos;
	while ((TokenPos = m_fullpath.find(Token)) != std::string::npos)
	{
		std::size_t FolderStartPos = m_fullpath.rfind(FOLDER_SEPARATOR, TokenPos - 1);
		m_fullpath = m_fullpath.substr(0, FolderStartPos) + m_fullpath.substr(TokenPos + Token.length());
	}
}

std::string CPath::GetRelativePath() const
{
	auto cwd = GetCurrentWorkingDirectory();
	auto SplittedCwd = split(cwd, FOLDER_SEPARATOR);
	auto SplittedFullPath = split(m_fullpath, FOLDER_SEPARATOR);
	
	std::size_t idx = 0;
	while ((idx < SplittedCwd.size()) && (idx < SplittedFullPath.size()) && (SplittedCwd[idx] == SplittedFullPath[idx]))
		idx++;
	
	if ((idx == 0) || ((idx == 1) && (SplittedFullPath[0] == ""))) // No common parts
		return m_fullpath;
	
	const std::size_t NumFoldersUp = SplittedCwd.size() - idx;
	std::vector<std::string> RelPath(NumFoldersUp, "..");
	
	RelPath.insert(RelPath.end(), SplittedFullPath.begin() + idx, SplittedFullPath.end());
		
	return join(RelPath, FOLDER_SEPARATOR);
}