#pragma once
#include <cassert>
#include <cstdint>
#include <atomic>
#include <iostream>
#include "MacrosHell.h"
#include "Utility.h"
#include "Moves.h"
#include "Position.h"
#include "Hashtable.h"

class PvsInfo
{
public:
	uint8_t cost;
	 int8_t depth;
	uint8_t selectivity;
	 int8_t alpha;
	 int8_t beta;
	Field   PV;
	Field   AV;

	PvsInfo();
	PvsInfo(uint64_t NodeCount, int8_t depth, uint8_t selectivity, int8_t alpha, int8_t beta, CMove PV, CMove AV);

	void Upgrade(const PvsInfo& NewValue);

private:
	void SetCost(uint64_t NodeCount);
	uint64_t GetNodeCount() const;
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

	void Update (const CPosition& key, const PvsInfo& value, uint8_t date);
	bool LookUp (const CPosition& key, PvsInfo& value) const; // The return value is stored in 'value'.
	void Refresh(const CPosition& key, uint8_t date);
	void Clear();

	int NumberOfNonEmptyNodes() const;
};

typedef HashTable<TwoNode, CPosition, PvsInfo> HashTablePVS;

template <>
std::size_t HashTable<TwoNode, CPosition, PvsInfo>::Hash(const CPosition& key) const
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
	//return (P + O + (O << 41)) % buckets;
}