#pragma once

class oArchive;

class Serializable
{
public:
	virtual void Serialize(oArchive&) const = 0;
};

class oArchive
{
public:
	virtual oArchive& operator<<(const Serializable&) = 0;
};



class iArchive;

class Deserializable
{
public:
	virtual void Deserialize(iArchive&) = 0;
};

class iArchive
{
public:
	virtual bool Good() = 0;
	virtual iArchive& operator>>(Deserializable&) = 0;
};


class ioArchive : public iArchive, public oArchive
{};