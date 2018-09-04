#include "IoPuzzleCollection.h"

#include <memory>
#include <fstream>
#include <functional>

#include "BinaryStreamArchive.h"
#include "SingleLineStreamArchive.h"
#include "MultiLineStreamArchive.h"

namespace IO
{
	PuzzleVector LoadPuzzles(const CPath& file)
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		std::unique_ptr<Archive> archive;
		const auto ext = file.GetExtension();

		if (ext == "pos")			archive = std::make_unique<BinaryStreamArchive>(fstream);
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else
			throw std::runtime_error("Unknown file extension");

		PuzzleVector vec;
		while (std::unique_ptr<CPuzzle> puzzle = archive->Deserialize())
			vec.push_back(std::move(puzzle));
		return vec;
	}

	void SavePuzzles(const PuzzleVector& vec, const CPath& file)
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::out | std::ios::binary);
		std::unique_ptr<Archive> archive;
		const auto ext = file.GetExtension();

		if (ext == "pos")			archive = std::make_unique<BinaryStreamArchive>(fstream);
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else
			throw std::runtime_error("Unknown file extension");

		for (const auto& it : vec)
			*archive << *it;
	}

	AutoSavingPuzzleVector::AutoSavingPuzzleVector(PuzzleVector&& puzzles, CPath output_file, std::chrono::seconds interval)
		: PuzzleVectorGuard(std::move(puzzles)), m_terminate(false)
	{
		m_thread = std::thread(
			[this, output_file, interval]()
		{
			std::unique_lock<std::mutex> lock(m_mtx);
			while (m_terminate.load(std::memory_order_acquire) == false)
			{
				m_cv.wait_for(lock, interval);
				if (m_terminate.load(std::memory_order_acquire) == false)
					SavePuzzles(m_puzzles, output_file);
			}
		}
		);
	}

	AutoSavingPuzzleVector::~AutoSavingPuzzleVector()
	{
		m_terminate.store(true, std::memory_order_release);
		m_cv.notify_all();
		m_thread.join();
	}

	PuzzleVector AutoSavingPuzzleVector::Release()
	{
		m_terminate.store(true, std::memory_order_release);
		m_cv.notify_all();
		m_thread.join();
		return PuzzleVectorGuard::Release();
	}

	void AutoSavingPuzzleVector::Save(const CPath & file) const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		SavePuzzles(m_puzzles, file);
	}
}