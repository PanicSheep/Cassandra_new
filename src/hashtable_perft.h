#pragma once
#include <atomic>
#include <cstdint>
#include <vector>

class BigNode
{
private:
	std::atomic<uint64_t> m_Value, m_P, m_O, m_depth;
public:
	BigNode() : m_Value(0), m_P(0), m_O(0), m_depth(0) {}

	void Update(const uint64_t P, const uint64_t O, const uint64_t depth, const uint64_t Value)
	{
		uint64_t V;
		while((V = m_Value.exchange(0xFFFFFFFFFFFFFFFULL, std::memory_order_acquire)) == 0xFFFFFFFFFFFFFFFULL) ; //lock
		if (Value > V)
		{
			m_P.store(P, std::memory_order_relaxed);
			m_O.store(O, std::memory_order_relaxed);
			m_depth.store(depth, std::memory_order_relaxed);
			V = Value;
		}
		m_Value.store(V, std::memory_order_release); //unlock
	}
	bool LookUp(const uint64_t P, const uint64_t O, const uint64_t depth, uint64_t & Value)
	{
		uint64_t V;
		bool KeyIsEqual;

		while((V = m_Value.exchange(0xFFFFFFFFFFFFFFFULL, std::memory_order_acquire)) == 0xFFFFFFFFFFFFFFFULL) ; //lock
		KeyIsEqual = (P == m_P.load(std::memory_order_relaxed)) && (O == m_O.load(std::memory_order_relaxed)) && (depth == m_depth.load(std::memory_order_relaxed));
		if (KeyIsEqual)
			Value = V;
		m_Value.store(V, std::memory_order_release); //unlock
		return KeyIsEqual;
	}
	void Clear()
	{
		while(m_Value.exchange(0xFFFFFFFFFFFFFFFULL, std::memory_order_acquire) == 0xFFFFFFFFFFFFFFFULL) ; //lock
		m_P.store(0, std::memory_order_relaxed);
		m_O.store(0, std::memory_order_relaxed);
		m_depth.store(0, std::memory_order_relaxed);
		m_Value.store(0, std::memory_order_release); //unlock
	}
};

class CHashTable
{
	typedef BigNode NodeType;
	std::vector<NodeType> table;
	const uint64_t buckets;
	inline uint64_t bucketSize(uint64_t Buckets) { while ((Buckets % 2 == 0) || (Buckets % 3 == 0) || (Buckets % 5 == 0)) Buckets--; return Buckets; }
public:
	CHashTable(const uint64_t Buckets) : buckets(bucketSize(Buckets)) { table = std::vector<NodeType>(buckets); }
	CHashTable() : CHashTable(1) {}
	inline void Update(const uint64_t P, const uint64_t O, const uint64_t depth, const uint64_t Value){ table[Hash(P, O)].Update(P, O, depth, Value); }
	inline bool LookUp(const uint64_t P, const uint64_t O, const uint64_t depth, uint64_t & Value){ return table[Hash(P, O)].LookUp(P, O, depth, Value); }
	inline void Clear() { for (auto& it : table) it.Clear(); }
	inline uint64_t size() { return sizeof(NodeType) * buckets; }
private:
	inline std::size_t Hash(uint64_t P, uint64_t O) const
	{
		P ^= P >> 36;
		O ^= O >> 21;
		return (P * O) % buckets;
	}
};