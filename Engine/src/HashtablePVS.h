#pragma once
#include <cassert>
#include <cstdint>
#include <atomic>
#include <iostream>
#include <memory>
#include "Moves.h"
#include "Position.h"
#include "Hashtable.h"

class PvsInfo
{
public:
	int8_t alpha;
	int8_t beta;
	int8_t depth;
	uint8_t selectivity;
	Field   PV;
	Field   AV;
	uint8_t cost;

	PvsInfo();
	PvsInfo(uint64_t NodeCount, int8_t depth, uint8_t selectivity, int8_t alpha, int8_t beta, CMove PV, CMove AV);

	void Upgrade(const PvsInfo& NewValue);

	uint64_t GetNodeCount() const;
private:
	void SetCost(uint64_t NodeCount);
};

class Node
{
	static const uint8_t OLD_AGE = 6;
public:
	CPosition key;
	PvsInfo value;
	uint8_t date;

	Node();
	Node(const CPosition& key, const PvsInfo& value, const uint8_t date);

	void Upgrade(const PvsInfo& NewValue, uint8_t date);
	void Refresh(const CPosition& key, uint8_t date);
	bool IsOld(uint8_t CompareDate) const;
};

class /*alignas(64)*/ TwoNode
{
	mutable std::atomic_flag spinlock;
	Node node1, node2;

	void lock() const; // blocking
	void unlock() const;
public:
	TwoNode();
	TwoNode(Node node1, Node node2);
	TwoNode(const TwoNode&);

	TwoNode& operator=(const TwoNode&);

	void Update(const CPosition& key, const PvsInfo& value, uint8_t date);
	std::pair<bool, PvsInfo> LookUp(const CPosition& key) const;
	void Refresh(const CPosition& key, uint8_t date);
	void Clear();

	int NumberOfNonEmptyNodes() const;
};

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
	//P *= 0xFF14AFD7ED558CCDULL;
	//O *= 0xFF14AFD7ED558CCDULL;
	//O ^= O >> 33;
	//return (P + O + (O << 41)) % table.size();
}