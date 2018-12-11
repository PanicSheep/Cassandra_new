#include "HashtablePVS.h"
#include <algorithm>
#include "MacrosHell.h"

PvsInfo::PvsInfo()
	: cost(0)
	, depth(-1)
	, selectivity(0)
	, min(-65)
	, max(+65)
	, PV(Field::invalid)
	, AV(Field::invalid)
{}

PvsInfo::PvsInfo(uint64_t NodeCount, int8_t depth, uint8_t selectivity, int8_t min, int8_t max, CMove PV, CMove AV)
	: depth(depth)
	, selectivity(selectivity)
	, min(min)
	, max(max)
	, PV(PV)
	, AV(AV)
{
	SetCost(NodeCount);
}

void PvsInfo::Upgrade(const PvsInfo& NewValue)
{
	if ((NewValue.depth > depth) || (NewValue.depth == depth && NewValue.selectivity < selectivity))
	{	// Evaluation is better
		*this = NewValue;
	}
	else if ((NewValue.depth == depth) && (NewValue.selectivity == selectivity))
	{	// Evaluation is the same
		assert(std::max(min, NewValue.min) <= std::min(max, NewValue.max));

		// TODO: This should be subject to a broad range of tests.
		//cost = std::max(cost, NewValue.cost);
		SetCost(NodeCount() + NewValue.NodeCount());
		min = std::max(min, NewValue.min);
		max = std::min(max, NewValue.max);
		if (NewValue.PV != Field::invalid) PV = NewValue.PV;
		if (NewValue.AV != Field::invalid) AV = NewValue.AV;
	}
}

void PvsInfo::SetCost(uint64_t NodeCount)
{
	cost = static_cast<int8_t>(BitScanMSB(NodeCount + 1)); // +1 prevents it from failing at NodeCount == 0.
}

uint64_t PvsInfo::NodeCount() const
{
	return MakeBit(cost);
}


Node::Node()
	: date(0)
{}

Node::Node(const CPosition & key, const PvsInfo & value, const uint8_t date)
	: key(key), value(value), date(date)
{}

void Node::Upgrade(const PvsInfo& NewValue, const uint8_t date)
{
	value.Upgrade(NewValue);
	this->date = date;
}

void Node::Refresh(const CPosition& key, uint8_t date)
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
	: node1(std::move(node1))
	, node2(std::move(node2))
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