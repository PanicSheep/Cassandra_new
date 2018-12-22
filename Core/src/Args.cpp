#include "Args.h"

CArgs::CArgs(int argc, char* argv[])
{
	Load(argc, argv);
}

void CArgs::Load(int argc, char* argv[])
{
	const std::string prefix = "-";
	const std::size_t prefix_length = prefix.length();

	for (std::size_t i = 0; i < argc; i++)
	{
		const std::string token = argv[i];
		if (token.compare(0, prefix_length, prefix) == 0)
		{
			const std::string key = token.substr(prefix_length);

			std::vector<std::string> values;
			for (std::size_t j = i + 1; j < argc; j++)
			{
				const std::string next_token = argv[j];
				if (next_token.compare(0, prefix_length, prefix) == 0)
					break;
				values.emplace_back(next_token);
			}
			std::move(values.begin(), values.end(), std::back_inserter(m_args[key]));

			i += values.size();
		}
	}
}

bool CArgs::Has(const std::string& key) const
{
	return m_args.count(key) > 0;
}

std::vector<std::string> CArgs::Get(const std::string& key) const
{
	return m_args.at(key);
}

void CArgs::Set(const std::string& key)
{
	Set(key, std::vector<std::string>{});
}

void CArgs::Set(const std::string & key, int64_t value)
{
	Set(key, std::to_string(value));
}

void CArgs::Set(const std::string& key, std::string&& value)
{
	Set(key, std::vector<std::string>{value});
}

void CArgs::Set(const std::string& key, std::vector<std::string>&& values)
{
	m_args[key] = std::move(values);
}