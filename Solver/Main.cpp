#include "BoardCollection.h"
#include "Path.h"
#include "Utility.h"
#include "PositionGenerator.h"
#include "Search.h"
#include <iostream>
#include <chrono>


void help()
{
	std::cout << "Expected input:\n"
		<< "-i filename\n"
		<< "-test\n";
}

int main(int argc, char* argv[])
{
	CPath Input("..\\pos\\fforum-1-19.obf");
	bool test = true;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i") Input = CPath(argv[++i]);
		else if (std::string(argv[i]) == "-test") test = true;
		else if (std::string(argv[i]) == "-h") { help(); return 0; }
	}

	SingleLineStreamDecorator SingleLineOutput(std::make_unique<iostreamArchive>(nullptr, std::shared_ptr<std::ostream>(&std::cout, [](void*) {})));
	const auto boards = CBoardCollection(Input);
	for (const auto& board : boards)
	{
		Search* search = new NegaMaxSearch();
		auto score = search->Eval(board->GetPosition());

		if (test)
			if (const auto BoarScore = std::dynamic_pointer_cast<CBoardAllMoveScore>(board))
			{
				SingleLineOutput << *board;
				if (score == BoarScore->MaxScore())
					std::cout << " Score: " << SignedInt(score);
				else
					std::cout << " Wrong value! Calculated: " << SignedInt(score) << ", expected: " << SignedInt(BoarScore->MaxScore()) << std::endl;
				std::cout << ", NodeCount: " << ThousandsSeparator(search->GetNodeCount()) << std::endl;
			}
		delete search;
	}
	return 0;
}