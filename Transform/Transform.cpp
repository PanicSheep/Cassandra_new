#include "Args.h"
#include "IoPuzzleCollection.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

enum class Mode
{
	Pos, PosScore, PosScoreDepth, PosAllDepthScore, PosAllMoveScore
};

PuzzleVector to_PuzzleAllDepthScore(const PuzzleVector& puzzles)
{
	PuzzleVector ret;
	ret.reserve(puzzles.size());
	for (const auto& it : puzzles)
		ret.emplace_back(to_PuzzleAllDepthScore(*it));
	return ret;
}

PuzzleVector Transform(const PuzzleVector& puzzles, const Mode mode)
{
	switch (mode)
	{
	case Mode::PosAllDepthScore:
		return to_PuzzleAllDepthScore(puzzles);
	default:
		throw std::runtime_error("not implemented");
	}
}

void Transform(const std::vector<CPath>& input_files, const CPath& output_file, const Mode mode)
{
	IO::SavePuzzles(Transform(Merge(IO::LoadPuzzles(input_files)), mode), output_file);
}

Mode ParseMode(const CArgs& args)
{
	if (args.Has("Pos"))
		return Mode::Pos;
	if (args.Has("PosScore"))
		return Mode::PosScore;
	if (args.Has("PosScoreDepth"))
		return Mode::PosScoreDepth;
	if (args.Has("PosAllDepthScore"))
		return Mode::PosAllDepthScore;
	if (args.Has("PosAllMoveScore"))
		return Mode::PosAllMoveScore;
	throw std::runtime_error("Invalid Mode");
}

Mode TryParseMode(const CArgs& args)
{
	try
	{
		return ParseMode(args);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw;
	}
}

void print_help()
{
	std::cout
		<< "Intended use: 'transform -i file_1 ... file_n -o file_a -Pos'\n"
		<< "Intended use: 'transform -i file_1 ... file_n -o file_a -PosScore'\n"
		<< "Intended use: 'transform -i file_1 ... file_n -o file_a -PosScoreDepth'\n"
		<< "Intended use: 'transform -i file_1 ... file_n -o file_a -PosAllDepthScore'\n"
		<< "Intended use: 'transform -i file_1 ... file_n -o file_a -PosAllMoveScore'\n"
		<< "Transforms all the input files into an output file of given type." << std::endl;
}


int main(int argc, char* argv[])
{
	CArgs args(argc, argv);

	if (args.Has("h")) { print_help(); return 0; }
	if (!args.Has("i")) { std::cerr << "Input misses parameter -i." << std::endl; return -1; }
	if (!args.Has("o")) { std::cerr << "Input misses parameter -o." << std::endl; return -1; }

	const auto input_files = to_Path(args.Get("i"));
	const auto output_file = CPath(args.Get("o").back());
	const auto mode = TryParseMode(args);

	Transform(input_files, output_file, mode);

	return 0;
}
