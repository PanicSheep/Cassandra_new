#include "HashtablePVS.h"
#include "MacrosHell.h"
#include <algorithm>

PvsInfo::PvsInfo(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves, uint64_t node_count)
	: min(min)
	, max(max)
	, depth(depth)
	, selectivity(selectivity)
	, cost(static_cast<uint8_t>(BitScanMSB(node_count + 1))) // +1 prevents it from failing at node_count == 0.
	, best_moves(best_moves)
{}



TwoNode::TwoNode(Node node1, Node node2)
	: node1(std::move(node1))
	, node2(std::move(node2))
{}

TwoNode::TwoNode(const TwoNode& o)
{
	std::lock_guard<SpinlockMutex> lock{ o.mutex };
	node1 = o.node1;
	node2 = o.node2;
}

TwoNode& TwoNode::operator=(const TwoNode& o)
{
	if (&o == this)
		return *this;
	std::lock_guard<SpinlockMutex> my_lock{ mutex };
	std::lock_guard<SpinlockMutex> o_lock{ o.mutex };
	node1 = o.node1;
	node2 = o.node2;
	return *this;
}

void TwoNode::Update(const CPosition& key, const PvsInfo& novum)
{
	std::lock_guard<SpinlockMutex> lock{ mutex };

	//if (key == node1.key)
	//	node1 = Node(key, novum, date);
	//else if (novum.cost > node1.novum.cost)
	//	node1 = Node(key, novum, date);

	if (key == node1.key)
	{
		const bool novum_is_exact = (novum.depth == key.EmptyCount()) && (novum.selectivity == 0);
		const bool node1_is_exact = (node1.value.depth == key.EmptyCount()) && (node1.value.selectivity == 0);
		if (novum_is_exact && node1_is_exact)
		{
			assert(std::max(node1.value.min, novum.min) <= std::min(node1.value.max, novum.max));

			node1.value.best_moves = Merge(
				node1.value.best_moves, node1.value.depth, node1.value.selectivity,
				novum.best_moves, novum.depth, novum.selectivity);
			node1.value.cost = novum.cost + node1.value.cost;
			if (novum.min > node1.value.min) node1.value.min = novum.min;
			if (novum.max < node1.value.max) node1.value.max = novum.max;
		}
		else if ((novum.depth >= node1.value.depth) && (novum.selectivity <= node1.value.selectivity))
			node1 = Node(key, novum);
		else if (novum.cost > node1.value.cost)
			node1 = Node(key, novum);
	}
	else if (key == node2.key)
	{
		const bool novum_is_exact = (novum.depth == key.EmptyCount()) && (novum.selectivity == 0);
		const bool node2_is_exact = (node2.value.depth == key.EmptyCount()) && (node2.value.selectivity == 0);
		if (novum_is_exact && node2_is_exact)
		{
			assert(std::max(node2.value.min, novum.min) <= std::min(node2.value.max, novum.max));

			node2.value.best_moves = Merge(
				node2.value.best_moves, node2.value.depth, node2.value.selectivity,
				novum.best_moves, novum.depth, novum.selectivity);
			node2.value.cost = novum.cost + node2.value.cost;
			if (novum.min > node2.value.min) node2.value.min = novum.min;
			if (novum.max < node2.value.max) node2.value.max = novum.max;
		}
		else if ((novum.depth >= node2.value.depth) && (novum.selectivity <= node2.value.selectivity))
			node2 = Node(key, novum);
		else if (novum.cost > node2.value.cost)
			node2 = Node(key, novum);
	}
	else if (novum.cost > node1.value.cost) { // TODO: This should be subject to a broad range of tests.
		node2 = node1;
		node1 = Node(key, novum);
	}
	else if (novum.cost > node2.value.cost) { // TODO: This should be subject to a broad range of tests.
		node2 = Node(key, novum);
	}
}

std::optional<PvsInfo> TwoNode::LookUp(const CPosition& key) const
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	if (key == node1.key)
		return node1.value;
	if (key == node2.key)
		return node2.value;
	return {};
}

void TwoNode::Clear()
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	node1 = Node();
	node2 = Node();
}

int TwoNode::NumberOfNonEmptyNodes() const
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	int sum = 0;
	if (node1.key != CPosition()) sum++;
	if (node2.key != CPosition()) sum++;
	return sum;
}




OneNode& OneNode::operator=(const OneNode& o)
{
	if (&o == this)
		return *this;
	std::lock_guard<SpinlockMutex> my_lock{ mutex };
	std::lock_guard<SpinlockMutex> o_lock{ o.mutex };
	node = o.node;
	return *this;
}

void OneNode::Update(const CPosition& key, const PvsInfo& novum)
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	if (key == node.key)
	{
		const bool novum_is_exact = (novum.depth == key.EmptyCount()) && (novum.selectivity == 0);
		const bool node_is_exact = (node.value.depth == key.EmptyCount()) && (node.value.selectivity == 0);
		if (novum_is_exact && node_is_exact)
		{
			assert(std::max(node.value.min, novum.min) <= std::min(node.value.max, novum.max));

			node.value.best_moves = Merge(
				node.value.best_moves, node.value.depth, node.value.selectivity,
				novum.best_moves, novum.depth, novum.selectivity);
			node.value.cost = novum.cost + node.value.cost;
			if (novum.min > node.value.min) node.value.min = novum.min;
			if (novum.max < node.value.max) node.value.max = novum.max;
			return;
		}
	}

	node = Node(key, novum);
}

std::optional<PvsInfo> OneNode::LookUp(const CPosition& key) const
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	if (key == node.key)
		return node.value;
	return {};
}

void OneNode::Clear()
{
	std::lock_guard<SpinlockMutex> lock{ mutex };
	node = Node();
}

int OneNode::NumberOfNonEmptyNodes() const
{
	return (node.key == CPosition()) ? 0 : 1;
}
