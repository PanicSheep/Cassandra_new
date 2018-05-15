#pragma once
#include <cstdint>
#include <string>
#include <queue>
#include <vector>
#include <strstream>
#include "StreamSerializables.h"

namespace IO
{
	class StreamQueue : public StreamArchive
	{
		std::queue<void*> queue;
	public:
		void Serialize(int8_t var) override { queue.push(new   int8_t(var)); }
		void Serialize(uint8_t var) override { queue.push(new  uint8_t(var)); }
		void Serialize(int16_t var) override { queue.push(new  int16_t(var)); }
		void Serialize(uint16_t var) override { queue.push(new uint16_t(var)); }
		void Serialize(int32_t var) override { queue.push(new  int32_t(var)); }
		void Serialize(uint32_t var) override { queue.push(new uint32_t(var)); }
		void Serialize(int64_t var) override { queue.push(new  int64_t(var)); }
		void Serialize(uint64_t var) override { queue.push(new uint64_t(var)); }
		void Serialize(float var) override { queue.push(new    float(var)); }
		void Serialize(double var) override { queue.push(new   double(var)); }
		void Serialize(std::string var) override { queue.push(new std::string(var)); }

		bool Good() override { return true; }
		std::string getline() override { return std::string(); }

		void Deserialize(int8_t& var) override { var = *reinterpret_cast<int8_t*>(queue.front()); queue.pop(); }
		void Deserialize(uint8_t& var) override { var = *reinterpret_cast<uint8_t*>(queue.front()); queue.pop(); }
		void Deserialize(int16_t& var) override { var = *reinterpret_cast<int16_t*>(queue.front()); queue.pop(); }
		void Deserialize(uint16_t& var) override { var = *reinterpret_cast<uint16_t*>(queue.front()); queue.pop(); }
		void Deserialize(int32_t& var) override { var = *reinterpret_cast<int32_t*>(queue.front()); queue.pop(); }
		void Deserialize(uint32_t& var) override { var = *reinterpret_cast<uint32_t*>(queue.front()); queue.pop(); }
		void Deserialize(int64_t& var) override { var = *reinterpret_cast<int64_t*>(queue.front()); queue.pop(); }
		void Deserialize(uint64_t& var) override { var = *reinterpret_cast<uint64_t*>(queue.front()); queue.pop(); }
		void Deserialize(float& var) override { var = *reinterpret_cast<float*>(queue.front()); queue.pop(); }
		void Deserialize(double& var) override { var = *reinterpret_cast<double*>(queue.front()); queue.pop(); }
		void Deserialize(std::string& var) override { var = *reinterpret_cast<std::string*>(queue.front()); queue.pop(); }
	};
}