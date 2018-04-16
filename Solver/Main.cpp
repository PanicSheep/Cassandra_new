#include "BoardCollection.h"
#include "Path.h"
#include "Utility.h"
#include "PositionGenerator.h"
#include "Search.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>

void help()
{
	std::cout << "Expected input:\n"
		<< "-f filename\n"
		<< "-test\n";
}

int main(int argc, char* argv[])
{
	CPath filename;
	std::size_t threads = std::thread::hardware_concurrency();
	bool test = false;
	bool print_each_board = false;

	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-f") filename = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-v") { print_each_board = true; }
		else if (std::string(argv[i]) == "-test") test = true;
		else if (std::string(argv[i]) == "-t") threads = std::stoi(argv[++i]);
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	}

	std::unique_ptr<CBoardCollection> boards;
	if (test)
		boards = std::make_unique<CBoardCollection>(filename);
	else
		boards = std::make_unique<AutoSavingBoardCollection>(filename, std::chrono::seconds(300));

	auto env = std::make_shared<Environment>(std::make_shared<CConfigurations>(), std::make_shared<CHashTablePVS>(16'000'000));
	std::vector<std::unique_ptr<Search>> searches;
	for (std::size_t i = 0; i < boards->size(); i++)
		searches.emplace_back(std::make_unique<PVSearch>(env));

	if (print_each_board)
	{
		std::cout << " # | depth|score|       time (s) |      nodes (N) |    N/s     " << std::endl;
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;
	}

	const auto startTime = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for schedule(static, 1) num_threads(threads)
	for (int64_t i = 0; i < boards->size(); i++)
	{
		auto board = boards->Get(i);
		std::unique_ptr<Search>& search = searches[i];

		if (board->IsSolved())
			continue;

		auto startTime = std::chrono::high_resolution_clock::now();
		auto score = search->Eval(board->GetPosition());
		auto endTime = std::chrono::high_resolution_clock::now();

		if (print_each_board)
		{
			if (const auto BoardScore = dynamic_cast<CBoardAllMoveScore*>(board.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << board->GetPosition().EmptyCount() << "|"
						<< (test && score != BoardScore->MaxScore() ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != BoardScore->MaxScore() ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
					std::cout << std::endl;
				}
			}
			else if (const auto BoardScore = dynamic_cast<CBoardScore*>(board.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << board->GetPosition().EmptyCount() << "|"
						<< (test && score != BoardScore->score ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != BoardScore->score ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";
					std::wcout << score << "§" << BoardScore->score;

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
						std::cout << std::endl;
				}
			}
		}

		if (const auto BoardScore = dynamic_cast<CBoardScore*>(board.get()))
			BoardScore->score = score;

		boards->Set(i, board);
	}
	const auto endTime = std::chrono::high_resolution_clock::now();

	std::size_t NodeCounter = 0;
	for (auto& search : searches)
		NodeCounter += search->GetNodeCount();

	const double time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	std::cout << ThousandsSeparator(NodeCounter) << " nodes in " << time_format(endTime - startTime)
		<< " (" << ThousandsSeparator(NodeCounter * 1000.0 / time_diff) << " N/s)" << std::endl;
	
	if (!test)
		boards->Save(filename);
	return 0;
}