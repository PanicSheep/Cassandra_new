#include "Perft_basic.h"
#include "HashtablePerft.h"
#include "Utility.h"
#include <chrono>
#include <iostream>
#include <map>
#include <optional>
#include <unordered_map>

namespace Perft
{
	uint64_t Correct(uint8_t depth)
	{
		const uint64_t correct[] = { 1, 4, 12, 56, 244, 1396, 8200, 55092, 390216, 3005288, 24571056, 212258216, 1939879668, 18429618408, 184041761768, 1891831332208, 20301171282452, 222742563853912, 2534535926617852, 29335558770589276, 20, 21 };
		return correct[depth];
	}

	namespace Basic
	{
		uint64_t perft(const CPosition& pos, const uint8_t depth)
		{
			if (depth == 0)
				return 1ui64;

			auto moves = pos.PossibleMoves();

			if (moves.empty())
			{
				auto PosPass = pos.PlayPass();
				if (PosPass.HasMoves())
					return perft(PosPass, depth - 1);
				
				return 0ui64;
			}

			uint64_t sum = 0ui64;
			while (!moves.empty())
				sum += perft(pos.Play(moves.ExtractMove()), depth - 1);

			return sum;
		}

		uint64_t perft(const uint8_t depth)
		{
			CPosition pos = CPosition::StartPosition();

			if (depth == 0)
				return perft(pos, depth);

			pos = pos.Play(pos.PossibleMoves().ExtractMove());
			return 4ui64 * perft(pos, depth - 1);
		}
	}

	namespace Unrolled2
	{
		// perft for 0 plies left
		uint64_t perft0(const CPosition&)
		{
			return 1ui64;
		}

		// perft for 1 ply left
		uint64_t perft1(const CPosition& pos)
		{
			return pos.PossibleMoves().size();
		}

		// perft for 2 plies left
		uint64_t perft2(const CPosition& pos)
		{
			auto moves = pos.PossibleMoves();

			if (moves.empty())
				return pos.PlayPass().PossibleMoves().size();

			uint64_t sum = 0ui64;
			while (!moves.empty())
			{
				const auto pos2 = pos.Play(moves.ExtractMove());
				const auto moves2 = pos2.PossibleMoves();
				if (!moves2.empty())
					sum += moves2.size();
				else
					sum += static_cast<uint64_t>(pos2.PlayPass().HasMoves());
			}

			return sum;
		}

		uint64_t perft_(const CPosition& pos, const uint8_t depth)
		{
			if (depth == 2)
				return perft2(pos);

			auto moves = pos.PossibleMoves();

			if (moves.empty())
			{
				auto PosPass = pos.PlayPass();
				if (PosPass.HasMoves())
					return perft_(PosPass, depth - 1);
				
				return 0ui64;
			}

			uint64_t sum = 0ui64;
			while (!moves.empty())
				sum += perft_(pos.Play(moves.ExtractMove()), depth - 1);

			return sum;
		}

		uint64_t perft(const CPosition& pos, const uint8_t depth)
		{
			switch (depth)
			{
				case 0: return perft0(pos);
				case 1: return perft1(pos);
				case 2: return perft2(pos);
				default: return perft_(pos, depth);
			}
		}

		uint64_t perft(const uint8_t depth)
		{
			CPosition pos = CPosition::StartPosition();

			if (depth == 0)
				return perft(pos, depth);
			
			pos = pos.Play(pos.PossibleMoves().ExtractMove());
			return 4ui64 * perft(pos, depth - 1);
		}
	}

	namespace HashTableMap
	{
		class CPerft
		{
			HashTablePerft ht;
			std::map<CPosition, std::size_t> PosMap;

			uint64_t perft_HT(const CPosition&, uint8_t depth);
			void fill_unique(CPosition, uint8_t depth);
		public:
			CPerft(uint64_t BytesRAM) : ht(BytesRAM / sizeof(HashTablePerft::nodetype)) {}
			uint64_t calculate(const CPosition&, uint8_t depth);
		};

		uint64_t CPerft::perft_HT(const CPosition& pos, const uint8_t depth)
		{
			if (depth == 2)
				return  Unrolled2::perft2(pos);

			auto moves = pos.PossibleMoves();

			if (moves.empty())
			{
				const auto PosPass = pos.PlayPass();
				if (PosPass.HasMoves())
					return perft_HT(PosPass, depth - 1);
				
				return 0ui64;
			}

			if (const auto ret = ht.LookUp(PerftKey(pos, depth)); ret.has_value())
				return ret.value();

			uint64_t sum = 0ui64;
			while (!moves.empty())
				sum += perft_HT(pos.Play(moves.ExtractMove()), depth - 1);

			ht.Update(PerftKey(pos, depth), sum);
			return sum;
		}

		void CPerft::fill_unique(CPosition pos, const uint8_t depth)
		{
			if (depth == 0) {
				pos.FlipToMin();
				PosMap[pos]++;
				return;
			}

			auto moves = pos.PossibleMoves();

			if (moves.empty())
			{
				auto PosPass = pos.PlayPass();
				if (PosPass.HasMoves())
					fill_unique(PosPass, depth - 1);
				return;
			}

			while (!moves.empty())
				fill_unique(pos.Play(moves.ExtractMove()), depth - 1);
		}

		uint64_t CPerft::calculate(const CPosition& pos, const uint8_t depth)
		{
			const std::size_t initialDepth = 9;
			fill_unique(pos, initialDepth);
			const auto size = static_cast<int64_t>(PosMap.size());
			std::vector<std::pair<CPosition, std::size_t>> vec(PosMap.begin(), PosMap.end());
			PosMap.clear();

			std::size_t sum = 0;
			#pragma omp parallel for schedule(dynamic,1) reduction(+:sum)
			for (int64_t i = 0; i < size; i++)
			{
				sum += perft_HT(vec[i].first, depth - initialDepth) * vec[i].second;
			}
			return sum;
		}

		uint64_t perft(const CPosition& pos, const uint8_t depth, const uint64_t BytesRAM)
		{
			if (depth < 13)
				return Unrolled2::perft(pos, depth);
			return CPerft(BytesRAM).calculate(pos, depth);
		}

		uint64_t perft(const uint8_t depth, const uint64_t BytesRAM)
		{
			CPosition pos = CPosition::StartPosition();

			if (depth == 0)
				return perft(pos, depth, BytesRAM);
			
			pos = pos.Play(pos.PossibleMoves().ExtractMove());
			return 4ui64 * perft(pos, depth - 1, BytesRAM);
		}
	}
}