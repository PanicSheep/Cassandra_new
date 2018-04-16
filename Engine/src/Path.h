#pragma once
#include <string>

class CPath
{
	std::string m_fullpath;
public:
	CPath() = default;
	CPath(std::string path);

	bool IsFolder() const;
	std::string GetFolderName() const;
	std::string GetAbsoluteFolderPath() const;
	std::string GetRelativeFolderPath() const;

	bool IsFile() const;
	std::string GetRawFileName() const;
	std::string GetFullFileName() const;
	std::string GetExtension() const;
	std::string GetAbsoluteFilePath() const;
	std::string GetRelativeFilePath() const;
private:
	void ProcessFolderUps();
	std::string GetRelativePath() const;
};