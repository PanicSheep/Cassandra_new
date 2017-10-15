#include <chrono>
#include <omp.h>
#include <iostream>
#include <string>
#include "matrixCSR_Grid.h"
#include "datamanipulation.h"
#include "utility.h"
#include "iterativealgorithms.h"
#include "vector_extensions.h"
#include "statistics.h"
#include "pattern.h"
#include "path.h"

void print_help()
{
	std::cout << "Makes matrices and a vectors out of positions.\n" <<
		"Arguments:\n" <<
		" In detail specification:\n" <<
		" -mat abc.xyz def.uvw ...  (NxM)   Matrix filenames.\n" <<
		" -vec abc.xyz def.uvw ...  (N  )   Vector filenames.\n" <<
		" -wei abc.xyz def.uvw ...  (  M)   Weights filenames.\n" <<
		" -wei0 abc.xyz def.uvw ... (  M)   Initial-weights filenames.\n" <<
		" Batch processing:\n" <<
		" -p abc abc                        Position filename prefix.\n" <<
		" -m \abc                           Matrix folder.\n" <<
		" -v \abc                           Vector folder.\n" <<
		" -w \abc                           Weights filename prefix.\n" <<
		" -w0 \abc                          Initial-weights filename prefix.\n" <<
		" -pattern name1 name2 name3        Pattern.\n" <<
		"-maxit 123                         Maximum iterations to use.\n" <<
		"-default							Creates default weights.\n" <<
		"-h                                 Display help." << std::endl;
}

class CommandlineArguments
{
	static const char NextTokenIndicator = '-';
	std::vector<std::string> args;
public:
	CommandlineArguments(int argc, char* argv[]) : args(argv, argv + argc) {}

	bool						ContainsToken (const std::string& token) const;

	CPath						ExtractPath   (const std::string& token);
	std::vector<CPath>			ExtractPaths  (const std::string& token);
	std::string					ExtractString (const std::string& token);
	std::vector<std::string>	ExtractStrings(const std::string& token);
	bool						ExtractToken  (const std::string& token);
	int							ExtractInt    (const std::string& token);
	
	std::string					TryExtractString(const std::string& token, const std::string& def);
};

bool CommandlineArguments::ContainsToken(const std::string& token) const
{
	const auto it = std::find(args.cbegin(), args.cend(), token);
	return (it != args.cend());
}

bool CommandlineArguments::ExtractToken(const std::string& token)
{
	auto it = std::find(args.begin(), args.end(), token);
	if (it != args.end()) {
		args.erase(it); // Removes token.
		return true;
	}
	else
		return false;
}

int CommandlineArguments::ExtractInt(const std::string& token)
{
	int ret = 0;

	auto it = std::find(args.begin(), args.end(), token);
	if (it != args.end()) {
		args.erase(it); // Removes token.
		if (it != args.end()) {
			ret = std::stoi(*it);
			args.erase(it);
		}
		else
			assert(false);
	}
	return ret;
}

std::vector<std::string> CommandlineArguments::ExtractStrings(const std::string& token)
{
	std::vector<std::string> ret;

	auto it = std::find(args.begin(), args.end(), token);
	if (it != args.end())
	{
		it = args.erase(it); // Removes token.
		while ((it != args.end()) && (it->at(0) != NextTokenIndicator))
		{
			ret.emplace_back(*it);
			it = args.erase(it);
		}
	}
	return ret;
}

std::vector<CPath> CommandlineArguments::ExtractPaths(const std::string& token)
{
	std::vector<CPath> ret;

	auto it = std::find(args.begin(), args.end(), token);
	if (it != args.end())
	{
		it = args.erase(it); // Removes token.
		while ((it != args.end()) && (it->at(0) != NextTokenIndicator))
		{
			ret.emplace_back(*it);
			it = args.erase(it);
		}
	}
	return ret;
}

std::string CommandlineArguments::ExtractString(const std::string& token)
{
	const auto s = ExtractStrings(token);
	if (s.size())
		return s[0];
	else
		throw std::out_of_range("Index out of range.");
}

std::string CommandlineArguments::TryExtractString(const std::string& token, const std::string& def)
{
	try
	{
		return ExtractString(token);
	}
	catch (std::out_of_range err)
	{
		return def;
	}
}


CPath CommandlineArguments::ExtractPath(const std::string& token)
{
	const auto s = ExtractPaths(token);
	if (s.size())
		return s[0];
	throw std::out_of_range("Index out of range.");
}

