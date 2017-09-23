#include "positiontomatrix.h"
#include <omp.h>
#include <iostream>

void print_help()
{
	std::cout << "Makes matrices and a vectors out of positions.\n" <<
		"Arguments:\n" <<
		"-pos abc.xyz                    Position filename.\n" <<
		"-mat \abc                       Matrix filepath.\n" <<
		"-vec \abc                       Vector filepath.\n" <<
		"-pattern name1 name2 name3      Pattern.\n" <<
		"-h                              Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	Configfile::Initialize(argv[0]);
	Pattern::Initialize(false, false);
	
	CPath position_filename;
	CPath matrix_filepath;
	CPath vector_filepath;
	std::vector<std::string> pattern;
	std::chrono::high_resolution_clock::time_point startTime, endTime;

	for (int i = 0; i < argc; ++i)
	{
		if (std::string(argv[i]) == "-pattern") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				pattern.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-pos") position_filename = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-mat") matrix_filepath = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-vec") vector_filepath = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-v") gConfigurations.Verbosity = 1;
		else if (std::string(argv[i]) == "-h") { print_help(); return 0; }
	}
	
	if (pattern.empty())
	{
		std::vector<std::string> ActivePattern = Pattern::GetActivePattern();
		for (const auto& it : ActivePattern)
			pattern.push_back(it);
	}

	startTime = std::chrono::high_resolution_clock::now();

	// Print input data
	// ------------------------
	if (gConfigurations.Verbosity >= 1)
	{
		std::cout << "Converting positions to matrices and vector." << std::endl;
		std::cout << "Input position: '" << position_filename.GetAbsoluteFilePath() << "'" << std::endl;
		std::cout << "Output matrix path: '" << matrix_filepath.GetAbsoluteFilePath() << "'" << std::endl;
		std::cout << "Output vector path: '" << vector_filepath.GetAbsoluteFilePath() << "'" << std::endl;
		std::cout << "Pattern: ";
		for (std::size_t i = 0; i < pattern.size() - 1; i++)
			std::cout << pattern[i] << ",";
		if (pattern.size())
			std::cout << pattern.back();
		std::cout << std::endl;
	}
	// ------------------------

	std::string pos_name = position_filename.GetFileName(); // filename without path and extension.
	pos_name = pos_name.substr(0, pos_name.rfind("."));

	//std::cout << "Loading positions...";
	//startTime = std::chrono::high_resolution_clock::now();
	std::vector<CPositionScore> pos = LoadTransform<CPositionScore>(position_filename.GetAbsoluteFilePath());
	//endTime = std::chrono::high_resolution_clock::now();
	//std::cout << "done. " << time_format(endTime - startTime) << std::endl;

	for (const auto& pat : pattern)
	{
		//std::cout << "Processing pattern '" << pat << "'...";
		//startTime = std::chrono::high_resolution_clock::now();
		std::pair<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pat);
		ret.first.save(matrix_filepath.GetAbsoluteFolderPath() + FOLDER_SEPARATOR + pos_name + "_" + pat + ".m");
		//endTime = std::chrono::high_resolution_clock::now();
		//std::cout << "done. " << time_format(endTime - startTime) << std::endl;

		//std::cout << "Processing vector...";
		//startTime = std::chrono::high_resolution_clock::now();
		std::vector<float> b;
		for (const auto& it : pos)
			b.push_back(it.score);
		write_to_file(vector_filepath.GetAbsoluteFolderPath() + FOLDER_SEPARATOR + pos_name + ".v", b);
		//endTime = std::chrono::high_resolution_clock::now();
		//std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	}

	endTime = std::chrono::high_resolution_clock::now();
	std::cout << time_format(endTime - startTime) << std::endl;
	return 0;
}