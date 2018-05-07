#include "Path.h"
#include <vector>
#include <cstdio>

template <typename T>
std::vector<T> read_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for input.");

	std::vector<T> vec;
	T buffer;
	while ((size > 0) && file.read(reinterpret_cast<char*>(&buffer), sizeof(T))) {
		vec.push_back(buffer);
		size--;
	}

	file.close();
	return vec;
}

template <typename T>
std::vector<T> read_vector(const CPath & file, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	return read_vector<T>(file.GetAbsoluteFilePath(), size);
}

template <typename T>
void write_to_file(const std::string & filename, const std::vector<T>& vec)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for output.");

	file.write(reinterpret_cast<const char*>(&vec[0]), sizeof(T) * vec.size());

	file.close();
}

template <typename T>
void write_to_file(const CPath & file, const std::vector<T>& vec)
{
	write_to_file(file.GetAbsoluteFilePath(), vec);
}

template <typename Iterator>
void write_to_file(const std::string & filename, Iterator begin, Iterator end)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	for (auto it = begin; it != end; ++it)
		file.write(reinterpret_cast<const char*>(&*it), sizeof(typename Iterator::value_type));

	file.close();
}

template <typename Iterator>
void write_to_file(const CPath & file, Iterator begin, Iterator end)
{
	write_to_file(file.GetAbsoluteFilePath(), begin, end);
}