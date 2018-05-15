#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "Serializables.h"

namespace IO
{
	class oStreamArchive;
	class iStreamArchive;

	class StreamSerializable : public virtual Serializable
	{
	public:
		void Serialize(oArchive&) const override;
		virtual void Serialize(oStreamArchive&) const = 0;
	};

	class StreamDeserializable : public Deserializable
	{
	public:
		void Deserialize(iArchive&) override;
		virtual void Deserialize(iStreamArchive&) = 0;
	};

	class oStreamArchive : public oArchive
	{
	public:
		virtual void Serialize(int8_t) = 0;
		virtual void Serialize(uint8_t) = 0;
		virtual void Serialize(int16_t) = 0;
		virtual void Serialize(uint16_t) = 0;
		virtual void Serialize(int32_t) = 0;
		virtual void Serialize(uint32_t) = 0;
		virtual void Serialize(int64_t) = 0;
		virtual void Serialize(uint64_t) = 0;
		virtual void Serialize(float) = 0;
		virtual void Serialize(double) = 0;
		virtual void Serialize(std::string) = 0;

		oStreamArchive& operator<<(int8_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(uint8_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(int16_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(uint16_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(int32_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(uint32_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(int64_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(uint64_t var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(float var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(double var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(std::string var) { Serialize(var); return *this; }
		oStreamArchive& operator<<(const char * var) { Serialize(std::string(var)); return *this; }

		oStreamArchive& operator<<(const Serializable& obj) override { return *this << dynamic_cast<const StreamSerializable&>(obj); }
		virtual oStreamArchive& operator<<(const StreamSerializable& obj) { obj.Serialize(*this); return *this; }
	};

	class iStreamArchive : public iArchive
	{
	public:
		virtual std::string getline() = 0;

		virtual void Deserialize(int8_t&) = 0;
		virtual void Deserialize(uint8_t&) = 0;
		virtual void Deserialize(int16_t&) = 0;
		virtual void Deserialize(uint16_t&) = 0;
		virtual void Deserialize(int32_t&) = 0;
		virtual void Deserialize(uint32_t&) = 0;
		virtual void Deserialize(int64_t&) = 0;
		virtual void Deserialize(uint64_t&) = 0;
		virtual void Deserialize(float&) = 0;
		virtual void Deserialize(double&) = 0;
		virtual void Deserialize(std::string&) = 0;

		iStreamArchive& operator>>(int8_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(uint8_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(int16_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(uint16_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(int32_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(uint32_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(int64_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(uint64_t& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(float& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(double& var) { Deserialize(var); return *this; }
		iStreamArchive& operator>>(std::string& var) { Deserialize(var); return *this; }

		iArchive& operator>>(Deserializable& obj) override { return *this >> dynamic_cast<StreamDeserializable&>(obj); }
		iStreamArchive& operator>>(StreamDeserializable& obj) { obj.Deserialize(*this); return *this; }
	};

	class Streamable : public StreamSerializable, public StreamDeserializable
	{};

	class StreamArchive : public iStreamArchive, public oStreamArchive
	{};

	inline void StreamSerializable::Serialize(oArchive& arch) const
	{
		Serialize(dynamic_cast<oStreamArchive&>(arch));
	}

	inline void StreamDeserializable::Deserialize(iArchive& arch)
	{
		Deserialize(dynamic_cast<iStreamArchive&>(arch));
	}

	class iostreamArchive : public StreamArchive
	{
	protected:
		std::shared_ptr<std::istream> istream;
		std::shared_ptr<std::ostream> ostream;
	public:
		iostreamArchive(const std::shared_ptr<std::istream>& istream, const std::shared_ptr<std::ostream>& ostream)
			: istream(istream), ostream(ostream)
		{}

		bool Good() override { return istream->good() && (istream->peek() != EOF); }
		std::string getline() override { std::string str; std::getline(*istream, str); return str; }

		void Serialize(int8_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(uint8_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(int16_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(uint16_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(int32_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(uint32_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(int64_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(uint64_t var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(float var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(double var) override { ostream->write(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Serialize(std::string var) override { *ostream << var; }

		void Deserialize(int8_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(uint8_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(int16_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(uint16_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(int32_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(uint32_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(int64_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(uint64_t& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(float& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(double& var) override { istream->read(reinterpret_cast<char*>(&var), sizeof(var)); }
		void Deserialize(std::string& var) override { *istream >> var; }
	};
}

namespace std
{
	inline IO::iStreamArchive& getline(IO::iStreamArchive& stream, std::string& string)
	{
		string = stream.getline();
		return stream;
	}
}