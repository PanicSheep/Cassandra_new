#pragma once
#include "Hashtable.h"
#include "Moves.h"
#include "Position.h"
#include <atomic>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <new>
#include <optional>

struct PvsInfo
{
	int8_t min = -65;
	int8_t max = +65;
	int8_t depth = -1;
	uint8_t selectivity = 99;
	uint8_t cost = 0;
	CBestMoves best_moves{};

	PvsInfo() = default;
	PvsInfo(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves, uint64_t node_count);
};

struct Node
{
	CPosition key;
	PvsInfo value;

	Node() = default;
	Node(const CPosition& key, const PvsInfo& value) : key(key), value(value) {}
};

class SpinlockMutex
{
	std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
public:
	void lock() { while (spinlock.test_and_set(std::memory_order_acquire)) continue; }
	void unlock() { spinlock.clear(std::memory_order_release); }
};

class OneNode
{
	mutable SpinlockMutex mutex;
	Node node;

public:
	OneNode() = default;
	OneNode(const OneNode&);

	OneNode& operator=(const OneNode&);

	void Update(const CPosition&, const PvsInfo&);
	std::optional<PvsInfo> LookUp(const CPosition&) const;
	void Clear();

	int NumberOfNonEmptyNodes() const;
};

class alignas(std::hardware_destructive_interference_size) TwoNode
{
	mutable SpinlockMutex mutex;
	Node node1, node2;

public:
	TwoNode() = default;
	TwoNode(Node node1, Node node2);
	TwoNode(const TwoNode&);

	TwoNode& operator=(const TwoNode&);

	void Update(const CPosition&, const PvsInfo&);
	std::optional<PvsInfo> LookUp(const CPosition&) const;
	void Clear();

	int NumberOfNonEmptyNodes() const;
};

static_assert(sizeof(TwoNode) <= std::hardware_constructive_interference_size);

struct CHashTablePVS : public HashTable<CPosition, PvsInfo, TwoNode>
{
	CHashTablePVS(uint64_t buckets)
		: HashTable(buckets, [](const keytype& key) {
			uint64_t P = key.GetP();
			uint64_t O = key.GetO();
			P ^= P >> 36;
			O ^= O >> 21;
			return P * O;
			})
	{}
};