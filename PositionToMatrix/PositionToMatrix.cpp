#include "Args.h"
#include "ConfigFile.h"
#include "IoPuzzleCollection.h"
#include "IoPattern.h"
#include "PositionToMatrix.h"
#include "VectorIO.h"

#include <omp.h>
#include <iostream>

using namespace IO;

void PrintHelp()
{
	std::cout << "Makes matrices and a vectors out of positions.\n" <<
		"Arguments:\n" <<
		"-pos abc.xyz                    Position filename.\n" <<
		"-o abc\\                         Output folder path.\n" <<
		"-pattern name1 name2 name3      Patterns.\n" <<
		"-h                              Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	const auto exe_folder = CPath(argv[0]).GetAbsoluteFolderPath();
	CArgs args(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	const CPath position_filename = args.Get("pos").back();
	const CPath output_folder = args.Get("o").back();
	const auto patterns = args.Get("pattern");
	Pattern::IO::CNameLookupTable pattern_translator(exe_folder + "pattern_names.ini");

	std::cout << "Converting positions to matrices and vector.\n";
	std::cout << "Input position: '" << position_filename.GetRelativeFilePath() << "'\n";
	std::cout << "Output folder: '" << output_folder.GetRelativeFolderPath() << "'\n";
	std::cout << "Pattern: " << join(patterns, ",") << std::endl;

	std::cout << "Loading positions...";
	const auto start_time = std::chrono::high_resolution_clock::now();
	auto puzzles = LoadPuzzles(position_filename);
	const auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(end_time - start_time) << std::endl;

	#pragma omp parallel for schedule(dynamic, 1)
	for (int64_t i = 0; i < static_cast<int64_t>(patterns.size()); i++)
	{
		const uint64_t pattern = pattern_translator.Translate(patterns[i]);
		const CPath matrix_file = output_folder.GetAbsoluteFolderPath() + position_filename.GetRawFileName() + "_" + patterns[i] + ".m";

		const auto start_time = std::chrono::high_resolution_clock::now();
		const auto mat = to_Matrix<uint8_t, uint32_t>(puzzles, pattern);
		mat.save(matrix_file.GetAbsoluteFilePath());
		const auto end_time = std::chrono::high_resolution_clock::now();

		#pragma omp critical
		{
			std::cout << "Processed pattern '" << patterns[i] << "' in " << time_format(end_time - start_time) << std::endl;
		}
	}

	const CPath vector_file = output_folder.GetAbsoluteFolderPath() + position_filename.GetRawFileName() + ".v";
	write_to_file(vector_file.GetAbsoluteFilePath(), to_Vector<float>(puzzles));

	return 0;
}