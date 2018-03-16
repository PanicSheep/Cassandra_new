#include "Environment.h"

Environment::Environment()
{
	Configurations = std::make_shared<CConfigurations>();
	HashTable = std::make_shared<CHashTablePVS>(16'000);
}

Environment::Environment(std::shared_ptr<CConfigurations> Configurations, std::shared_ptr<CHashTablePVS> HashTable)
	: Configurations(Configurations)
	, HashTable(HashTable)
{}