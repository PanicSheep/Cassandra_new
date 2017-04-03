#pragma once
#include "position.h"
#include <algorithm>
#include <cassert>
#include <vector>

class CLine
{
public:
	uint8_t size;
	std::vector<uint8_t> line;

	CLine() : size(0) {} /// default constructor
	CLine(const CLine& o) : size(o.size), line(o.line) {} /// copy constructor
	CLine(CLine&& o) : size(o.size), line(std::move(o.line)) {} // move constructor
	
	CLine(const uint8_t size) : size(size), line(std::vector<uint8_t>(size, 64)) {}
	
	CLine & operator=(const CLine& o) {
		if (this != &o) {
			size = o.size;
			line = o.line;
		}
		return *this;
	}
	CLine & operator=(CLine&& o) {
		if (this != &o) {
			std::swap(size, o.size);
			std::swap(line, o.line);
		}
		return *this;
	}

	// Sets a new best move and its line
	inline void NewPV(const uint8_t newMove, CLine newLine) {
		assert(size == newLine.size+1);
		line[0] = newMove;
		std::copy(line.begin()+1, std::move_iterator(newLine.begin()), std::move_iterator(newLine.end()));
	}
	
	// Sets a new best move
	inline void NewPV(const uint8_t newMove) { line[0] = newMove; throw 0; } // TODO: How abount invalidating the old moves?
	
	inline void NoMoves() { std::fill_n(line, size, 64); } <-------
	inline uint8_t BestMove() const { return line[0]; }
	
	inline const std::string GetPV(const int depth) const
	{
		assert(line[depth] >= 0);
		assert(line[depth] <= 64);
		if (depth >= size)
			return "--";
		else
			return field_name(line[depth]);
	}
	inline const std::string GetPV(const int StartDepth, const int Count) const
	{
		std::string s;
		for (int i = StartDepth; i < StartDepth + Count; i++)
			s.append(GetPV(i)).append(" ");
		return s.substr(0, s.length() - 1);
	}
	inline const std::string GetPV() const { return GetPV(0, size); }
	
};
