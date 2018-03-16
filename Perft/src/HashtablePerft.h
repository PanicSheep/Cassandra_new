#pragma once
#include <atomic>
#include <cstdint>
#include "Hashtable.h"

class PerftKey
{
public:
	CPosition pos;
	uint64_t depth;

	PerftKey(const CPosition& pos, uint64_t depth) : pos(pos), depth(depth) {}
};

class BigNode
{
public:
	BigNode() : m_Value(0), m_P(0), m_O(0), m_depth(0) {}

	void Update(const PerftKey& key, const uint64_t value, uint8_t date)
	{
		const uint64_t old_value = lock();
		if (value > old_value)
		{
			m_P.store(key.pos.GetP(), std::memory_order_relaxed);
			m_O.store(key.pos.GetO(), std::memory_order_relaxed);
			m_depth.store(key.depth, std::memory_order_relaxed);
			unlock(value);
		}
		else
			unlock(old_value);
	}

	std::pair<bool, uint64_t> LookUp(const PerftKey& key) const
	{
		const uint64_t old_value = lock();
		const bool KeyIsEqual = (key.pos.GetP() == m_P.load(std::memory_order_relaxed))
			      && (key.pos.GetO() == m_O.load(std::memory_order_relaxed))
			      && (key.depth  == m_depth.load(std::memory_order_relaxed));
		unlock(old_value);
		return std::pair<bool, uint64_t>(KeyIsEqual, old_value);
	}

	void Clear()
	{
		lock();
		m_P.store(0, std::memory_order_relaxed);
		m_O.store(0, std::memory_order_relaxed);
		m_depth.store(0, std::memory_order_relaxed);
		unlock(0);
	}
private:
	mutable std::atomic<uint64_t> m_Value;
	std::atomic<uint64_t> m_P, m_O, m_depth;

	uint64_t lock() const
	{
		uint64_t value;
		while ((value = m_Value.exchange(0xFFFFFFFFFFFFFFFULL, std::memory_order_acquire)) == 0xFFFFFFFFFFFFFFFULL)
			continue; 
		return value;
	}

	void unlock(uint64_t value) const 
	{
		m_Value.store(value, std::memory_order_release); 
	}
};

//class HashTable
//{
//public:
//	typedef BigNode NodeType;
//private:
//	std::vector<NodeType> table;
//	const uint64_t buckets;
//	uint64_t OptimizedBucketSize(uint64_t Buckets) { while ((Buckets % 2 == 0) || (Buckets % 3 == 0) || (Buckets % 5 == 0)) Buckets--; return Buckets; }
//public:
//	HashTable(const uint64_t Buckets) : buckets(OptimizedBucketSize(Buckets)) { table = std::vector<NodeType>(buckets); }
//	HashTable() : HashTable(1) {}
//	void Update(const CPosition& pos, const uint64_t depth, const uint64_t value){ table[Hash(pos)].Update(pos, depth, value); }
//	bool LookUp(const CPosition& pos, const uint64_t depth, uint64_t & value){ return table[Hash(pos)].LookUp(pos, depth, value); }
//	void Clear() { for (auto& it : table) it.Clear(); }
//	uint64_t size() { return sizeof(NodeType) * buckets; }
//private:
//	inline std::size_t Hash(const CPosition& pos) const
//	{
//		uint64_t P = pos.GetP();
//		uint64_t O = pos.GetO();
//		P ^= P >> 36;
//		O ^= O >> 21;
//		return (P * O) % buckets;
//	}
//};

typedef HashTable<BigNode, PerftKey, uint64_t> HashTablePerft;

template <>
std::size_t HashTable<BigNode, PerftKey, uint64_t>::Hash(const PerftKey& key) const
{
	uint64_t P = key.pos.GetP();
	uint64_t O = key.pos.GetO();
	P ^= P >> 36;
	O ^= O >> 21;
	return (P * O + key.depth) % table.size();
}