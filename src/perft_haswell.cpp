#include "hashtable_perft.h"
#include "perft_basic.h"
#include "position.h"
#include <iostream>
#include <unordered_map>

namespace Perft_Haswell
{
	CHashTable * hashtable;
	std::unordered_map<CPosition, std::size_t> PosMap;
	
	uint64_t perft0(const uint64_t P, const uint64_t O)
	{
		return 1;
	}
	
	uint64_t perft1(const uint64_t P, const uint64_t O)
	{
		return PopCount(PossibleMoves(P, O));
	}

	// perft for 2 plies left
	uint64_t perft2(const uint64_t P, const uint64_t O)
	{
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0) return PopCount(PossibleMoves(O, P));

		uint64_t sum = 0;

		while (BitBoardPossible)
		{
			const auto LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, LSB);
			const auto BitBoardPossible2 = PossibleMoves(O ^ flipped, P ^ flipped ^ (1ULL << LSB));
			if (BitBoardPossible2)
				sum += PopCount(BitBoardPossible2);
			else if (HasMoves(P ^ flipped ^ (1ULL << LSB), O ^ flipped))
				sum++;
		}

		return sum;
	}

	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		if (depth == 2) return perft2(P, O);
		
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
		{
			if (HasMoves(O, P))
				return perft(O, P, depth-1);
			else
				return 0ULL;
		}
		
		uint64_t sum = 0;

		while (BitBoardPossible)
		{
			const auto LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, LSB);
			sum += perft(O ^ flipped, P ^ flipped ^ (1ULL << LSB), depth - 1);
		}

		return sum;
	}

	uint64_t perft_HT(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		uint64_t sum = 0;
		if (depth == 2) return perft2(P, O);
		if (hashtable->LookUp(P, O, depth, sum)) return sum;
		
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
		{
			if (HasMoves(O, P))
				return perft_HT(O, P, depth-1);
			else
				return 0ULL;
		}

		while (BitBoardPossible)
		{
			const auto LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, LSB);
			sum += perft_HT(O ^ flipped, P ^ flipped ^ (1ULL << LSB), depth - 1);
		}
		
		hashtable->Update(P, O, depth, sum);
		return sum;
	}
	
	void perft_fill_unique(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		if (depth == 0) {
			CPosition pos(P, O);
			pos.FlipToMin();
			PosMap[pos]++;
			return;
		}
		
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
		{
			if (HasMoves(O, P))
				perft_fill_unique(O, P, depth-1);
			return;
		}
		
		while (BitBoardPossible)
		{
			const auto LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, LSB);
			perft_fill_unique(O ^ flipped, P ^ flipped ^ (1ULL << LSB), depth - 1);
		}
	}
	
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth, const uint64_t bits)
	{
		switch (depth)
		{
			case 0: return perft0(P, O);
			case 1: return perft1(P, O);
			case 2: return perft2(P, O);
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11: return perft(P, O, depth);
			default:
				PosMap.clear();
				perft_fill_unique(P, O, 8);
				const std::size_t size = PosMap.size();
				hashtable = new CHashTable(1ULL << (bits - 1));
				std::vector<std::pair<CPosition, std::size_t>> vec(PosMap.begin(), PosMap.end());
				std::size_t sum = 0;
				#pragma omp parallel for reduction(+:sum)
				for (long long i = 0; i < vec.size(); i++)
				{
					sum += perft_HT(vec[i].first.P, vec[i].first.O, depth-8) * vec[i].second;
				}
				delete hashtable;
				return sum;
		}
	}
	
	uint64_t perft(const uint8_t depth, const uint64_t bits)
	{
		return perft(START_POSITION_P, START_POSITION_O, depth, bits);
	}
}

void perft(const uint8_t depth, const uint64_t bits)
{
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	uint64_t result;

	std::cout << "depth|       Positions        |correct|      Time      " << std::endl;
	std::cout << "-----+------------------------+-------+----------------" << std::endl;

	for (uint8_t d = 0; d <= depth; ++d)
	{
		startTime = std::chrono::high_resolution_clock::now();
		result = Perft_Haswell::perft(d, bits);
		endTime = std::chrono::high_resolution_clock::now();

		printf(" %3u | %22s |%7s| %14s\n", d, ThousandsSeparator(result).c_str(), (Perft::Correct(d) == result ? "  true " : " false "), time_format(endTime - startTime).c_str());
	}
}

void PrintHelp()
{
	std::cout << "\t-d\tDepth of perft.\n" 
		      << "\t-bit\tNumber of hash table key bits.\n"
	          << "\t-h\tPrints this help." << std::endl;
}

int main(int argc, char* argv[])
{
	unsigned int d = 8;
	unsigned int bits = 25;

	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-d") d = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-bit") bits = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-h") { PrintHelp(); return 0; }
	}
	
	perft(d, bits);
	
	return 0;
}