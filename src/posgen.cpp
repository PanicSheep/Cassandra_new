#include "position.h"
#include "generatepositions.h"
#include "utility.h"
#include <iostream>
#include <chrono>

void Next(const std::string& s_input, const std::string& s_output)
{
	const auto vec_in = LoadTransform<CPositionScore>(s_input);
	
	std::vector<CPositionScore> vec_out;
	for (const auto& pos : vec_in)
	{
		auto possibleMoves = pos.PossibleMoves();
		while (possibleMoves)
		{
			CPositionScore newPos = pos;
			newPos.PlayStone(BitScanLSB(possibleMoves));
			vec_out.push_back(newPos);
			
			RemoveLSB(possibleMoves);
		}
	}
	
	SaveTransform<CPositionScore>(s_output, vec_out);
}

void help()
{
	std::cout << "Expected input:\n"
		<< "-rnd -o filename -n NumPos -e EmptyCount\n"
		<< "-all -o filename -e EmptyCount\n"
		<< "-next -i filename -o filename\n"
		<< "-q for quiet mode\n";
}

int main(int argc, char* argv[])
{
	std::string s_input, s_output;
	std::size_t NumPos = 0;
	uint8_t NumEmpties = 0;
	bool rnd = false;
	bool all = false;
	bool next = false;
	bool quiet = false;
	
	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-i") s_input  = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-o") s_output = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-n") NumPos = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-e") NumEmpties = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-rnd") rnd = true;
		else if (std::string(argv[i]) == "-all") all = true;
		else if (std::string(argv[i]) == "-next") next = true;
		else if (std::string(argv[i]) == "-q") quiet = true;
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	
	}
	
	if (next)
	{
		Next(s_input, s_output);
		return 0;
	}
	
	if (!quiet)
	{
		if (rnd)
			std::cout << "Writing " << ThousandsSeparator(NumPos) << " positions with " << std::to_string(NumEmpties) << " empties to " << s_output << " ..." << std::flush;
		else if (all)
			std::cout << "Writing all positions with " << std::to_string(NumEmpties) << " empties to " << s_output << " ..." << std::flush;
	}
	
	std::unordered_set<CPosition> posSet;
	if (rnd)
		posSet = GenerateRandomPositions(NumPos, NumEmpties);
	else if (all)
		posSet = GenerateAllPositions(NumEmpties);
	
	SaveTransform(s_output, posSet.begin(), posSet.end());
	
	if (!quiet)
	{
		if (rnd)
			std::cout << " done." << std::endl;
		else if (all)
			std::cout << ThousandsSeparator(posSet.size()) << " positions written." << std::endl;
	}
	
	return 0;
}