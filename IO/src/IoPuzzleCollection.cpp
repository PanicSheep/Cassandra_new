#include "IoPuzzleCollection.h"

#include <memory>
#include <fstream>
#include <functional>

#include "BinaryStreamArchive.h"
#include "SingleLineStreamArchive.h"
#include "MultiLineStreamArchive.h"

namespace IO
{
	FilePuzzleCollection::FilePuzzleCollection(const CPath & file)
	{
		Load(file);
	}

	void FilePuzzleCollection::Load(const CPath & file)
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		std::unique_ptr<Archive> archive;
		const auto ext = file.GetExtension();

		if (ext == "pos")			archive = std::make_unique<BinaryStreamArchive>(fstream);
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else
			return;

		while (std::unique_ptr<CPuzzle> puzzle = archive->Deserialize())
			FilePuzzleCollection::push_back(std::move(puzzle));
	}

	void FilePuzzleCollection::Save(const CPath & file) const
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::out | std::ios::binary);
		std::unique_ptr<Archive> archive;
		const auto ext = file.GetExtension();

		if (ext == "pos")			archive = std::make_unique<BinaryStreamArchive>(fstream);
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamArchive>(fstream);
		else
			return;

		for (const auto& it : m_puzzles)
			*archive << *it;
	}

	AutoSavingPuzzleCollection::AutoSavingPuzzleCollection(const CPath& input_file, const CPath& output_file, std::chrono::seconds interval)
		: m_terminate(false)
	{
		Load(input_file);

		m_thread = std::thread(
			[this, output_file, interval]()
		{
			std::unique_lock<std::mutex> lock(m_mtx);
			while (m_terminate.load(std::memory_order_acquire) == false)
			{
				m_cv.wait_for(lock, interval);
				FilePuzzleCollection::Save(output_file);
			}
		}
		);
	}

	AutoSavingPuzzleCollection::~AutoSavingPuzzleCollection()
	{
		m_terminate.store(true, std::memory_order_release);
		m_thread.join();
	}

	void AutoSavingPuzzleCollection::push_back(std::unique_ptr<CPuzzle>&& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		FilePuzzleCollection::push_back(std::move(pos));
	}

	std::unique_ptr<CPuzzle> AutoSavingPuzzleCollection::Get(std::size_t index) const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		return FilePuzzleCollection::Get(index);
	}

	void AutoSavingPuzzleCollection::Set(std::size_t index, std::unique_ptr<CPuzzle>&& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		FilePuzzleCollection::Set(index, std::move(pos));
	}

	std::size_t AutoSavingPuzzleCollection::size() const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		return FilePuzzleCollection::size();
	}

	void AutoSavingPuzzleCollection::Load(const CPath & file)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		FilePuzzleCollection::Load(file);
	}

	void AutoSavingPuzzleCollection::Save(const CPath & file) const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		FilePuzzleCollection::Save(file);
	}
}