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
	std::vector<std::shared_ptr<CBoard>> m_boards;
public:
	CBoardCollection() {}
	CBoardCollection(const CPath& file);
	template <typename ITERATOR> CBoardCollection(ITERATOR begin, ITERATOR end) : m_boards(begin, end) {}

	void Load(const CPath& file);
	void Save(const CPath& file) const;

	void push_back(const std::shared_ptr<CBoard>& pos) { m_boards.push_back(pos); }
	std::shared_ptr<CBoard> operator[](std::size_t index) const { return m_boards[index]; }
	std::size_t size() const { return m_boards.size(); }
	std::vector<std::shared_ptr<CBoard>>::      iterator begin()       { return m_boards.begin(); }
	std::vector<std::shared_ptr<CBoard>>::const_iterator begin() const { return m_boards.begin(); }
	std::vector<std::shared_ptr<CBoard>>::      iterator end()         { return m_boards.end(); }
	std::vector<std::shared_ptr<CBoard>>::const_iterator end() const   { return m_boards.end(); }
};