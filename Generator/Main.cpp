#include <iostream>
#include <chrono>
#include "BoardCollection.h"
#include "Path.h"
#include "Utility.h"
#include "PositionGenerator.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"

void Next(const CPath& Input, const CPath& Output)
{
	const auto input = CBoardCollection(Input);
	auto output = CBoardCollection(Input);

	for (const auto& pos : input)
	{
		auto possibleMoves = pos.get()->GetPosition().PossibleMoves();
		while (!possibleMoves.empty())
		{
			output.push_back(std::make_unique<CBoard>(*pos.get()->Clone()->Play(possibleMoves.ExtractMove())));
		}
	}

	output.Save(Output);
}

void help()
{
	std::cout << "Expected input:\n"
		<< "-rnd -o filename -n size -e EmptyCount\n"
		<< "-all -o filename -e EmptyCount\n"
		<< "-next -i filename -o filename\n"
		<< "-q for quiet mode\n";
}

int main(int argc, char* argv[])
{
	CountLastFlip(CPosition(), A1);
	CPath Input;
	CPath Output;
	std::size_t size = 0;
	uint8_t NumEmpties = 0;
	bool rnd = true;
	bool all = false;
	bool next = false;
	bool quiet = false;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i") Input = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-o") Output = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-n") size = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-e") NumEmpties = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-rnd") rnd = true;
		else if (std::string(argv[i]) == "-all") all = true;
		else if (std::string(argv[i]) == "-next") next = true;
		else if (std::string(argv[i]) == "-q") quiet = true;
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	}

	if (!rnd && !all) {
		std::cerr << "No mode was set. Choose -rnd or -all." << std::endl;
		help();
		throw std::runtime_error("No mode was set. Choose -rnd or -all.");
	}

	if (next)
	{
		Next(Input, Output);
		return 0;
	}

	if (!quiet)
	{
		std::cout << "Writing ";
		if (rnd)
			std::cout << ThousandsSeparator(size);
		else if (all)
			std::cout << "all";
		std::cout << " positions with " << std::to_string(NumEmpties) << " empties to " << Output.GetRelativeFilePath() << " ..." << std::flush;
	}

	CPositionGenerator posgen;
	std::unordered_set<CPosition> positions;
	if (rnd)
		positions = posgen.GenerateRandomPositionSet(NumEmpties, size);
	else if (all)
		positions = posgen.GenerateAllPositions(NumEmpties);

	CBoardCollection boards;
	for (const auto& pos : positions)
		boards.push_back(std::make_unique<CBoardScore>(pos));

	boards.Save(Output);

	if (!quiet)
	{
		if (rnd)
			std::cout << " done." << std::endl;
		else if (all)
			std::cout << ThousandsSeparator(positions.size()) << " positions written." << std::endl;
	}

	return 0;
}