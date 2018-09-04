#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class CArgs
{
	std::unordered_map<std::string, std::vector<std::string>> m_args;
public:
	CArgs() = default;
	CArgs(int argc, char* argv[]);

	void Load(int argc, char* argv[]);

	bool Has(const std::string& key) const;
	std::vector<std::string> Get(const std::string& key) const;
	void Set(const std::string& key);
	void Set(const std::string& key, int64_t value);
	void Set(const std::string& key, std::string&& value);
	void Set(const std::string& key, std::vector<std::string>&& values);
};

// Command Line Interface Arguments
class CLI_Arg
{
public:
	const static std::size_t UNDEFINED = std::numeric_limits<std::size_t>::max();
	std::string name;
	std::size_t size;

	CLI_Arg(std::string name, std::size_t size = UNDEFINED) : name(name), size(size) {}
};