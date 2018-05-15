#pragma once

namespace IO
{
	class oArchive;

	class Serializable
	{
	public:
		virtual ~Serializable() {}
		virtual void Serialize(oArchive&) const = 0;
	};

	class oArchive
	{
	public:
		virtual ~oArchive() {}
		virtual oArchive& operator<<(const Serializable&) = 0;
	};



	class iArchive;

	class Deserializable
	{
	public:
		virtual ~Deserializable() {}
		virtual void Deserialize(iArchive&) = 0;
	};

	class iArchive
	{
	public:
		virtual ~iArchive() {}
		virtual bool Good() = 0;
		virtual iArchive& operator>>(Deserializable&) = 0;
	};
}