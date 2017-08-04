#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include "datamanipulation.h"
#include "position.h"
#include "utility.h"

template <typename T>
void Progress(const std::vector<T>& vec)
{
	const std::size_t solved = std::count_if(vec.begin(), vec.end(), [](const T& pos) { return pos.IsSolved(); });
	std::cout << "Solved:   " << ThousandsSeparator(solved) << std::endl;
	std::cout << "Unsolved: " << ThousandsSeparator(vec.size() - solved) << std::endl;
}

void Progress(const std::string & filename)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);

	if (filename_extension == CPositionScore     ::FILENAME_EXTENSION)		{ Progress(LoadVector<CPositionScore     >(filename)); } 
	if (filename_extension == CPositionFullScore ::FILENAME_EXTENSION)		{ Progress(LoadVector<CPositionFullScore >(filename)); } 
	if (filename_extension == CPositionScoreDepth::FILENAME_EXTENSION)		{ Progress(LoadVector<CPositionScoreDepth>(filename)); } 
	if (filename_extension == CPositionAllScore  ::FILENAME_EXTENSION)		{ Progress(LoadVector<CPositionAllScore  >(filename)); } 
	if (filename_extension == "obf")										{ Progress(LoadVector<CPositionAllScore  >(filename)); } 
}

void ScoreHistogram(const std::string & filename)
{
	std::map<int, std::size_t> score_map;
	
	const auto vec = LoadTransform<CPositionScore>(filename);
	for (const auto& it : vec) score_map[it.score]++;
	
	printf("score|   frequency  \n");
	printf("-----+--------------\n");

	for (const auto& it : score_map)
		printf(" %+2.2d |%14s\n", it.first, ThousandsSeparator(it.second).c_str());
}

void PrintHelp()
{
	// TODO
}

int main(int argc, char* argv[])
{
	bool bProg = false;
	bool bHist = false;
	std::string s_input, s_output;
	
	for (int i = 0; i < argc; ++i)
	{
		     if (std::string(argv[i]) == "-i") s_input = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-o") s_output = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-prog") bProg = true;
		else if (std::string(argv[i]) == "-hist") bHist = true;
		else if (std::string(argv[i]) == "-h") { PrintHelp(); return 0; }
	}
	
	if (bProg) Progress(s_input);
	if (bHist) ScoreHistogram(s_input);
	
	return 0;
}
