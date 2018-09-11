#include "PuzzleCollection.h"

PuzzleVector Merge(std::vector<PuzzleVector>&& vec)
{
	PuzzleVector ret;
	if (vec.size())
		ret = std::move(vec.front());

	for (auto it = vec.begin() + 1; it != vec.end(); ++it)
		std::move(it->begin(), it->end(), std::back_inserter(ret));

	return ret;
}
