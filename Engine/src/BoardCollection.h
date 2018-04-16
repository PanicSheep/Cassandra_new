#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "Boards.h"
#include "Path.h"

#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "FileStreamArchive.h"
#include "StreamConverter.h"
#include "SingleLineStreamDecorator.h"
#include "MultiLineStreamDecorator.h"

class CBoardCollection
{
	std::vector<std::unique_ptr<CBoard>> m_boards;
public:
	CBoardCollection() {}
	CBoardCollection(const CPath& file);
	template <typename ITERATOR> CBoardCollection(ITERATOR begin, ITERATOR end) : m_boards(begin, end) {}

	virtual void Load(const CPath& file);
	virtual void Save(const CPath& file) const;

	virtual void push_back(std::unique_ptr<CBoard>&& pos) { m_boards.push_back(std::move(pos)); }
	virtual void push_back(const std::unique_ptr<CBoard>& pos) { m_boards.push_back(pos->Clone()); }

	virtual std::unique_ptr<CBoard> Get(std::size_t index) const { return m_boards[index]->Clone(); }
	virtual void Set(std::size_t index, std::unique_ptr<CBoard>&& pos) { m_boards[index] = std::move(pos); }
	virtual void Set(std::size_t index, const std::unique_ptr<CBoard>& pos) { m_boards[index] = pos->Clone(); }

	virtual std::size_t size() const { return m_boards.size(); }
};


class AutoSavingBoardCollection : public CBoardCollection
{
	std::atomic_bool m_terminate;
	std::condition_variable m_cv;
	std::thread m_thread;

	mutable std::mutex m_mtx;

public:
	AutoSavingBoardCollection() {}
	AutoSavingBoardCollection(const CPath& file, std::chrono::seconds interval) : AutoSavingBoardCollection(file, file, interval) {}
	AutoSavingBoardCollection(const CPath& input_file, const CPath& output_file, std::chrono::seconds interval);
	~AutoSavingBoardCollection();

	void Load(const CPath& file) override;
	void Save(const CPath& file) const override;

	void push_back(std::unique_ptr<CBoard>&& pos);
	void push_back(const std::unique_ptr<CBoard>& pos);

	std::unique_ptr<CBoard> Get(std::size_t index) const override;
	void Set(std::size_t index, std::unique_ptr<CBoard>&& pos) override;
	void Set(std::size_t index, const std::unique_ptr<CBoard>& pos) override;

	std::size_t size() const override;
};