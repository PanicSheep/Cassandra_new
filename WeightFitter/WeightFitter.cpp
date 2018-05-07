#include <chrono>
#include <omp.h>
#include <iostream>
#include <string>
#include "Args.h"
#include "ConfigFile.h"
#include "Pattern.h"
#include "MatrixCSR_Grid.h"
#include "VectorIO.h"
#include "Utility.h"
#include "IterativeAlgorithms.h"
#include "VectorExtension.h"
#include "Path.h"

void CreateDefaultWeights(CArgs& args);

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
		"-pos abc abc                      Position raw filenames.\n" <<
		"-i abc\                           Matrix & vector input folder.\n" <<
		"-wi abc\                          input weights filename prefix.\n" <<
		"-wo abc\                          output weights filename prefix.\n" <<
		"-pattern name1 name2 name3        Pattern.\n" <<
		"-maxit 123                        Maximum iterations.\n" <<
		"Batch processing:\n" <<
		"-default                          Creates default weights.\n" <<
		"-h                                Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	CPattern::Initialize();

	CArgs args;
	args.Set("config", "config.ini");
	args.Load(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	if (args.Has("default")) {
		CreateDefaultWeights(args);
		return 0;
	}

	const CPath config_file = args.Get("config")[0];
	CConfigurations configs(config_file);

	const auto positions = args.Get("pos");
	const CPath input_folder = args.Get("i")[0];

	std::vector<std::string> pattern_names;
	if (args.Has("pattern"))
		pattern_names = args.Get("pattern");
	else if (configs.Has("active pattern"))
		pattern_names = split(configs.Get("active pattern"), " ");

	std::vector<CPath> matrix_filenames;
	for (const auto& pos : positions)
		for (const auto& pat : pattern_names)
			matrix_filenames.push_back(input_folder.GetAbsoluteFolderPath() + pos + "_" + pat + ".m");

	std::vector<CPath> vector_filenames;
	for (const auto& pos : positions)
		vector_filenames.push_back(input_folder.GetAbsoluteFolderPath() + pos + ".v");

	std::vector<CPath> weight_in_filenames;
	if (args.Has("wi")) {
		const CPath weights_in = args.Get("wi")[0];
		for (const auto& pat : pattern_names)
			weight_in_filenames.push_back(weights_in.GetAbsoluteFilePath() + pat + ".w");
	}

	std::vector<CPath> weight_out_filenames;
	const CPath weights_out = args.Get("wo")[0];
	for (const auto& pat : pattern_names)
		weight_out_filenames.push_back(weights_out.GetAbsoluteFilePath() + pat + ".w");

	std::size_t maxIt = 100;
	if (args.Has("maxit"))
		maxIt = std::stoull(args.Get("maxit")[0]);


	const std::size_t n = positions.size();
	const std::size_t m = pattern_names.size();

	if (matrix_filenames.size() != n * m)
		std::cerr << "ERROR: Sizes mismatch!" << std::endl;

	std::cout << "Matrix size: " << matrix_filenames.size() << "\n"
		<< "Vector size: " << n << "\n"
		<< "Weight size: " << m << std::endl;

	std::cout << "Matrices:" << std::endl;
	for (const auto& it : matrix_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	std::cout << "Vectors:" << std::endl;
	for (const auto& it : vector_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	std::cout << "Weights:" << std::endl;
	for (const auto& it : weight_out_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	std::cout << "Initial-Weights:" << std::endl;
	for (const auto& it : weight_in_filenames)
		std::cout << it.GetRelativeFilePath() << ", ";
	std::cout << std::endl;

	if (matrix_filenames.size() != n * m)
	{
		throw std::logic_error("Matrix vector size mismatch");
		return -1;
	}


	Array2D<std::string> arr_matrix_filenames(n, m);
	for (std::size_t i = 0; i < n; i++)
		for (std::size_t j = 0; j < m; j++)
			arr_matrix_filenames(i, j) = matrix_filenames[i * m + j].GetAbsoluteFilePath();

	// Loading data
	// ------------------------
	std::cout << "Loading data...";
	auto startTime = std::chrono::high_resolution_clock::now();

	auto A = CMatrix_CSR_Grid<uint8_t, uint32_t>(arr_matrix_filenames);
	auto b = Vecvec<float>(n);
	for (std::size_t i = 0; i < n; i++)
		b(i) = read_vector<float>(vector_filenames[i]);

	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	// ------------------------

	// Solving data
	// ------------------------
	std::cout << "Solving data...";
	startTime = std::chrono::high_resolution_clock::now();

	Vecvec<float> x0;
	if (weight_in_filenames.empty())
		x0 = Vecvec<float>(A.ms(), 0);
	else
	{
		x0 = Vecvec<float>(m);
		for (std::size_t j = 0; j < m; j++)
			x0(j) = read_vector<float>(weight_in_filenames[j]);
	}
	CDiagonalPreconditioner<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	auto alg = new CIterativePLSQR<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, &P, b, x0, 1);
	alg->Iterate(maxIt);
	auto x = alg->GetX();
	for (std::size_t j = 0; j < m; j++)
		write_to_file(weight_out_filenames[j], x(j));
	endTime = std::chrono::high_resolution_clock::now();
	std::cout << "done. " << time_format(endTime - startTime) << std::endl;
	if (alg->GetTerminationCause() == eTerminationCause::Converged)
		std::cout << "Converged." << std::endl;
	else
		std::cout << "WARNING: Did not converge!" << std::endl;
	// ------------------------

	std::cout << "stddev: " << StandardDeviation(A * x - b) << std::endl;

	return 0;
}

void CreateDefaultWeights(CArgs& args)
{
	const CPath config_file = args.Get("config")[0];
	CConfigurations configs(config_file);

	const CPath weights_out = args.Get("wo")[0];

	std::vector<std::string> pattern_names;
	if (args.Has("pattern"))
		pattern_names = args.Get("pattern");
	else if (configs.Has("active pattern"))
		pattern_names = split(configs.Get("active pattern"), " ");

	std::vector<CPath> weight_out_filenames;
	for (const auto& pat : pattern_names)
		weight_out_filenames.push_back(weights_out.GetAbsoluteFilePath() + pat + ".w");

	std::vector<std::size_t> ms;
	for (const auto& pat : pattern_names)
	{
		uint64_t pattern;
		if (configs.Has("pattern " + pat))
			pattern = std::stoull(configs.Get("pattern " + pat), nullptr, 16);
		else
			std::cerr << "WARNING: Unknown pattern '" << pat << "'." << std::endl;

		auto Pattern = CreatePattern(pattern);
		ms.push_back(Pattern->ReducedSize());
	}

	Vecvec<float> x0(ms, 0);
	for (std::size_t j = 0; j < pattern_names.size(); j++)
		write_to_file(weight_out_filenames[j], x0(j));
}