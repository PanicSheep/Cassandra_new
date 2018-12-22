#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

#include "Path.h"
#include "PuzzleCollection.h"
#include "Serializable.h"


namespace IO
{
	PuzzleVector LoadPuzzles(const CPath&);
	std::vector<PuzzleVector> LoadPuzzles(const std::vector<CPath>&);

	void SavePuzzles(const PuzzleVector&, const CPath&);
	void SavePuzzles(const std::vector<PuzzleVector>&, const std::vector<CPath>&);
	
	class AutoSavingPuzzleVector : public PuzzleVectorGuard
	{
		std::atomic_bool m_terminate;
		std::condition_variable_any m_cv;
		std::thread m_thread;

	public:
		AutoSavingPuzzleVector(PuzzleVector&&, const CPath& output_file, std::chrono::seconds interval);
		~AutoSavingPuzzleVector() override;

		PuzzleVector Release() override;

		void Save(const CPath&) const;
	};
}