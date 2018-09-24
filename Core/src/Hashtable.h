#pragma once
#include <cassert>
#include <cstdint>
#include <atomic>
#include <iostream>
#include "Position.h"

template <typename KeyType, typename ValueType>
class IHashTable
{
public:
	virtual ~IHashTable() {}
	virtual void Update(const KeyType& key, const ValueType& value) = 0;
	virtual std::pair<bool, ValueType> LookUp(const KeyType& key) const = 0;
	virtual void Refresh(const KeyType& key) = 0;
	virtual void AdvanceDate() = 0;
	virtual void Clear() = 0;
};

template <typename NodeType, typename KeyType, typename ValueType>
class HashTable : public IHashTable<KeyType, ValueType>
{
public:
	typedef NodeType nodetype;
	typedef KeyType keytype;
	typedef ValueType valuetype;

	HashTable(uint64_t Buckets);
	HashTable() : HashTable(1) {}

	void Update(const KeyType& key, const ValueType& value) override;
	std::pair<bool, ValueType> LookUp(const KeyType& key) const override;
	void Refresh(const KeyType& key) override;
	void AdvanceDate() override;
	void Clear() override;
	void PrintStatistics();

private:
	std::vector<NodeType> table;
	uint8_t date;
	mutable std::atomic<uint64_t> UpdateCounter, LookUpCounter, RefreshCounter, HitCounter;

	uint64_t OptimizedBucketSize(uint64_t Buckets);
	std::size_t Hash(const KeyType& key) const;
};


template <typename NodeType, typename KeyType, typename ValueType>
inline HashTable<NodeType, KeyType, ValueType>::HashTable(uint64_t Buckets)
	: date(0)
	, UpdateCounter(0)
	, LookUpCounter(0)
	, RefreshCounter(0)
	, HitCounter(0)
{
	table = std::vector<NodeType>(OptimizedBucketSize(Buckets));
}

template <typename NodeType, typename KeyType, typename ValueType>
inline void HashTable<NodeType, KeyType, ValueType>::Update(const KeyType& key, const ValueType& value)
{
	UpdateCounter++;
	table[Hash(key)].Update(key, value, date);
}

template <typename NodeType, typename KeyType, typename ValueType>
inline std::pair<bool, ValueType> HashTable<NodeType, KeyType, ValueType>::LookUp(const KeyType& key) const
{
	LookUpCounter++;
	const auto ret = table[Hash(key)].LookUp(key);
	if (ret.first)
		HitCounter++;
	return ret;
}

template <typename NodeType, typename KeyType, typename ValueType>
inline void HashTable<NodeType, KeyType, ValueType>::Refresh(const KeyType& key)
{
	RefreshCounter++;
	table[Hash(key)].Refresh(key, date);
}

template <typename NodeType, typename KeyType, typename ValueType>
inline void HashTable<NodeType, KeyType, ValueType>::AdvanceDate()
{
	date++;
}

template <typename NodeType, typename KeyType, typename ValueType>
inline void HashTable<NodeType, KeyType, ValueType>::Clear()
{
	UpdateCounter = 0;
	LookUpCounter = 0;
	RefreshCounter = 0;
	HitCounter = 0;

	for (auto& it : table)
		it.Clear();
}

template <typename NodeType, typename KeyType, typename ValueType>
inline void HashTable<NodeType, KeyType, ValueType>::PrintStatistics()
{
	uint64_t counter[3] = { 0,0,0 };
	for (const auto& it : table)
		counter[it.NumberOfNonEmptyNodes()]++;
	float total = static_cast<float>(counter[0] + counter[1] + counter[2]);

	std::cout
		<< "Size:     " << ThousandsSeparator(sizeof(table)) << " Bytes\n"
		<< "Updates:  " << ThousandsSeparator(UpdateCounter) << "\n"
		<< "LookUps:  " << ThousandsSeparator(LookUpCounter) << "\n"
		<< "Refreshs: " << ThousandsSeparator(RefreshCounter) << "\n"
		<< "Hits:     " << ThousandsSeparator(HitCounter) << "\n"
		<< "Hit rate: " << static_cast<float>(HitCounter * 100) / static_cast<float>(LookUpCounter) << "\n"
		<< "Zero entry nodes : " << counter[0] / total * 100 << "\n"
		<< "One  entry nodes : " << counter[1] / total * 100 << "\n"
		<< "Two  entry nodes : " << counter[2] / total * 100 << "\n";
}

template <typename NodeType, typename KeyType, typename ValueType>
inline uint64_t HashTable<NodeType, KeyType, ValueType>::OptimizedBucketSize(uint64_t Buckets)
{
	assert(Buckets > 0);

	// Exits for Buckets == 1.
	// Buckets either becomes 1 or exits bigger.
	while ((Buckets % 2 == 0) || (Buckets % 3 == 0) || (Buckets % 5 == 0))
		Buckets--;

	return Buckets;
}