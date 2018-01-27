#include "position.h"
#include "generatepositions.h"
#include "utility.h"
#include <iostream>
#include <chrono>

void Next(const CPath& Input, const CPath& Output)
{
	const auto vec_in = LoadVector(Input);
	
	std::vector<std::unique_ptr<CPosition>> vec_out;
	for (const auto& pos : vec_in)
	{
		auto possibleMoves = pos.get()->PossibleMoves();
		while (possibleMoves)
		{
			auto newPos = std::make_unique<CPosition>(*pos.get()->Clone());
			newPos->PlayStone(BitScanLSB(possibleMoves));
			vec_out.push_back(std::move(newPos));
			
			RemoveLSB(possibleMoves);
		}
	}
	
	Save(Output, vec_out);
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
	CPath Input;
	CPath Output;
	std::size_t NumPos;
	uint8_t NumEmpties;
	bool rnd = true;
	bool all = false;
	bool next = false;
	bool quiet = false;
	
	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-i") Input = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-o") Output = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-n") NumPos = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-e") NumEmpties = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-rnd") rnd = true;
		else if (std::string(argv[i]) == "-all") all = true;
		else if (std::string(argv[i]) == "-next") next = true;
		else if (std::string(argv[i]) == "-q") quiet = true;
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	}

	if (!rnd && !all)
		throw std::runtime_error("No mode was set. Choose -rnd or -all.");
	
	if (next)
	{
		Next(Input, Output);
		return 0;
	}
	
	if (!quiet)
	{
		if (rnd)
			std::cout << "Writing " << ThousandsSeparator(NumPos) << " positions with " << std::to_string(NumEmpties) << " empties to " << Output.GetRelativeFilePath() << " ..." << std::flush;
		else if (all)
			std::cout << "Writing all positions with " << std::to_string(NumEmpties) << " empties to " << Output.GetRelativeFilePath() << " ..." << std::flush;
	}
	
	std::unordered_set<CPosition> posSet;
	if (rnd)
		posSet = GenerateRandomPositions(NumPos, NumEmpties);
	else if (all)
		posSet = GenerateAllPositions(NumEmpties);
	
	SaveTransform<CPositionScore>(Output, posSet.begin(), posSet.end());
	
	if (!quiet)
	{
		if (rnd)
			std::cout << " done." << std::endl;
		else if (all)
			std::cout << ThousandsSeparator(posSet.size()) << " positions written." << std::endl;
	}
	
	return 0;
}