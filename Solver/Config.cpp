#include "Config.h"

CInputParser::CInputParser(const CPath& exe_folder)
	: pattern_names(exe_folder + "pattern_names.ini")
	, pattern_use(exe_folder + "pattern_use.ini")
{}

CInputParser::CInputParser(const CPath& exe_folder, const CArgs& args)
	: CInputParser(exe_folder)
{
	if (args.Has("f"))
		filename = args.Get("f").back();
	if (args.Has("pattern_names"))
		pattern_names = args.Get("pattern_names").back();
	if (args.Has("pattern_use"))
		pattern_use = args.Get("pattern_use").back();
	if (args.Has("RAM"))
		RAM = ParseBytes(args.Get("RAM").back());
	if (args.Has("t"))
		threads = std::stoll(args.Get("t").back());
	if (args.Has("d"))
		depth = std::stoi(args.Get("d").back());
	test = args.Has("test");
	force = args.Has("force") || args.Has("test");
	print_each_puzzle = args.Has("v");
}

std::ostream& operator<<(std::ostream& os, const CInputParser& config)
{
	os << "Filename: " << config.filename.GetAbsoluteFilePath() << '\n';
	os << "RAM: " << std::to_string(config.RAM) << '\n';
	os << "Threads: " << std::to_string(config.threads) << '\n';
	os << "Depth: " << std::to_string(config.depth) << '\n';
	os << "Test puzzles: " << std::boolalpha << config.test << '\n';
	os << "Force solving: " << std::boolalpha << config.force << '\n';
	os << "Print each puzzle: " << std::boolalpha << config.print_each_puzzle << '\n';
	return os;
}