int main(int argc, char* argv[])
{
	std::size_t maxIt = 100;
	
	Configfile::Initialize(argv[0]);
	Pattern::Initialize(false, false);

	CommandlineArguments Args(argc, argv);
	std::vector<std::string> pattern  = Args.ExtractStrings("-pattern");
	std::vector<std::string> position = Args.ExtractStrings("-p");
	std::string matrix  = Args.TryExtractString("-m", "");
	std::string vector  = Args.TryExtractString("-v", "");
	std::string weight  = Args.TryExtractString("-w", "");
	std::string weight0 = Args.TryExtractString("-w0", "");
	std::vector<CPath> matrix_filename	= Args.ExtractPaths("-mat");
	std::vector<CPath> vector_filename	= Args.ExtractPaths("-vec");
	std::vector<CPath> weight_filename	= Args.ExtractPaths("-wei");
	std::vector<CPath> weight0_filename = Args.ExtractPaths("-wei0");
	bool bDefaults = Args.ContainsToken("-default");

	if (Args.ContainsToken("-maxit"))
		maxIt = Args.ExtractInt("-maxit");

	if (Args.ContainsToken("-h")) { 
		print_help(); 
		return 0;
	}
	
	// If no pattern provided, use pattern from config file.
	if (pattern.empty())
		pattern = Pattern::GetActivePattern();
	
	if (!pattern.empty())
	{
		for (const auto& pos : position)
			for (const auto& pat : pattern)
				matrix_filename.push_back(matrix + FOLDER_SEPARATOR + pos + "_" + pat + ".m");
		
		for (const auto& pos : position)
			vector_filename.push_back(vector + FOLDER_SEPARATOR + pos + ".v");
		
		for (const auto& pat : pattern)
			weight_filename.push_back(weight + "_" + pat + ".w");
		
		if (!weight0.empty())
			for (const auto& pat : pattern)
				weight0_filename.push_back(weight0 + "_" + pat + ".w");
	}

	if (bDefaults)
	{
		std::vector<std::size_t> ms;
		for (const auto& pat : pattern) {
			Pattern::CPattern * Pattern = Pattern::NewPattern(pat);
			ms.push_back(Pattern->ReducedSize());
			delete Pattern;
		}

		Vecvec<float> x0(ms, 0);
		for (std::size_t j = 0; j < pattern.size(); j++)
			write_to_file(weight_filename[j], x0(j));

		return 0;
	}
	
	const std::size_t n = vector_filename.size();
	const std::size_t m = weight_filename.size();
	
	if (matrix_filename.size() != n*m)
		std::cerr << "ERROR: Sizes mismatch!" << std::endl;
	
	std::cout << "Matrix size: " << matrix_filename.size() << "\n"
		<< "Vector size: " << n << "\n"
		<< "Weight size: " << m << std::endl;
	
	std::cout << "Matrices:" << std::endl;
	for (const auto& it : matrix_filename)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;
	
	std::cout << "Vectors:" << std::endl;
	for (const auto& it : vector_filename)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;
	
	std::cout << "Weights:" << std::endl;
	for (const auto& it : weight_filename)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;
	
	std::cout << "Initial-Weights:" << std::endl;
	for (const auto& it : weight0_filename)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;
	
	if (matrix_filename.size() != n*m)
	{
		throw std::logic_error("Matrix vector size mismatch");
		return -1;
	}
	

	Array2D<std::string> arr_matrix_filename(n,m);
	for (std::size_t i = 0; i < n; i++)
		for (std::size_t j = 0; j < m; j++)
			arr_matrix_filename(i,j) = matrix_filename[i * m + j].GetAbsoluteFilePath();
	
	// Loading data
	// ------------------------
	std::cout << "Loading data...";
	auto startTime = std::chrono::high_resolution_clock::now();
	
	auto A = CMatrix_CSR_Grid<uint8_t, uint32_t>(arr_matrix_filename);
	auto b = Vecvec<float>(n);
	for (std::size_t i = 0; i < n; i++)
		b(i) = read_vector<float>(vector_filename[i]);
	
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	// ------------------------
	
	// Solving data
	// ------------------------
	std::cout << "Solving data...";
	startTime = std::chrono::high_resolution_clock::now();
	
	Vecvec<float> x0;
	if (weight0_filename.empty())
		x0 = Vecvec<float>(A.ms(), 0);
	else
	{
		x0 = Vecvec<float>(m);
		for (std::size_t j = 0; j < m; j++)
			x0(j) = read_vector<float>(weight0_filename[j]);
	}
	CDiagonalPreconditioner<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	auto alg = new CIterativePLSQR<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, &P, b, x0, 1);
	alg->Iterate(maxIt);
	auto x = alg->GetX();
	for (std::size_t j = 0; j < m; j++)
		write_to_file(weight_filename[j], x(j));
	endTime = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	if (alg->GetTerminationCause() == eTerminationCause::Converged)
		std::cout << "Converged." << std::endl;
	else
		std::cout << "WARNING: Did not converge!" << std::endl;
	// ------------------------
	
	std::cout << "stddev: " << StandardDeviation(A*x-b) << std::endl;
	
	return 0;
}