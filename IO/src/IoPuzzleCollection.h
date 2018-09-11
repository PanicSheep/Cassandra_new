#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <condition_variable>

#include "PuzzleCollection.h"
#include "Path.h"
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
		std::condition_variable m_cv;
		std::thread m_thread;

	public:
		AutoSavingPuzzleVector(PuzzleVector&&, CPath output_file, std::chrono::seconds interval);
		~AutoSavingPuzzleVector();

		PuzzleVector Release() override;

		void Save(const CPath&) const;
	};
}