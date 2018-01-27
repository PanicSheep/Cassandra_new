#include "hashtable_perft.h"
#include "perft_basic.h"
#include "position.h"
#include <iostream>
#include <map>
#include <unordered_map>

namespace Perft_Haswell
{
	CHashTable * hashtable;
	std::map<CPosition, std::size_t> PosMap;
	
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
		
		if (BitBoardPossible == 0)
			return PopCount(PossibleMoves(O, P));
		
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
	
	// perft for 2 plies left
	uint64_t perft2_HT(const uint64_t P, const uint64_t O)
	{
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		
		if (!BitBoardPossible)
			return PopCount(PossibleMoves(O, P));
		
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

	uint64_t perft_HT(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		uint64_t sum = 0;
		if (depth == 2) return perft2_HT(P, O);
		
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
		{
			if (HasMoves(O, P))
				return perft_HT(O, P, depth-1);
			else
				return 0ULL;
		}
		
		if (hashtable->LookUp(P, O, depth, sum)) return sum;

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
	
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth, const uint64_t RAM)
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
				const std::size_t initialDepth = 9;
				PosMap.clear();
				perft_fill_unique(P, O, initialDepth);
				const int64_t size = static_cast<int64_t>(PosMap.size());
				hashtable = new CHashTable(RAM / sizeof(BigNode));
				std::vector<std::pair<CPosition, std::size_t>> vec(PosMap.begin(), PosMap.end());
				std::size_t sum = 0;
				#pragma omp parallel for schedule(static,1) reduction(+:sum)
				for (int64_t i = 0; i < size; i++)
				{
					sum += perft_HT(vec[i].first.P, vec[i].first.O, depth-initialDepth) * vec[i].second;
				}
				delete hashtable;
				return sum;
		}
	}
	
	uint64_t perft(const uint8_t depth, const uint64_t RAM)
	{
		return perft(CPosition::StartPosition().P, CPosition::StartPosition().O, depth, RAM);
	}
}

void perft(const uint8_t depth, const uint64_t RAM)
{
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	uint64_t result;

	std::cout << "depth|       Positions        |correct|       Time       |       N/s       " << std::endl;
	std::cout << "-----+------------------------+-------+------------------+-----------------" << std::endl;

	for (uint8_t d = 0; d <= depth; ++d)
	{
		startTime = std::chrono::high_resolution_clock::now();
		result = Perft_Haswell::perft(d, RAM);
		endTime = std::chrono::high_resolution_clock::now();

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		printf(" %3u | %22s |%7s| %14s | %15s\n", 
			   d, 
			   ThousandsSeparator(result).c_str(), (Perft::Correct(d) == result ? "  true " : " false "), 
			   time_format(endTime - startTime).c_str(), 
			   ms > 0 ? ThousandsSeparator(result / ms * 1000).c_str() : ""
			  );
	}
}

std::size_t ParseRAM(const std::string& s)
{
	std::size_t GB = s.find("GB");
	std::size_t MB = s.find("MB");
	std::size_t kB = s.find("kB");
	std::size_t B = s.find("B");
	if (GB != std::string::npos) return std::stoll(s) * 1024 * 1024 * 1024;
	if (MB != std::string::npos) return std::stoll(s) * 1024 * 1024;
	if (kB != std::string::npos) return std::stoll(s) * 1024;
	if ( B != std::string::npos) return std::stoll(s);
	return 0;
}

void PrintHelp()
{
	std::cout << "\t-d\tDepth of perft.\n" 
		      << "\t-RAM\tNumber of hash table bytes.\n"
	          << "\t-h\tPrints this help." << std::endl;
}

int main(int argc, char* argv[])
{
	unsigned int d = 17;
	std::size_t RAM = 512 * 1024 * 1024;

	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-d") d = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-RAM") RAM = ParseRAM(argv[++i]);
		else if (std::string(argv[i]) == "-h") { PrintHelp(); return 0; }
	}
	
	perft(d, RAM);
	
	return 0;
}