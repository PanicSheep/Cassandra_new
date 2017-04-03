#pragma once
#include "position.h"
#include <algorithm>
#include <cassert>
#include <vector>

class CLine
{
	std::vector<uint8_t> line;
public:
	CLine() {} /// default constructor
	CLine(const CLine& o) : line(o.line) {} /// copy constructor
	CLine(CLine&& o) : line(std::move(o.line)) {} // move constructor
	
	CLine(const uint8_t size) : line(std::vector<uint8_t>(size, 64)) {}
	
	CLine & operator=(const CLine& o) {
		if (this != &o)
			line = o.line;
		return *this;
	}
	CLine & operator=(CLine&& o) {
		if (this != &o)
			std::swap(line, o.line);
		return *this;
	}

	// Sets a new best move and its line
	inline void NewPV(const uint8_t newMove, CLine newLine) {
		assert(line.size() == newLine.size()+1);
		line[0] = newMove;
		std::copy(line.begin()+1, std::move_iterator(newLine.begin()), std::move_iterator(newLine.end()));
	}
	
	// Sets a new best move
	inline void NewPV(const uint8_t newMove) { 
		assert(line.size() == 1);
		line[0] = newMove;
	}
	
	inline void NoMoves() { std::fill(line.begin(), line.end(), 64); }
	
	inline uint8_t BestMove() const { 
		assert(line.size());
		return line[0];
	}
	
	inline const std::string GetPV(const int depth) const {
		if (depth >= line.size())
			return "--";
		else
			return field_name(line[depth]);
	}
	inline const std::string GetPV(const int StartDepth, const int Count) const {
		std::string s;
		for (int i = StartDepth; i < StartDepth + Count - 1; i++)
			s.append(GetPV(i)).append(" ");
		s.append(GetPV(StartDepth + Count - 1));
		return s;
	}
	inline const std::string GetPV() const { return GetPV(0, size); }
	
};
