#pragma once
#include <cstdint>
#include <memory>
#include <vector>

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