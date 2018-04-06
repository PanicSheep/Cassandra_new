#include "BoardCollection.h"
#include <iostream>

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

	std::unique_ptr<CBoard> board;
	while (board = archive->DeserializeBoard())
		m_boards.push_back(std::move(board));
}

void CBoardCollection::Save(const CPath & file) const
{
	std::unique_ptr<oBoardArchive> archive;
	std::unique_ptr<StreamArchive> fstream = std::make_unique<fstreamArchive>(file, false);
	//std::unique_ptr<StreamArchive> fstream = std::make_unique<iostreamArchive>(std::shared_ptr<std::istream>(&std::cin, [](void*) {}), std::shared_ptr<std::ostream>(&std::cout, [](void*) {}));
	
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