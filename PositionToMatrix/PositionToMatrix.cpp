#include "Args.h"
#include "IoPuzzleCollection.h"
#include "ConfigFile.h"
#include "PositionToMatrix.h"
#include "VectorIO.h"
#include <omp.h>
#include <iostream>

using namespace IO;

bool Check_argument(const std::string& message_prefix, const CArgs& args, CLI_Arg arg)
{
	if (args.Has(arg.name))
	{
		if (arg.size == CLI_Arg::UNDEFINED)
			return true;
		else if (args.Get(arg.name).size() == arg.size)
			return true;
		else
		{
			std::cerr << message_prefix << ": Wrong argument count of '-" << arg.name << "'. Expectet " << arg.size << ", having " << args.Get(arg.name).size() << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << message_prefix << ": Missing argument '-" << arg.name << "'." << std::endl;
		return false;
	}
}

bool Check_mandatory_argument(const CArgs& args, CLI_Arg arg)
{
	return Check_argument("ERROR", args, arg);
}

bool Check_optional_argument(const CArgs& args, CLI_Arg arg)
{
	return Check_argument("WARNING", args, arg);
}

const CLI_Arg CLI_arg_position("pos", 1);
const CLI_Arg CLI_arg_output_folder("o", 1);
const CLI_Arg CLI_arg_pattern("pattern");

void PrintHelp()
{
	std::cout << "Makes matrices and a vectors out of positions.\n" <<
		"Arguments:\n" <<
		"-" << CLI_arg_position.name << " abc.xyz                    Position filename.\n" <<
		"-" << CLI_arg_output_folder.name << " abc\\                       Output folder path.\n" <<
		"-" << CLI_arg_pattern.name << " name1 name2 name3          Pattern.\n" <<
		"-h                              Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	CPattern::Initialize();

	CArgs args;
	args.Set("config", "config.ini");
	args.Load(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	// Check input
	Check_mandatory_argument(args, CLI_arg_position);
	Check_mandatory_argument(args, CLI_arg_output_folder);
	Check_optional_argument(args, CLI_arg_pattern);

	// Get arguments without config file
	const CPath config_file = args.Get("config").back();
	const CPath position_filename = args.Get(CLI_arg_position.name).back();
	const CPath output_folder = args.Get(CLI_arg_output_folder.name).back();

	CConfigurations configs(config_file);

	// Get arguments with config file
	std::vector<std::string> pattern_names;
	if (args.Has(CLI_arg_pattern.name))
		pattern_names = args.Get(CLI_arg_pattern.name);
	else if (configs.Has("active pattern"))
		pattern_names = split(configs.Get("active pattern"), " ");

	std::cout << "Converting positions to matrices and vector." << std::endl;
	std::cout << "Input position: '" << position_filename.GetRelativeFilePath() << "'" << std::endl;
	std::cout << "Output folder: '" << output_folder.GetRelativeFolderPath() << "'" << std::endl;
	std::cout << "Pattern: " << join(pattern_names, ",") << std::endl;

	std::cout << "Loading positions...";
	const auto StartTime_load = std::chrono::high_resolution_clock::now();
	auto puzzles = LoadPuzzles(position_filename);
	const auto EndTime_load = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(EndTime_load - StartTime_load) << std::endl;

	#pragma omp parallel for schedule(static, 1)
	for (int64_t i = 0; i < static_cast<int64_t>(pattern_names.size()); i++)
	{
		const auto& pat = pattern_names[i];
		uint64_t pattern;
		if (configs.Has("pattern " + pat))
			pattern = std::stoull(configs.Get("pattern " + pat), nullptr, 16);
		else
			std::cerr << "WARNING: Unknown pattern '" << pat << "'." << std::endl;

		const CPath matrix_file(output_folder.GetAbsoluteFolderPath() + position_filename.GetRawFileName() + "_" + pat + ".m");

		const auto startTime_mat = std::chrono::high_resolution_clock::now();
		auto mat = to_Matrix<uint8_t, uint32_t>(puzzles, pattern);
		mat.save(matrix_file.GetAbsoluteFilePath());
		const auto endTime_mat = std::chrono::high_resolution_clock::now();

		#pragma omp critical
		{
			std::cout << "Processing pattern '" << pat << "'...";
			std::cout << "done. " << time_format(endTime_mat - startTime_mat) << std::endl;
		}
	}

	const CPath vector_file(output_folder.GetAbsoluteFolderPath() + position_filename.GetRawFileName() + ".v");
	write_to_file(vector_file.GetAbsoluteFilePath(), to_Vector<float>(puzzles));

	return 0;
}