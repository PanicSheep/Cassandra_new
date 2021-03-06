#include <chrono>
#include <omp.h>
#include <iostream>
#include <string>
#include "Args.h"
#include "Pattern.h"
#include "IoPattern.h"
#include "MatrixCSR_Grid.h"
#include "VectorIO.h"
#include "Utility.h"
#include "IterativeAlgorithms.h"
#include "VectorExtension.h"
#include "Path.h"

using namespace IO;

void CreateDefaultWeight(const CPath& exe_folder, const CArgs& args)
{
	// Intended use:
	// weightfitter.exe -default -wo folder\prefix_ -pattern name1 name2 0x0123456789ABCDEF ...
	//		creates file 'folder\prefix_name1.w' with default weights
	//		creates file 'folder\prefix_name2.w' with default weights
	//		creates file 'folder\prefix_0x0123456789ABCDEF.w' with default weights

	CPath pattern_names;
	if (args.Has("pattern_names"))
		pattern_names = args.Get("pattern_names").back();
	else
		pattern_names = exe_folder + "pattern_names.ini";
	Pattern::IO::CNameLookupTable pattern_lookup_table(pattern_names);

	const CPath weights_out = args.Get("wo").back();

	std::vector<std::string> patterns = args.Get("pattern");
	
	for (const auto it : patterns)
	{
		const uint64_t pattern = pattern_lookup_table.Translate(it);
		const auto weights = Pattern::Configurations::CreateDefaultWeight(pattern);

		write_to_file(weights_out.GetAbsoluteFilePath() + it + ".w", weights);
	}
}

bool Check_argument(const std::string& message_prefix, const CArgs& args, CLI_Arg arg)
{
	if (args.Has(arg.name))
	{
		if (arg.size == CLI_Arg::UNDEFINED)
			return true;
		else if (args.Get(arg.name).size() == arg.size)
			return true;
		else
		{
			std::cerr << message_prefix << ": Wrong argument count of '-" << arg.name << "'. Expectet " << arg.size << ", having " << args.Get(arg.name).size() << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << message_prefix << ": Missing argument '-" << arg.name << "'." << std::endl;
		return false;
	}
}

bool Check_mandatory_argument(const CArgs& args, CLI_Arg arg)
{
	return Check_argument("ERROR", args, arg);
}

bool Check_optional_argument(const CArgs& args, CLI_Arg arg)
{
	return Check_argument("WARNING", args, arg);
}

const CLI_Arg CLI_arg_position("pos", 1);
const CLI_Arg CLI_arg_input_folder("i", 1);
const CLI_Arg CLI_arg_weights_input_folder("wi", 1);
const CLI_Arg CLI_arg_weights_ounput_folder("wo", 1);
const CLI_Arg CLI_arg_pattern("pattern");
const CLI_Arg CLI_arg_maxit("maxit", 1);
const CLI_Arg CLI_arg_default("default", 0);

void PrintHelp()
{
	std::cout << "Fits weights.\n" <<
		"Arguments:\n" <<
		"-i abc\\                           Matrix & vector input folder.\n" <<
		"-wi abc\\                          input weights filename prefix.\n" <<
		"-wo abc\\                          output weights filename prefix.\n" <<
		"-pattern name1 name2 name3        Pattern.\n" <<
		"-maxit 123                        Maximum iterations.\n" <<

		"-default                          Creates default weights.\n" <<
		"-h                                Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	const auto exe_folder = CPath(argv[0]).GetAbsoluteFolderPath();
	CArgs args(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	if (args.Has("default")) {
		CreateDefaultWeight(exe_folder, args);
		return 0;
	}

	// Parse input data
	// ------------------------
	const CPath input_folder = args.Get("i").back();
	const CPath weights_out = args.Get("wo").back();
	const auto patterns = args.Get("pattern");

	Pattern::IO::CNameLookupTable pattern_translator(exe_folder + "pattern_names.ini");
	
	std::vector<CPath> weight_in_filenames;
	if (args.Has("wi")) {
		const CPath weights_in = args.Get("wi").back();
		for (const auto& pat : patterns)
			weight_in_filenames.push_back(weights_in.GetAbsoluteFilePath() + pat + ".w");
	}

	std::vector<CPath> weight_out_filenames;
	for (const auto& pat : patterns)
		weight_out_filenames.push_back(weights_out.GetAbsoluteFilePath() + pat + ".w");

	std::size_t maxIt = 100;
	if (args.Has("maxit"))
		maxIt = std::stoull(args.Get("maxit").back());
	// ------------------------

	// Process input data
	// ------------------------
	std::vector<CPath> matrix_filenames;
	for (const auto& pat : patterns)
		matrix_filenames.push_back(input_folder.GetAbsoluteFilePath() + "_" + pat + ".m");

	const CPath vector_filename = input_folder.GetAbsoluteFilePath() + ".v";

	const std::size_t n = 1;
	const std::size_t m = patterns.size();
	// ------------------------

	// Print input data
	// ------------------------
	if (matrix_filenames.size() != n * m)
		std::cerr << "ERROR: Sizes mismatch!" << std::endl;

	std::cout << "Matrix size: " << matrix_filenames.size() << "\n"
		<< "Vector size: " << n << "\n"
		<< "Weight size: " << m << std::endl;

	std::cout << "Matrices:" << std::endl;
	for (const auto& it : matrix_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	std::cout << "Vector:" <<  vector_filename.GetRelativeFilePath() << ", " << std::endl;

	std::cout << "Weights:" << std::endl;
	for (const auto& it : weight_out_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	std::cout << "Initial-Weights:" << std::endl;
	for (const auto& it : weight_in_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;
	// ------------------------

	Array2D<std::string> arr_matrix_filenames(n, m);
	for (std::size_t i = 0; i < n; i++)
		for (std::size_t j = 0; j < m; j++)
			arr_matrix_filenames(i, j) = matrix_filenames[i * m + j].GetAbsoluteFilePath();


	// Loading data
	// ------------------------
	std::cout << "Loading data...";
	auto start_time = std::chrono::high_resolution_clock::now();

	auto A = CMatrix_CSR_Grid<uint8_t, uint32_t>(arr_matrix_filenames);
	auto b = Vecvec<float>(n);
	for (std::size_t i = 0; i < n; i++)
		b(i) = read_vector<float>(vector_filename);

	Vecvec<float> x0;
	if (weight_in_filenames.empty())
		x0 = Vecvec<float>(A.ms(), 0);
	else
	{
		x0 = Vecvec<float>(m);
		for (std::size_t j = 0; j < m; j++)
			x0(j) = read_vector<float>(weight_in_filenames[j]);
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(end_time - start_time) << std::endl;
	// ------------------------

	// Solving data
	// ------------------------
	std::cout << "Solving data...";
	start_time = std::chrono::high_resolution_clock::now();

	CDiagonalPreconditioner<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	auto alg = new CIterativePLSQR<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, &P, b, x0, 1);
	alg->Iterate(maxIt);
	auto x = alg->GetX();
	for (std::size_t j = 0; j < m; j++)
		write_to_file(weight_out_filenames[j], x(j));
	end_time = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(end_time - start_time) << std::endl;
	if (alg->GetTerminationCause() == eTerminationCause::Converged)
		std::cout << "Converged." << std::endl;
	else
		std::cout << "WARNING: Did not converge!" << std::endl;
	// ------------------------

	std::cout << "stddev: " << StandardDeviation(A * x - b) << std::endl;

	return 0;
}