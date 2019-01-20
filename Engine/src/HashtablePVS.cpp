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

void PvsInfo::Upgrade(const PvsInfo& novum)
{
	if ((novum.depth > depth) || (novum.depth == depth && novum.selectivity < selectivity))
	{	// Evaluation is better
		*this = novum;
	}
	else if ((novum.depth == depth) && (novum.selectivity == selectivity))
	{	// Evaluation is the same
		if (selectivity == 0)
		{
			assert(std::max(min, novum.min) <= std::min(max, novum.max));

			if (novum.cost > cost) cost = novum.cost;
			if (novum.min > min) min = novum.min;
			if (novum.max < max) max = novum.max;
			if (novum.best_moves.PV != Field::invalid)
				best_moves = novum.best_moves;
		}
		else if (novum.cost > cost)
			*this = novum;
	}
}


Node::Node(const CPosition & key, const PvsInfo & value, const uint8_t date)
	: key(key), value(value), date(date)
{}

void Node::Upgrade(const PvsInfo& novum, const uint8_t date)
{
	value.Upgrade(novum);
	this->date = date;
}

void Node::Refresh(const CPosition&, uint8_t date)
{
	this->date = date;
}

bool Node::IsOld(const uint8_t CompareDate) const
{
	return CompareDate - date >= OLD_AGE;
}


void TwoNode::lock() const
{
	while (spinlock.test_and_set(std::memory_order_acquire))
		continue;
}

void TwoNode::unlock() const
{
	spinlock.clear(std::memory_order_release);
}

TwoNode::TwoNode()
{
	unlock();
}

TwoNode::TwoNode(Node node1, Node node2)
	: node1(node1)
	, node2(node2)
{
	unlock();
}

TwoNode::TwoNode(const TwoNode& o)
{
	o.lock();

	node1 = o.node1;
	node2 = o.node2;

	o.unlock();
	unlock();
}

TwoNode& TwoNode::operator=(const TwoNode& o)
{
	if (&o == this)
		return *this;

	o.lock();
	lock();

	node1 = o.node1;
	node2 = o.node2;

	unlock();
	o.unlock();

	return *this;
}

void TwoNode::Update(const CPosition& key, const PvsInfo& value, const uint8_t date)
{
	lock();

	if (key == node1.key)
	{
		node1.Upgrade(value, date);
	}
	else if (key == node2.key)
	{
		node2.Upgrade(value, date);
	}
	else if (value.cost > node1.value.cost) { // TODO: This should be subject to a broad range of tests.
		node2 = node1;
		node1 = Node(key, value, date);
	}
	else if (value.cost > node2.value.cost) { // TODO: This should be subject to a broad range of tests.
		node2 = Node(key, value, date);
	}
	else // TODO: This should be subject to a broad range of tests
	{
		const bool IsOld1 = node1.IsOld(date);
		const bool IsOld2 = node2.IsOld(date);
		if (IsOld1 && IsOld2) {
			node2 = node1;
			node1 = Node(key, value, date);
		}
		else if (!IsOld1 && IsOld2) {
			node2 = Node(key, value, date);
		}
		else if (IsOld1 && !IsOld2) {
			node1 = node2;
			node2 = Node(key, value, date);
		}
	}

	unlock();
}

std::optional<PvsInfo> TwoNode::LookUp(const CPosition& key) const
{
	lock();

	if (key == node1.key)
	{
		const auto value = node1.value;
		unlock();
		return value;
	}

	if (key == node2.key)
	{
		const auto value = node2.value;
		unlock();
		return value;
	}

	unlock();
	return {};
}

void TwoNode::Refresh(const CPosition& key, const uint8_t date)
{
	lock();

	     if (key == node1.key) node1.date = date;
	else if (key == node2.key) node2.date = date;

	unlock();
}

void TwoNode::Clear()
{
	lock();

	node1 = Node();
	node2 = Node();

	unlock();
}

int TwoNode::NumberOfNonEmptyNodes() const
{
	int ret = 0;
	if (node1.key != CPosition()) ret++;
	if (node2.key != CPosition()) ret++;
	return ret;
}
