#pragma once
#include <algorithm>
#include <unordered_set>
#include <random>
#include <functional>
#include <thread>
#include <atomic>
#include "position.h"

std::unordered_set<CPosition> GenerateRandomPositions(const std::size_t numPos, const uint8_t numEmpties, const bool ETH, const std::size_t seedNum);
std::unordered_set<CPosition> GenerateRandomPositions(const std::size_t numPos, const uint8_t numEmpties, const bool ETH = false);
std::unordered_set<CPosition> GenerateAllPositions(const uint8_t numEmpties, const bool ETH = false);
