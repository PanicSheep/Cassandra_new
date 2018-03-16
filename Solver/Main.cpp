#include "BoardCollection.h"
#include "Path.h"
#include "Utility.h"
#include "PositionGenerator.h"
#include "Search.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>

void help()
{
	std::cout << "Expected input:\n"
		<< "-i filename\n"
		<< "-test\n";
}

int main(int argc, char* argv[])
{
	//CPath Input("..\\pos\\fforum-1-19.obf");
	CPath Input("..\\pos\\fforum-20-39.obf");
	//CPath Input("..\\pos\\fforum-40-59.obf");
	bool test = true;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i") Input = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-test") test = true;
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	}

	SingleLineStreamDecorator SingleLineOutput(std::make_unique<iostreamArchive>(nullptr, std::shared_ptr<std::ostream>(&std::cout, [](void*) {})));
	const auto boards = CBoardCollection(Input);
	std::vector<Search*> searches;

	std::cout << " # | depth|score|       time (s) |      nodes (N) |    N/s     " << std::endl;
	std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;

	auto startTime = std::chrono::high_resolution_clock::now();
	for (std::size_t i = 0; i < boards.size(); i++)
	{
		const auto& board = boards[i];
		searches.push_back(new PVSearch());
		auto search = searches.back();

		auto startTime = std::chrono::high_resolution_clock::now();
		auto score = search->Eval(board->GetPosition());
		auto endTime = std::chrono::high_resolution_clock::now();

		if (test)
			if (const auto BoardScore = std::dynamic_pointer_cast<CBoardAllMoveScore>(board))
			{
				std::cout
					<< std::setw(3) << i << "|"
					<< std::setw(6) << board->GetPosition().EmptyCount() << "|"
					<< (score == BoardScore->MaxScore() ? " " : "#") << DoubleDigitSignedInt(score) << (score == BoardScore->MaxScore() ? " " : "#") << "|"
					<< std::setw(16) << time_format(endTime - startTime) << "|"
					<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|"
					<< std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count())
					<< std::endl;
			}
	}
	auto endTime = std::chrono::high_resolution_clock::now();

	std::size_t NodeCounter = 0;
	for (auto search : searches)
		NodeCounter += search->GetNodeCount();

	std::cout << ThousandsSeparator(NodeCounter) << " nodes in " << time_format(endTime - startTime)
		<< " (" << ThousandsSeparator(NodeCounter * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;

	for (auto search : searches)
		delete search;
	return 0;
}