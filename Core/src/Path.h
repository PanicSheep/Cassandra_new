#pragma once
#include <string>
#include <vector>

class CPath
{
	std::string m_fullpath;
public:
	CPath() = default;
	CPath(const CPath&) = default;
	CPath(CPath&&) = default;
	CPath(std::string path);
	CPath(const char* path) : CPath(std::string(path)) {}

	bool IsFolder() const;
	std::string GetFolderName() const;
	std::string GetAbsoluteFolderPath() const;
	std::string GetRelativeFolderPath() const;

	bool IsFile() const;
	std::string GetRawFileName() const; // The filename without extension.
	std::string GetFullFileName() const; // The filenam with extension.
	std::string GetExtension() const;
	std::string GetAbsoluteFilePath() const;
	std::string GetRelativeFilePath() const;

	CPath& operator=(const CPath&) = default;
	CPath& operator=(CPath&&) = default;

	bool operator==(const CPath&) const;
private:
	void ProcessFolderUps();
	std::string GetRelativePath() const;
};


std::vector<CPath> to_Path(const std::vector<std::string>&);

CPath GetCurrentWorkingDirectory();