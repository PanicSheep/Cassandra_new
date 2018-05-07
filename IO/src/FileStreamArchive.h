#pragma once
#include <fstream>
#include <string>
#include "path.h"
#include "StreamSerializables.h"

class fstreamArchive : public iostreamArchive
{
	std::shared_ptr<std::fstream> fstream;
public:
	fstreamArchive(const CPath& path, bool in)
		: iostreamArchive(nullptr, nullptr)
		, fstream(std::make_shared<std::fstream>(path.GetAbsoluteFilePath(), in ? (std::ios::in | std::ios::binary) : (std::ios::out | std::ios::binary)))
	{
		istream = fstream;
		ostream = fstream;
	}
	virtual ~fstreamArchive() { Close(); }

	bool Good() override { return fstream->good() && (fstream->peek() != EOF); }
	std::string getline() override { std::string str; std::getline(*fstream, str); return str; }
	void Close() { fstream->close(); }
};