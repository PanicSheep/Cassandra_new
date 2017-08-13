#include "matrixCSR_Grid.h"
#include <chrono>
#include <omp.h>
#include <iostream>
#include "datamanipulation.h"
#include "utility.h"
#include "iterativealgorithms.h"
#include "vector_extensions.h"
#include "statistics.h"
#include "pattern.h"

void print_help()
{
	std::cout << "Makes matrices and a vectors out of positions.\n" <<
		"Arguments:\n" <<
		" In detail specification:\n" <<
		" -mat abc.xyz def.uvw ... (NxM)    Matrix filenames.\n" <<
		" -vec abc.xyz def.uvw ... (N  )    Vector filenames.\n" <<
		" -wei abc.xyz def.uvw ... (  M)    Weights filenames.\n" <<
		" -wei0 abc.xyz def.uvw ... (  M)   Initial-weights filenames.\n" <<
		" Batch processing:\n" <<
		" -p abc abc                        Position filename prefix.\n" <<
		" -m \abc                           Matrix folder.\n" <<
		" -v \abc                           Vector folder.\n" <<
		" -w \abc                           Weights filename prefix.\n" <<
		" -w0 \abc                          Initial-weights filename prefix.\n" <<
		" -pattern name1 name2 name3        Pattern.\n" <<
		"-maxit 123                         Maximum iterations to use.\n"
		"-h                                 Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	std::size_t maxIt = 100;
	std::string matrix;
	std::string vector;
	std::string weight;
	std::string weight0;
	std::vector<std::string> position;
	std::vector<std::string> pattern;
	std::vector<std::string> matrix_filename;
	std::vector<std::string> vector_filename;
	std::vector<std::string> weight_filename;
	std::vector<std::string> weight0_filename;
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	
	Configfile::Initialize(argv[0]);
	Pattern::Initialize(false, false);

	for (int i = 0; i < argc; ++i)
	{
		if (std::string(argv[i]) == "-pattern") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				pattern.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-p") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				position.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-m") matrix = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-v") vector = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-w") weight = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-w0") weight0 = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-mat") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				matrix_filename.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-vec") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				vector_filename.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-wei") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				weight_filename.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-wei0") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				weight_filename.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-maxit") maxIt = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-h") { print_help(); return 0; }
	}
	
	// If no pattern provided, use pattern from config file.
	if (pattern.empty())
	{
		std::vector<std::string> ActivePattern = Pattern::GetActivePattern();
		for (const auto& it : ActivePattern)
			pattern.push_back(it);
	}
	
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
	

	startTime = std::chrono::high_resolution_clock::now();

	const std::size_t n = vector_filename.size();
	const std::size_t m = weight_filename.size();
	
	if (matrix_filename.size() != n*m)
		std::cerr << "ERROR: Sizes mismatch!" << std::endl;
	
	std::cout << "Matrix size: " << matrix_filename.size() << "\n"
		<< "Vector size: " << n << "\n"
		<< "Weight size: " << m << std::endl;
	
	std::cout << "Matrices:" << std::endl;
	for (const auto& it : matrix_filename)
		std::cout << it << ", ";
	std::cout << std::endl;
	
	std::cout << "Vectors:" << std::endl;
	for (const auto& it : vector_filename)
		std::cout << it << ", ";
	std::cout << std::endl;
	
	std::cout << "Weights:" << std::endl;
	for (const auto& it : weight_filename)
		std::cout << it << ", ";
	std::cout << std::endl;
	
	std::cout << "Initial-Weights:" << std::endl;
	for (const auto& it : weight0_filename)
		std::cout << it << ", ";
	std::cout << std::endl;
	
	if (matrix_filename.size() != n*m)
		return -1;
	
	
	Array2D<std::string> arr_matrix_filename(n,m);
	for (std::size_t i = 0; i < n; i++)
		for (std::size_t j = 0; j < m; j++)
			arr_matrix_filename(i,j) = matrix_filename[i * m + j];
	
	// Loading data
	// ------------------------
	std::cout << "Loading data...";
	startTime = std::chrono::high_resolution_clock::now();
	
	auto A = CMatrix_CSR_Grid<uint8_t, uint32_t>(arr_matrix_filename);
	auto b = Vecvec<float>(n);
	for (std::size_t i = 0; i < n; i++)
		b(i) = read_vector<float>(vector_filename[i]);
	
	endTime = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	// ------------------------
	
	// Solving data
	// ------------------------
	std::cout << "Solving data...";;
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