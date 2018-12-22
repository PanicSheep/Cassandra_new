#pragma once
#include "Path.h"
#include "Pattern.h"

namespace Pattern::IO
{
	std::unique_ptr<Pattern::Eval::CBase> CreatePattern(std::iostream&);
	std::unique_ptr<Pattern::Eval::CBase> CreatePattern(const CPath&);

	Eval::CPack CreatePack(const std::vector<CPath>&);
	Eval::CPack CreatePack(const CPath& prefix, const std::vector<std::string>& infix, const std::string& suffix);

	class CNameLookupTable
	{
		std::vector<std::string> m_names;
		std::vector<uint64_t> m_patterns;
	public:
		CNameLookupTable() = default;
		CNameLookupTable(std::vector<std::string> names, std::vector<uint64_t> patterns);
		CNameLookupTable(std::iostream&);
		CNameLookupTable(const CPath&);

		void Add(std::string, uint64_t);

		bool Has(const std::string&) const;
		bool Has(uint64_t) const;

		uint64_t Get(const std::string&) const;
		std::string Get(uint64_t) const;

		uint64_t Translate(const std::string&) const;
	private:
		void Read(std::iostream&);
		std::vector<std::string>::const_iterator Find(const std::string&) const;
		std::vector<uint64_t>::const_iterator Find(uint64_t) const;
	};

	class CFactory
	{
		CNameLookupTable m_names;
		std::array<std::shared_ptr<Pattern::Eval::CPack>, 61> m_packs;
	public:
		CFactory() = default;
		CFactory(CNameLookupTable);

		CWeights CreateDefaultWeight(const std::string& name) const;

		std::unique_ptr<Pattern::Eval::CBase> CreatePattern(const std::string& name, const CWeights& compressed_weights) const;
		std::unique_ptr<Pattern::Eval::CBase> CreatePattern(const std::string& name, const CPath& file) const;
		
		Eval::CEnsemble CreateEnsemble(const CPath&) const;

		uint64_t Translate(const std::string& name) const;
	};

	std::shared_ptr<Pattern::Eval::CEnsemble> CreateEnsemble(const CPath& names, const CPath& use);
}