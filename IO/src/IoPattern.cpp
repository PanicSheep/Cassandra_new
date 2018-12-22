#include "IoPattern.h"
#include "VectorIO.h"
#include <fstream>
#include <utility>
#include <utility>

using namespace Pattern;

namespace Pattern::IO
{
	std::vector<float> Extract(std::iostream& stream, const std::size_t size)
	{
		std::vector<float> vec;
		vec.reserve(size);
		for (std::size_t i = 0; i < size; i++) {
			float f;
			stream >> f;
			vec.push_back(f);
		}
		return vec;
	}

	std::unique_ptr<Eval::CBase> CreatePattern(std::iostream& stream)
	{
		if (!stream.good())
			return nullptr;
		if (stream.peek() == EOF)
			return nullptr;

		uint64_t pattern;
		if (stream >> pattern)
		{
			auto config = Configurations::CreatePattern(pattern);
			const auto size = config->ReducedSize();
			return Eval::CreatePattern(pattern, Extract(stream, size));
		}
		return nullptr;
	}

	std::unique_ptr<Eval::CBase> CreatePattern(const CPath& file)
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		return CreatePattern(fstream);
	}

	Eval::CPack CreatePack(const std::vector<CPath>& files)
	{
		Eval::CPack pack(files.size());
		for (const auto& file : files)
			pack.Add(CreatePattern(file));
		return pack;
	}

	Eval::CPack CreatePack(const CPath& prefix, const std::vector<std::string>& infix, const std::string& suffix)
	{
		std::vector<CPath> paths;
		paths.reserve(infix.size());
		for (const auto& it : infix)
			paths.emplace_back(prefix.GetAbsoluteFilePath() + it + suffix);
		return CreatePack(paths);
	}



	CNameLookupTable::CNameLookupTable(std::vector<std::string> names, std::vector<uint64_t> patterns)
		: m_names(std::move(names)), m_patterns(std::move(patterns))
	{}

	CNameLookupTable::CNameLookupTable(std::iostream& stream)
	{
		Read(stream);
	}

	CNameLookupTable::CNameLookupTable(const CPath& file)
	{
		std::fstream fstream(file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		Read(fstream);
	}

	void CNameLookupTable::Add(std::string name, uint64_t pattern)
	{
		m_names.push_back(std::move(name));
		m_patterns.push_back(pattern);
	}

	bool CNameLookupTable::Has(const std::string& name) const
	{
		return Find(name) != std::cend(m_names);
	}

	bool CNameLookupTable::Has(uint64_t pattern) const
	{
		return Find(pattern) != std::cend(m_patterns);
	}

	uint64_t CNameLookupTable::Get(const std::string& name) const
	{
		const auto it = Find(name);
		return m_patterns[std::distance(std::cbegin(m_names), it)];
	}

	std::string CNameLookupTable::Get(uint64_t pattern) const
	{
		const auto it = Find(pattern);
		return m_names[std::distance(std::cbegin(m_patterns), it)];
	}

	uint64_t CNameLookupTable::Translate(const std::string& name) const
	{
		if (Has(name))
			return Get(name);
				
		auto ull = std::stoull(name, nullptr, 16);
		if (ull != 0)
			return ull;
			
		throw std::runtime_error("Invalid pattern name");
	}

	void CNameLookupTable::Read(std::iostream& stream)
	{
		if (!stream.good())
			return;
		if (stream.peek() == EOF)
			return;

		const std::string delimiter = " = ";
		std::string line;
		while (std::getline(stream, line))
		{
			const std::size_t begin_of_delimiter = line.find(delimiter);
			const std::size_t end_of_delimiter = begin_of_delimiter + delimiter.length();
			const std::size_t end_of_value = line.find('\r', end_of_delimiter);

			std::string name = line.substr(0, begin_of_delimiter);
			std::string pattern = line.substr(end_of_delimiter, end_of_value - end_of_delimiter);

			m_names.push_back(std::move(name));
			m_patterns.push_back(std::stoull(pattern, nullptr, 16));
		}
	}

	std::vector<std::string>::const_iterator CNameLookupTable::Find(const std::string& name) const
	{
		return std::find(std::cbegin(m_names), std::cend(m_names), name);
	}

	std::vector<uint64_t>::const_iterator CNameLookupTable::Find(uint64_t pattern) const
	{
		return std::find(std::cbegin(m_patterns), std::cend(m_patterns), pattern);
	}



	CFactory::CFactory(CNameLookupTable names)
		: m_names(std::move(names))
	{}

	CWeights CFactory::CreateDefaultWeight(const std::string & name) const
	{
		const uint64_t pattern = Translate(name);
		return Configurations::CreateDefaultWeight(pattern);
	}

	std::unique_ptr<Eval::CBase> CFactory::CreatePattern(const std::string& name, const CWeights& compressed_weights) const
	{
		const uint64_t pattern = Translate(name);
		return Eval::CreatePattern(pattern, compressed_weights);
	}

	std::unique_ptr<Eval::CBase> CFactory::CreatePattern(const std::string& name, const CPath& file) const
	{
		const uint64_t pattern = Translate(name);
		CWeights compressed_weights = ::IO::read_vector<float>(file);
		return Eval::CreatePattern(pattern, compressed_weights);
	}

	std::vector<uint8_t> ParseEmpties(const std::string& str)
	{
		const auto vec = split(str, " ");
		std::vector<uint8_t> ret;
		ret.reserve(vec.size());
		for (const auto& it : vec)
			ret.push_back(static_cast<uint8_t>(std::stoi(it)));
		return ret;
	}
	
	Eval::CEnsemble CFactory::CreateEnsemble(const CPath& file) const
	{
		std::fstream stream(file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		if (!stream.good())
			throw std::runtime_error("stream is not good");
		if (stream.peek() == EOF)
			throw std::runtime_error("stream reached EOF");

		Eval::CEnsemble ensemble;
		std::string line;
		while (std::getline(stream, line))
		{
			const std::size_t begin = line.find('{');
			const std::size_t end = line.find('}');
			const std::string str_depths = line.substr(begin + 1, end - begin - 1);
			const auto empties = ParseEmpties(str_depths);

			const std::size_t end_of_line = line.find('\r', end);
			const std::string remainder = line.substr(end + 2, end_of_line - end - 2);
			auto vec = split(remainder, " ");
			const std::string stencil = vec[0];
			vec.erase(vec.begin());

			auto pack = std::make_shared<Eval::CPack>();
			for (const auto& it : vec)
			{
				std::string compressed_weights = stencil;
				replace_all(compressed_weights, "*", it);
				pack->Add(CreatePattern(it, file.GetAbsoluteFolderPath() + compressed_weights));
			}

			for (const auto& e : empties)
				ensemble.Set(e, pack);
		}
		return ensemble;
	}

	uint64_t CFactory::Translate(const std::string & name) const
	{
		if (m_names.Has(name))
			return m_names.Get(name);
		return std::stoull(name, nullptr, 16);
	}

	std::shared_ptr<Pattern::Eval::CEnsemble> CreateEnsemble(const CPath& names, const CPath& use)
	{
		Pattern::IO::CNameLookupTable pattern_lookup_table(names);
		Pattern::IO::CFactory pattern_factory(pattern_lookup_table);
		return std::make_shared<Pattern::Eval::CEnsemble>(pattern_factory.CreateEnsemble(use));
	}
}
