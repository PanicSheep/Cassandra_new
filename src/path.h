#pragma once
#include "macros_hell.h"
#include "configfile.h"
#include "utility.h"
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
	std::string GetFileName() const;
	std::string GetFileNameExtension() const;
	std::string GetAbsoluteFilePath() const;
	std::string GetRelativeFilePath() const;
private:
	void ProcessFolderUps();
};