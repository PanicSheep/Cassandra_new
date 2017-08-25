#pragma once
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
std::vector<T> read_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	std::vector<T> vec;
	T buffer;
	while ((size > 0) && file.read(reinterpret_cast<char*>(&buffer), sizeof T)){
		vec.push_back(buffer);
		size--;
	}

	file.close();
	return vec;
}

template <typename T>
void write_to_file(const std::string & filename, const std::vector<T>& vec)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	file.write(reinterpret_cast<const char*>(&vec[0]), sizeof vec);

	file.close();
}

template <typename Iterator>
void write_to_file(const std::string & filename, Iterator begin, Iterator end)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	for (auto it = begin; it != end; it++)
		file.write(&*it, sizeof(typename Iterator::value_type));

	file.close();
}
