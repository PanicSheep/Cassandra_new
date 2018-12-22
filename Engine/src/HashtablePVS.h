#pragma once
#include "Hashtable.h"
#include "Moves.h"
#include "Position.h"
#include <atomic>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>

class PvsInfo
{
public:
	int8_t min = -65;
	int8_t max = +65;
	int8_t depth = -1;
	uint8_t selectivity = 99;
	uint8_t cost = 0;
	CBestMoves best_moves{};

	PvsInfo() = default;
	PvsInfo(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves, uint64_t node_count);

	void Upgrade(const PvsInfo&);
};

class Node
{
	static const uint8_t OLD_AGE = 6;
public:
	CPosition key;
	PvsInfo value;
	uint8_t date{0};

	Node() = default;
	Node(const CPosition& key, const PvsInfo& value, uint8_t date);

	void Upgrade(const PvsInfo& novum, uint8_t date);
	void Refresh(const CPosition& key, uint8_t date);
	bool IsOld(uint8_t CompareDate) const;
};

class /*alignas(CACHE_LINE_SIZE)*/ TwoNode
{
	mutable std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
	Node node1, node2;

	void lock() const; // blocking
	void unlock() const;
public:
	TwoNode();
	TwoNode(Node node1, Node node2);
	TwoNode(const TwoNode&);

	TwoNode& operator=(const TwoNode&);

	void Update(const CPosition& key, const PvsInfo& value, uint8_t date);
	std::optional<PvsInfo> LookUp(const CPosition& key) const;
	void Refresh(const CPosition& key, uint8_t date);
	void Clear();

	int NumberOfNonEmptyNodes() const;
};

//static_assert(sizeof(TwoNode) <= CACHE_LINE_SIZE);

typedef HashTable<TwoNode, CPosition, PvsInfo> CHashTablePVS;

template <>
inline std::size_t HashTable<TwoNode, CPosition, PvsInfo>::Hash(const CPosition& key) const
{
	uint64_t P = key.GetP();
	uint64_t O = key.GetO();
	P ^= P >> 36;
	O ^= O >> 21;
	return (P * O) % table.size();

	//P ^= P >> 33;
	//O ^= O >> 33;
	//P *= 0xFF14AFD7ED558CCDui64;
	//O *= 0xFF14AFD7ED558CCDui64;
	//O ^= O >> 33;
	//return (P + O + (O << 41)) % table.size();
}