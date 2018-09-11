#include "Args.h"
#include "IoPuzzleCollection.h"
#include <algorithm>
#include <iterator>
#include <random>
#include <vector>

PuzzleVector Merge(std::vector<PuzzleVector>&& vec)
{
	PuzzleVector ret;
	if (vec.size())
		ret = std::move(vec.front());

	for (auto it = vec.begin() + 1; it != vec.end(); ++it)
		std::move(it->begin(), it->end(), std::back_inserter(ret));

	return ret;
}

PuzzleVector ExtractRandomSample(PuzzleVector& puzzles, const std::size_t count)
{
	PuzzleVector ret;
	std::sample(
		std::move_iterator(puzzles.begin()),
		std::move_iterator(puzzles.end()),
		std::back_inserter(ret),
		count,
		std::mt19937{ std::random_device{}() });
	puzzles.erase(
		std::remove_if(puzzles.begin(), puzzles.end(), [](const std::unique_ptr<CPuzzle>& puzzle) { return !puzzle; }),
		puzzles.end());
	return ret;
}

std::vector<PuzzleVector> Split(PuzzleVector&& puzzles, const std::vector<double>& fractions)
{
	const std::size_t size = puzzles.size();

	std::vector<PuzzleVector> ret;
	ret.reserve(fractions.size());

	for (const auto fraction : fractions)
		ret.emplace_back(ExtractRandomSample(puzzles, std::round(size * fraction)));

	if (puzzles.size())
		ret.emplace_back(std::move(puzzles));

	return ret;
}

void Split(const std::vector<CPath>& input_files, const std::vector<CPath>& output_files, const std::vector<double>& fractions)
{
	IO::SavePuzzles(Split(Merge(IO::LoadPuzzles(input_files)), fractions), output_files);
}

std::vector<CPath> ParseFiles(const std::vector<std::string>& vec)
{
	std::vector<CPath> ret;
	for (std::size_t i = 1; i < vec.size(); i += 2)
		ret.push_back(CPath(vec[i]));

	if (vec.size() % 2 == 1)
		ret.push_back(CPath(vec.back()));

	return ret;
}

std::vector<double> ParseFractions(const std::vector<std::string>& vec)
{
	std::vector<double> ret;
	for (std::size_t i = 0; i + 1 < vec.size(); i += 2)
		ret.push_back(std::stod(vec[i]) / 100);
	return ret;
}

void print_help()
{
	std::cout
		<< "Intended use: 'split -i file_1 ... file_n -o 20% file_a (-o 10% file_b)... -o file_z'\n"
		<< "Merges all the input files.\n"
		<< "Splits 20% of the merged files into fila_a.\n"
		<< "Splits 10% of the merged files into file_b.\n"
		<< "Puts the rest of the merged files into file_z." << std::endl;
}

int main(int argc, char* argv[])
{
	CArgs args(argc, argv);

	if (args.Has("h")) { print_help(); return 0; }
	if (!args.Has("i")) { std::cerr << "Input misses parameter -i." << std::endl; return -1; }
	if (!args.Has("o")) { std::cerr << "Input misses parameter -o." << std::endl; return -1; }

	const auto input_files = to_Path(args.Get("i"));
	const auto output_files = ParseFiles(args.Get("o"));
	const auto fractions = ParseFractions(args.Get("o"));

	Split(input_files, output_files, fractions);

	return 0;
}