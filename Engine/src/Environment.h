#pragma once
#include <memory>
#include "FlipFast.h"
#include "LastFlipCounter.h"
#include "HashtablePVS.h"
#include "ConfigFile.h"
#include "Stability.h"

struct Environment
{
	std::shared_ptr<CConfigurations> Configurations;
	std::shared_ptr<CHashTablePVS> HashTable;

	Environment();
	Environment(std::shared_ptr<CConfigurations> Configurations, std::shared_ptr<CHashTablePVS> HashTable);
};