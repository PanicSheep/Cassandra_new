#include "Path.h"
#include "MacrosHell.h"
#include "Utility.h"

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
		m_fullpath = std::move(path);
	else // relative path
	{
		if (StartsWith(path, FOLDER_SEPARATOR))
			m_fullpath = GetCurrentWorkingDirectory() + path;
		else
			m_fullpath = GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + path;
	}
	ProcessFolderUps();
}

std::string CPath::GetRelativeFolderPath() const
{
	if (IsFolder())
		return GetRelativePath();
	else
	{
		const auto RelativePath = GetRelativePath();
		const auto pos = RelativePath.rfind(GetFullFileName());
		return RelativePath.substr(0, pos);
	}
}

std::string CPath::GetRelativeFilePath() const
{
	if (!IsFile())
		throw std::logic_error("Not a file!");

	return GetRelativePath();
}

std::string CPath::GetAbsoluteFolderPath() const
{
	if (IsFolder())
		return m_fullpath;
	else
	{
		const auto pos = m_fullpath.rfind(GetFullFileName());
		return m_fullpath.substr(0, pos);
	}
}

std::string CPath::GetAbsoluteFilePath() const
{
	if (!IsFile())
		throw std::logic_error("Not a file!");

	return m_fullpath;
}

std::string CPath::GetRawFileName() const
{
	if (!IsFile())
		throw std::logic_error("Not a file!");

	const auto FolderSeparatorPos = m_fullpath.rfind(FOLDER_SEPARATOR);
	const auto DotPos = m_fullpath.rfind(".");

	if ((DotPos > FolderSeparatorPos) && (DotPos != std::string::npos))
		return m_fullpath.substr(FolderSeparatorPos + 1, DotPos - FolderSeparatorPos - 1);
	else
		return m_fullpath.substr(FolderSeparatorPos + 1);
}

std::string CPath::GetFullFileName() const
{
	if (!IsFile())
		throw std::logic_error("Not a file!");

	return m_fullpath.substr(m_fullpath.rfind(FOLDER_SEPARATOR) + 1);
}

std::string CPath::GetFolderName() const
{
	if (IsFile())
	{
		std::size_t end_pos = m_fullpath.rfind(FOLDER_SEPARATOR);
		std::size_t begin_pos = m_fullpath.rfind(FOLDER_SEPARATOR, end_pos - 1);
		return m_fullpath.substr(begin_pos + 1, end_pos - begin_pos);
	}
	else
	{
		std::size_t pos = m_fullpath.rfind(FOLDER_SEPARATOR, m_fullpath.length() - 2);
		return m_fullpath.substr(pos + 1, m_fullpath.length() - pos - 1);
	}
}

std::string CPath::GetExtension() const
{
	if (!IsFile())
		throw std::logic_error("Not a file!");

	const auto DotPos = m_fullpath.rfind(".");
	const auto FolderSeparatorPos = m_fullpath.rfind(FOLDER_SEPARATOR);

	if ((DotPos == std::string::npos) || (DotPos < FolderSeparatorPos))
		return "";
	else
		return m_fullpath.substr(DotPos + 1);
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

bool CPath::operator==(const CPath& o) const
{
	return m_fullpath == o.m_fullpath;
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

std::vector<CPath> to_Path(const std::vector<std::string>& strings)
{
	std::vector<CPath> ret;
	std::transform(strings.begin(), strings.end(), std::back_inserter(ret), [](const std::string& str) { return CPath(str); });
	return ret;
}