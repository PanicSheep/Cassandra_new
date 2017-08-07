#include "hashtable.h"

CHashTable gTT;
CHashTable gTTPV;

CTwoNode::CTwoNode(const CTwoNode& o)
{
	while (o.spinlock.test_and_set(std::memory_order_acquire)) ;
	
	m_P1 = o.m_P1;
	m_O1 = o.m_O1;
	m_P2 = o.m_P2;
	m_O2 = o.m_O2;
	m_value1 = o.m_value1;
	m_value2 = o.m_value2;
	m_date1 = o.m_date1;
	m_date2 = o.m_date2;
	
	o.spinlock.clear(std::memory_order_release);
	  spinlock.clear(std::memory_order_release);
}

CTwoNode::CTwoNode(CTwoNode&& o)
{
	m_P1 = o.m_P1;
	m_O1 = o.m_O1;
	m_P2 = o.m_P2;
	m_O2 = o.m_O2;
	m_value1 = o.m_value1;
	m_value2 = o.m_value2;
	m_date1 = o.m_date1;
	m_date2 = o.m_date2;
	
	spinlock.clear(std::memory_order_release);
}

CTwoNode& CTwoNode::operator=(CTwoNode o)
{
	while (spinlock.test_and_set(std::memory_order_acquire)) ;
	
	std::swap(m_P1, o.m_P1);
	std::swap(m_O1, o.m_O1);
	std::swap(m_P2, o.m_P2);
	std::swap(m_O2, o.m_O2);
	std::swap(m_value1, o.m_value1);
	std::swap(m_value2, o.m_value2);
	std::swap(m_date1, o.m_date1);
	std::swap(m_date2, o.m_date2);
	
	spinlock.clear(std::memory_order_release);
	return *this;
}

// Two positions are equal, but they might come with different informations
// Make the most out of both
inline void Upgrade(CHashTableValueType& Value_db, const CHashTableValueType& Value_new)
{
	if ((Value_new.depth > Value_db.depth) || (Value_new.depth == Value_db.depth && Value_new.selectivity < Value_db.selectivity))
	{	// Evaluation is better
		Value_db = Value_new; // Replace
	}
	else if ((Value_new.depth == Value_db.depth) && (Value_new.selectivity == Value_db.selectivity))
	{	// Evaluation is the same
		// TODO: This should be subject to a broad range of tests
		if (Value_new.cost > Value_db.cost) 		Value_db.cost = Value_new.cost;
		//else if (Value_new.cost == Value_db.cost)	Value_db.cost++;
		if (Value_new.alpha > Value_db.alpha) Value_db.alpha = Value_new.alpha;
		if (Value_new.beta  < Value_db.beta ) Value_db.beta  = Value_new.beta;
		if (Value_new.PV != 64) Value_db.PV = Value_new.PV;
		if (Value_new.AV != 64) Value_db.AV = Value_new.AV;
	}
}

void CTwoNode::Update(const uint64_t P, const uint64_t O, const CHashTableValueType& Value, const uint8_t date)
{
	while (spinlock.test_and_set(std::memory_order_acquire)) ;

	if (P == m_P1 && O == m_O1) {
		Upgrade(m_value1, Value);
		m_date1 = date;
	}
	else if (P == m_P2 && O == m_O2) {
		Upgrade(m_value2, Value);
		m_date2 = date;
	}
	else if (Value.cost > m_value1.cost) { // TODO: This should be subject to a broad range of tests
		m_P2 = m_P1;
		m_O2 = m_O1;
		m_value2 = m_value1;
		m_date2 = m_date1;

		m_P1 = P;
		m_O1 = O;
		m_value1 = Value;
		m_date1 = date;
	}
	else if (Value.cost > m_value2.cost) {// TODO: This should be subject to a broad range of tests
		m_P2 = P;
		m_O2 = O;
		m_value2 = Value;
		m_date2 = date;
	}
	else // TODO: This should be subject to a broad range of tests
	{
		const bool IsOld1 = (date - m_date1 >= OLD_AGE);
		const bool IsOld2 = (date - m_date2 >= OLD_AGE);
		if (IsOld1 && IsOld2){
			m_P2 = m_P1;
			m_O2 = m_O1;
			m_value2 = m_value1;
			m_date2 = m_date1;

			m_P1 = P;
			m_O1 = O;
			m_value1 = Value;
			m_date1 = date;
		}
		else if (!IsOld1 && IsOld2){
			m_P2 = P;
			m_O2 = O;
			m_value2 = Value;
			m_date2 = date;
		}
		else if (IsOld1 && !IsOld2){
			m_P1 = m_P2;
			m_O1 = m_O2;
			m_value1 = m_value2;
			m_date1 = m_date2;

			m_P2 = P;
			m_O2 = O;
			m_value2 = Value;
			m_date2 = date;
		}
	}

	spinlock.clear(std::memory_order_release);
}

