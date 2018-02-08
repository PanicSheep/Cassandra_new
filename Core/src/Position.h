#pragma once
#include "MacrosHell.h"
#include "Moves.h"

class CPosition
{
public:
	uint64_t P, O;
	
	CPosition() : CPosition(0, 0) {}
	CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}
	
	static CPosition StartPosition   () { return CPosition(0x0000000810000000ULL, 0x0000001008000000ULL); }
	static CPosition StartPositionETH() { return CPosition(0x0000001800000000ULL, 0x0000000018000000ULL); }
	
	uint64_t GetP() const { return P; }
	uint64_t GetO() const { return O; }
	
	uint64_t Parity() const;
	uint64_t Empties() const       { return ~(P | O); }
	uint64_t EmptyCount() const    { return PopCount(Empties()); }
	CMoves   PossibleMoves() const;
	bool     HasMoves() const;
	
	CPosition Play(const CMove& move) const;
	CPosition PlayPass() const { return CPosition(O, P); }
	
	void FlipCodiagonal();
	void FlipDiagonal  ();
	void FlipHorizontal();
	void FlipVertical  ();
	void FlipToMin     ();
	
	bool operator==(const CPosition& o) const { return (P == o.P) && (O == o.O); }
	bool operator!=(const CPosition& o) const { return (P != o.P) || (O != o.O); }
	bool operator<=(const CPosition& o) const { return (P <= o.P) || ((P == o.P) && (O <= o.O)); }
	bool operator>=(const CPosition& o) const { return (P >= o.P) || ((P == o.P) && (O >= o.O)); }
	bool operator< (const CPosition& o) const { return (P <  o.P) || ((P == o.P) && (O <  o.O)); }
	bool operator> (const CPosition& o) const { return (P >  o.P) || ((P == o.P) && (O >  o.O)); }
};

namespace std
{
	template<> struct hash<CPosition>
	{
		inline std::size_t operator()(const CPosition& pos) const
		{
			return pos.P ^ ((pos.O << 32) | (pos.O >> 32));
		}
	};
}