#pragma once
#include "Utility.h"
#include "ConfigFile.h"
#include "Position.h"
#include "Environment.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>

class CPattern
{
	static std::vector<uint32_t> m_sumpow3_cache;
protected:
	static uint32_t FullPatternIndex(const CPosition& pos, const uint64_t mask);
	static uint32_t SumPow3(const uint64_t index);
public:
	static void Initialize();
	static void For_each_configuration_in_pattern_do_fkt(const uint64_t pattern, std::function<void(const CPosition&)> fkt);

protected:
	const uint64_t Pattern;
	const uint32_t m_FullSize, m_ReducedSize, m_Occurrences;

public:
	CPattern(uint64_t Pattern, uint32_t FullSize, uint32_t ReducedSize, uint32_t Occurrences);
	virtual ~CPattern() {}
	
	uint64_t GetPattern() const { return Pattern; }
	uint32_t Occurrences() const { return m_Occurrences; }
	uint32_t FullSize() const { return m_FullSize; }
	uint32_t ReducedSize() const { return m_ReducedSize; }

	virtual void set_weights() = 0;
	virtual void set_weights(const std::vector<float>& compressed_weights) = 0;

	virtual float Eval(const CPosition&) const = 0;
	virtual std::vector<float> GetScores(const CPosition&) const = 0;
	virtual std::vector<uint32_t> GetConfigurations(const CPosition&) const = 0;
};

class CPatternH : public CPattern
{
	static const uint64_t HALF = 0x0F0F0F0F0F0F0F0FULL;
	static const uint64_t MID  = 0x0000001818000000ULL;
	const uint64_t PatternC, PatternV, PatternD;
	const uint32_t halfSize;
	std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

public:
	CPatternH(uint64_t Pattern);

	void set_weights() override;
	void set_weights(const std::vector<float>& compressed_weights) override;

	float Eval(const CPosition&) const override;
	std::vector<float> GetScores(const CPosition&) const override;
	std::vector<uint32_t> GetConfigurations(const CPosition&) const override;
private:
	uint32_t ReducedPatternIndex0(CPosition) const;
	uint32_t ReducedPatternIndex1(CPosition) const;
	uint32_t ReducedPatternIndex2(CPosition) const;
	uint32_t ReducedPatternIndex3(CPosition) const;
};

class CPatternD : public CPattern
{
	static const uint64_t HALF = 0x0080C0E0F0F8FCFEULL;
	static const uint64_t DIAG = 0x8040201008040201ULL;
	static const uint64_t MID  = 0x0000001818000000ULL;
	const uint64_t PatternH, PatternC, PatternV;
	const uint32_t halfSize, diagSize;
	std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

public:
	CPatternD(uint64_t Pattern);

	void set_weights() override;
	void set_weights(const std::vector<float>& compressed_weights) override;

	float Eval(const CPosition&) const override;
	std::vector<float> GetScores(const CPosition&) const override;
	std::vector<uint32_t> GetConfigurations(const CPosition&) const override;
private:
	uint32_t ReducedPatternIndex0(CPosition) const;
	uint32_t ReducedPatternIndex1(CPosition) const;
	uint32_t ReducedPatternIndex2(CPosition) const;
	uint32_t ReducedPatternIndex3(CPosition) const;
};

class CPattern0 : public CPattern
{
	static const uint64_t MID = 0x0000001818000000ULL;
	const uint64_t PatternH, PatternV, PatternD, PatternC, PatternHV, PatternHD, PatternHC;
	std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

public:
	CPattern0(uint64_t Pattern);

	void set_weights() override;
	void set_weights(const std::vector<float>& compressed_weights) override;

	float Eval(const CPosition&) const override;
	std::vector<float> GetScores(const CPosition&) const override;
	std::vector<uint32_t> GetConfigurations(const CPosition&) const override;
private:
	uint32_t ReducedPatternIndex0(CPosition) const;
	uint32_t ReducedPatternIndex1(CPosition) const;
	uint32_t ReducedPatternIndex2(CPosition) const;
	uint32_t ReducedPatternIndex3(CPosition) const;
	uint32_t ReducedPatternIndex4(CPosition) const;
	uint32_t ReducedPatternIndex5(CPosition) const;
	uint32_t ReducedPatternIndex6(CPosition) const;
	uint32_t ReducedPatternIndex7(CPosition) const;
};

// Pattern in a group belong together and shoud never be evaluated separately.
class CPatternGroup
{
	std::vector<std::unique_ptr<CPattern>> m_group;
public:
	CPatternGroup(std::vector<std::unique_ptr<CPattern>> group) : m_group(std::move(group)) {}
	virtual ~CPatternGroup() {}

	float Eval(const CPosition&) const;
	std::vector<std::vector<uint32_t>> GetConfigurations(const CPosition&) const;
};

// Holds a PatternGroup for every Position.
class CPatternCollection : public IPattern
{
	std::vector<std::shared_ptr<CPatternGroup>> m_collection;
public:
	CPatternCollection(std::vector<std::shared_ptr<CPatternGroup>> collection);
	virtual ~CPatternCollection() {}

	float Eval(const CPosition&) const override;
	std::vector<std::vector<uint32_t>> GetConfigurations(const CPosition&) const;
};

std::unique_ptr<CPattern> CreatePattern(const uint64_t pattern);