#pragma once
#include <vector>
#include "position.h"
#include "datamanipulation.h"
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "FileStreamArchive.h"
#include "StreamConverter.h"
#include "SingleLineStreamDecorator.h"
#include "MultiLineStreamDecorator.h"

std::vector<std::unique_ptr<CPosition>> LoadVector(const CPath& filename, std::size_t size = std::numeric_limits<std::size_t>::max());

void Save(const CPath& filename, const std::vector<std::unique_ptr<CPosition>>& positions);

template <typename ITERATOR>
inline void Save(const CPath& filename, ITERATOR begin, ITERATOR end)
{
	std::unique_ptr<oPositionArchive> archive;
	oArchive& fstream = fstreamArchive(filename);
	const auto ext = filename.GetExtension();

	if (ext == "pos")		archive = std::make_unique<StreamConverter>(fstream);
	if (ext == "script")	archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	if (ext == "full")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	if (ext == "obf")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);

	for (auto it = begin; it != end; ++it)
		*archive << *it;
}

template <typename T>
inline std::vector<std::unique_ptr<T>> Transform(const std::vector<std::unique_ptr<CPosition>>& positions)
{
	std::vector<std::unique_ptr<CPosition>> ret;
	for (const auto& it : positions)
	{
		switch (T::classId) {
		case CPosition::classId: ret.push_back(std::make_unique<CPosition>(*it)); break;
		case CPositionScore::classId: ret.push_back(std::make_unique<CPositionScore>(*it)); break;
		case CPositionScoreDepth::classId: ret.push_back(std::make_unique<CPositionScoreDepth>(*it)); break;
		case CPositionFullScore::classId: ret.push_back(std::make_unique<CPositionFullScore>(*it)); break;
		case CPositionAllScore::classId: ret.push_back(std::make_unique<CPositionAllScore>(*it)); break;
		default: throw std::runtime_error("Invalid class name");
		}
	}
	return ret;
}

template <typename T, typename ITERATOR>
inline void SaveTransform(const CPath& filename, ITERATOR begin, ITERATOR end)
{
	std::unique_ptr<oStreamArchive> archive;
	const auto ext = filename.GetExtension();

	if (ext == "pos") archive = std::make_unique<fstreamArchive>(filename);

	for (auto it = begin; it != end; ++it)
	{
		switch (T::classId) {
		case CPosition::classId: *archive << CPosition(*it); break;
		case CPositionScore::classId: *archive << CPositionScore(*it); break;
		case CPositionScoreDepth::classId: *archive << CPositionScoreDepth(*it); break;
		case CPositionFullScore::classId: *archive << CPositionFullScore(*it); break;
		case CPositionAllScore::classId: *archive << CPositionAllScore(*it); break;
		default: throw std::runtime_error("Invalid class name");
		}
	}
}