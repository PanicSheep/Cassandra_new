#include "Args.h"

CArgs::CArgs(int argc, char* argv[])
{
	Load(argc, argv);
}

void CArgs::Load(int argc, char* argv[])
{
	const std::string prefix = "-";

	for (int i = 0; i < argc; i++)
	{
		const std::string token = argv[i];
		if (token.substr(0, prefix.length()) == prefix)
		{
			const std::string key = token.substr(prefix.length());

			std::vector<std::string> values;
			for (int j = i + 1; j < argc; j++)
			{
				const std::string next_token = argv[j];
				if (next_token.substr(0, prefix.length()) == prefix)
					break;
				else
					values.push_back(next_token);
			}
			m_args[key] = std::move(values);

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