#pragma once
#include <fstream>
#include <string>
#include "path.h"
#include "StreamSerializables.h"

class fstreamArchive : public iostreamArchive
{
	std::fstream fstream;
public:
	fstreamArchive(const CPath& path) 
		: fstream(path.GetAbsoluteFilePath(), std::ios::in | std::ios::out | std::ios::binary)
		, iostreamArchive(fstream, fstream)
	{}
	virtual ~fstreamArchive() { Close(); }

	bool Good() override { return fstream.good() && (fstream.peek() != EOF); }
	std::string getline() override { std::string str; std::getline(fstream, str); return str; }
	void Close() { fstream.close(); }
};