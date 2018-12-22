#include "IoPuzzleCollection.h"
#include "FlipFast.h"
#include "LastFlipCounter.h"
#include "Path.h"
#include "PositionGenerator.h"
#include "Utility.h"
#include <chrono>
#include <iostream>

using namespace IO;

void Next(const CPath& input_file, const CPath& output_file)
{
	const auto input = LoadPuzzles(input_file);
	PuzzleVector output;

	for (const auto & pos : input)
	{
		auto possibleMoves = pos->GetPosition().PossibleMoves();
		while (!possibleMoves.empty())
		{
			output.push_back(std::unique_ptr<CPuzzle>(pos->Play(possibleMoves.ExtractMove())));
		}
	}

	SavePuzzles(output, output_file);
}

void help()
{
	std::cout << "Expected one of these inputs:\n"
		<< " -rnd -o filename -n size -e EmptyCount\n"
		<< " -all -o filename -e EmptyCount\n"
		<< " -next -i filename -o filename\n"
		<< "with one of these options:\n"
		<< " -Score (default)\n"
		<< " -AllDepthScore\n"
		<< "and optional:\n"
		<< " -q for quiet mode\n";
}

int main(int argc, char* argv[])
{
	CPath Input;
	CPath Output;
	std::size_t size = 0;
	uint8_t NumEmpties = 0;
	bool rnd = false;
	bool all = false;
	bool next = false;
	bool quiet = false;
	bool AllDepthScore = false;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-q") quiet = true;
		else if (std::string(argv[i]) == "-i") Input = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-o") Output = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-n") size = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-e") NumEmpties = std::atoi(argv[++i]);
		else if (std::string(argv[i]) == "-rnd") rnd = true;
		else if (std::string(argv[i]) == "-all") all = true;
		else if (std::string(argv[i]) == "-next") next = true;
		else if (std::string(argv[i]) == "-AllDepthScore") AllDepthScore = true;
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

	CPositionGenerator PositionGenerator;
	std::unordered_set<CPosition> positions;
	if (rnd)
		positions = PositionGenerator.GenerateRandomPositionSet(NumEmpties, size);
	else if (all)
		positions = PositionGenerator.GenerateAllPositions(NumEmpties);

	PuzzleVector puzzles;
	for (const auto& pos : positions)
	{
		if (AllDepthScore)
			puzzles.push_back(std::make_unique<CPuzzleAllDepthScore>(pos));
		else
			puzzles.push_back(std::make_unique<CPuzzleScore>(pos));
	}

	SavePuzzles(puzzles, Output);

	if (!quiet)
	{
		if (rnd)
			std::cout << " done." << std::endl;
		else if (all)
			std::cout << ThousandsSeparator(positions.size()) << " positions written." << std::endl;
	}

	return 0;
}
