#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

template <class T>
std::vector<T> read_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
		throw "File could not be opened.";

	const std::size_t N = 4 * 1024;
	std::size_t ValidData;
	std::vector<T> vec;
	T* DataArray = new T[N];
	while ((ValidData = fread(DataArray, sizeof(T), std::min(N, size), file))){
		vec.insert(vec.end(), DataArray, DataArray + ValidData);
		size -= ValidData;
	}

	fclose(file);
	delete[] DataArray;

	return vec;
}

template <class T>
void write_to_file(const std::string & filename, const std::vector<T>& vec)
{
	FILE* file = fopen(filename.c_str(), "wb");
	if (!file)
		throw "File could not be opened.";

	fwrite(&vec[0], sizeof(T), vec.size(), file);

	fclose(file);
}