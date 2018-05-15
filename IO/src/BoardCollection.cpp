#include "BoardCollection.h"
#include <iostream>
#include <functional>

namespace IO
{
	CBoardCollection::CBoardCollection(const CPath & file)
	{
		Load(file);
	}

	void CBoardCollection::Load(const CPath & file)
	{
		std::unique_ptr<iBoardArchive> archive;
		std::unique_ptr<StreamArchive> fstream = std::make_unique<fstreamArchive>(file, true);
		const auto ext = file.GetExtension();

		if (ext == "pos")		archive = std::make_unique<BoardToStreamConverter>(std::move(fstream));
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else
			return;

		while (true)
		{
			std::unique_ptr<CBoard> board = archive->DeserializeBoard();
			if (board == nullptr)
				break;
			m_boards.push_back(std::move(board));
		}
	}

	void CBoardCollection::Save(const CPath & file) const
	{
		std::unique_ptr<oBoardArchive> archive;
		std::unique_ptr<StreamArchive> fstream = std::make_unique<fstreamArchive>(file, false);

		const auto ext = file.GetExtension();

		if (ext == "pos")		archive = std::make_unique<BoardToStreamConverter>(std::move(fstream));
		else if (ext == "script")	archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else if (ext == "full")		archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else if (ext == "obf")		archive = std::make_unique<SingleLineStreamDecorator>(std::move(fstream));
		else
			return;

		for (const auto& it : m_boards)
			*archive << *it;
	}

	AutoSavingBoardCollection::AutoSavingBoardCollection(const CPath& input_file, const CPath& output_file, std::chrono::seconds interval)
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
				CBoardCollection::Save(output_file);
			}
		}
		);
	}

	AutoSavingBoardCollection::~AutoSavingBoardCollection()
	{
		m_terminate.store(true, std::memory_order_release);
	}

	void AutoSavingBoardCollection::push_back(std::unique_ptr<CBoard>&& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::push_back(std::move(pos));
	}

	void AutoSavingBoardCollection::push_back(const std::unique_ptr<CBoard>& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::push_back(std::unique_ptr<CBoard>(pos->Clone()));
	}

	std::unique_ptr<CBoard> AutoSavingBoardCollection::Get(std::size_t index) const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		return CBoardCollection::Get(index);
	}

	void AutoSavingBoardCollection::Set(std::size_t index, std::unique_ptr<CBoard>&& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::Set(index, std::move(pos));
	}

	void AutoSavingBoardCollection::Set(std::size_t index, const std::unique_ptr<CBoard>& pos)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::Set(index, pos);
	}

	std::size_t AutoSavingBoardCollection::size() const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		return CBoardCollection::size();
	}

	void AutoSavingBoardCollection::Load(const CPath & file)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::Load(file);
	}

	void AutoSavingBoardCollection::Save(const CPath & file) const
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		CBoardCollection::Save(file);
	}
}