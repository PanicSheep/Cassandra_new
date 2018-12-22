#pragma once
#include "Args.h"
#include "Path.h"
#include "Utility.h"

#include <thread>

struct CInputParser
{
	CPath filename;
	CPath pattern_names, pattern_use;
	std::size_t RAM = 1ui64 << 30; // 1GB
	std::size_t threads = std::thread::hardware_concurrency();
	uint8_t depth = 99;
	bool test{};
	bool force{};
	bool print_each_puzzle{};

	CInputParser(const CPath& exe_folder);
	CInputParser(const CPath& exe_folder, const CArgs&);

};

std::ostream& operator<<(std::ostream&, const CInputParser&);