// The return value is stored in 'Value'
bool CTwoNode::LookUp(const uint64_t P, const uint64_t O, CHashTableValueType& Value) const
{
	while (spinlock.test_and_set(std::memory_order_acquire)) ;
	
	bool ret = false;
	if ((P == m_P1) && (O == m_O1))
	{
		Value = m_value1;
		ret = true;
	}
	else if ((P == m_P2) && (O == m_O2))
	{
		Value = m_value2;
		ret = true;
	}
	
	spinlock.clear(std::memory_order_release);
	return ret;
}

void CTwoNode::Refresh(const uint64_t P, const uint64_t O, const uint8_t date)
{
	while (spinlock.test_and_set(std::memory_order_acquire)) ;

	     if ((P == m_P1) && (O == m_O1)) m_date1 = date;
	else if ((P == m_P2) && (O == m_O2)) m_date2 = date;

	spinlock.clear(std::memory_order_release);
}

void CTwoNode::Clear()
{
	while (spinlock.test_and_set(std::memory_order_acquire)) ;

	m_value1 = CHashTableValueType();
	m_value2 = CHashTableValueType();

	spinlock.clear(std::memory_order_release);
}

CHashTable& CHashTable::operator=(const CHashTable& other)
{
	if (this != &other)
	{
		delete[] table;
		table = new NodeType[other.buckets];
		for (uint64_t i = 0; i < other.buckets; i++) table[i] = other.table[i];
		buckets = other.buckets;
		date = other.date;
		UpdateCounter .store(other.UpdateCounter .load(std:: memory_order_acquire), std::memory_order_release);
		LookUpCounter .store(other.LookUpCounter .load(std:: memory_order_acquire), std::memory_order_release);
		RefreshCounter.store(other.RefreshCounter.load(std:: memory_order_acquire), std::memory_order_release);
		HitCounter	  .store(other.HitCounter	 .load(std:: memory_order_acquire), std::memory_order_release);
	}
	return *this;
}

CHashTable& CHashTable::operator=(CHashTable&& other)
{
	assert(this != &other);
	delete[] table;
	table = other.table;
	other.table = nullptr;
	buckets = other.buckets;
	date = other.date;
	UpdateCounter .store(other.UpdateCounter .load(std:: memory_order_acquire), std::memory_order_release);
	LookUpCounter .store(other.LookUpCounter .load(std:: memory_order_acquire), std::memory_order_release);
	RefreshCounter.store(other.RefreshCounter.load(std:: memory_order_acquire), std::memory_order_release);
	HitCounter	  .store(other.HitCounter	 .load(std:: memory_order_acquire), std::memory_order_release);
	return *this;
}

void CHashTable::Clear()
{
	UpdateCounter = 0;
	LookUpCounter = 0;
	RefreshCounter = 0;
	HitCounter = 0;
	for (std::size_t i = 0; i < buckets; i++) table[i].Clear();
}

void CHashTable::Save(const std::string & filename) const
{
	FILE* file = fopen(filename.c_str(), "wb");
	if (!file) {
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return;
	}
	fwrite(table, sizeof(NodeType), buckets, file);
	fclose(file);
}

void CHashTable::Load(const std::string & filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file) {
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return;
	}
	fread(table, sizeof(NodeType), buckets, file);
	fclose(file);
}

void CHashTable::print_stats()
{
	printf("Size: %s Bytes\n", ThousandsSeparator(buckets * sizeof(NodeType)).c_str());
	printf("Updates:  %s\n", ThousandsSeparator(UpdateCounter).c_str());
	printf("LookUps:  %s\n", ThousandsSeparator(LookUpCounter).c_str());
	printf("Refreshs: %s\n", ThousandsSeparator(RefreshCounter).c_str());
	printf("Hits:     %s\n", ThousandsSeparator(HitCounter).c_str());
	printf("Hit rate: %2.1f%%\n", static_cast<float>(HitCounter * 100) / static_cast<float>(LookUpCounter));
	uint64_t counter[3] = {0,0,0};
	int num;
	for (std::size_t i = 0; i < buckets; i++){
		num = 0;
		if ((table[i].m_P1 != 0) || (table[i].m_O1 != 0)) num++;
		if ((table[i].m_P2 != 0) || (table[i].m_O2 != 0)) num++;
		counter[num]++;
	}
	float total = counter[0] + counter[1] + counter[2];
	printf("Zero entry nodes : %2.1f%%\n", counter[0] / total * 100);
	printf("One  entry nodes : %2.1f%%\n", counter[1] / total * 100);
	printf("Two  entry nodes : %2.1f%%\n", counter[2] / total * 100);
}
