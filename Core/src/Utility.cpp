#include "Utility.h"

int64_t Pow_int(int64_t base, uint64_t exponent)
{
	if (exponent == 0)
		return 1;
	else if (exponent % 2 == 0)
		return Pow_int(base * base, exponent / 2);
	else
		return base * Pow_int(base, exponent - 1);
}

//ddd:hh:mm:ss.ccc
std::string time_format(const std::chrono::milliseconds duration)
{
	typedef std::chrono::duration<int, std::ratio<24 * 3600> > days_t;
	const auto millis  = duration.count() % 1000;
	const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;
	const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60;
	const auto hours   = std::chrono::duration_cast<std::chrono::hours>  (duration).count() % 24;
	const auto days    = std::chrono::duration_cast<days_t>              (duration).count();

	std::ostringstream oss;
	oss << std::setfill(' ');

	if (days)
		oss << std::setw(3) << days << ":" << std::setfill('0');
	else
		oss << "    ";

	if (days || hours)
		oss << std::setw(2) << hours << ":" << std::setfill('0');
	else
		oss << "   ";

	if (days || hours || minutes)
		oss << std::setw(2) << minutes << ":" << std::setfill('0');
	else
		oss << "   ";

	oss << std::setw(2) << seconds << "." << std::setfill('0') << std::setw(3) << millis;

	return oss.str();
}

std::string short_time_format(std::chrono::duration<long long, std::pico> duration)
{
	// TODO: Refactor and test!
	char buff[16];

	unsigned long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	unsigned long long micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	unsigned long long nanoss = std::chrono::duration_cast<std::chrono::nanoseconds >(duration).count();
	unsigned long long picos = duration.count();

	     if (millis >= 100) sprintf(buff, "%4lldms", millis);
	else if (millis >=  10) sprintf(buff, "%2.1fms", static_cast<double>(micros) / 1000.0);
	else if (millis >=   1) sprintf(buff, "%1.2fms", static_cast<double>(micros) / 1000.0);
	else if (micros >= 100) sprintf(buff, "%4lldus", micros);
	else if (micros >=  10) sprintf(buff, "%2.1fus", static_cast<double>(nanoss) / 1000.0);
	else if (micros >=   1) sprintf(buff, "%1.2fus", static_cast<double>(nanoss) / 1000.0);
	else if (nanoss >= 100) sprintf(buff, "%4lldns", nanoss);
	else if (nanoss >=  10) sprintf(buff, "%2.1fns", static_cast<double>(picos) / 1000.0);
	else if (nanoss >=   1) sprintf(buff, "%1.2fns", static_cast<double>(picos) / 1000.0);
	else if (picos        ) sprintf(buff, "%4lldps", picos);
	else
		sprintf(buff, "Error!");

	return std::string(buff);
}

std::string ThousandsSeparator(uint64_t n)
{
	std::ostringstream oss;
	oss.imbue(std::locale(""));
	oss << n;
	return oss.str();
}

std::string DateTimeNow()
{
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	return std::string(std::ctime(&t));
}

std::string GetCurrentWorkingDirectory()
{
	char * cwd = getcwd(nullptr, 0);
	std::string ret(cwd);
	std::free(cwd);
	return ret;
}

void replace_all(std::string& source, const std::string& from, const std::string& to)
{
	assert(!from.empty());

	for (std::size_t i = source.find(from); i != std::string::npos; i = source.find(from, i + to.length()))
		source.replace(i, from.length(), to);
}

std::vector<std::string> split(const std::string& src, const std::string& deli)
{
	assert(!deli.empty());
	std::vector<std::string> vec;

	std::size_t begin = 0;
	std::size_t end = src.find(deli);
	while (end != std::string::npos) {
		vec.push_back(src.substr(begin, end-begin));
		begin = end + deli.length();
		end = src.find(deli, begin);
	}
	vec.push_back(src.substr(begin));

	return vec;
}

std::string join(const std::vector<std::string>& parts, const std::string& deli)
{
	std::string str;
	for (std::size_t i = 0; i+1 < parts.size(); i++)
		str += parts[i] + deli;
	if (parts.size() > 0)
		str += parts.back();
	return str;
}

std::string SignedInt(int score)
{
	const std::string Sign = (score >= 0) ? "+" : "-";
	const std::string Number = std::to_string(std::abs(score));
	return Sign + Number;
}

std::string DoubleDigitSignedInt(int score)
{
	const std::string Sign = (score >= 0) ? "+" : "-";
	const std::string FillingZero = (std::abs(score) < 10) ? "0" : "";
	const std::string Number = std::to_string(std::abs(score));
	return Sign + FillingZero + Number;
}

std::size_t ParseBytes(const std::string& bytes)
{
	if (bytes.find("EB") != std::string::npos) return std::stoll(bytes) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
	if (bytes.find("PB") != std::string::npos) return std::stoll(bytes) * 1024 * 1024 * 1024 * 1024 * 1024;
	if (bytes.find("TB") != std::string::npos) return std::stoll(bytes) * 1024 * 1024 * 1024 * 1024;
	if (bytes.find("GB") != std::string::npos) return std::stoll(bytes) * 1024 * 1024 * 1024;
	if (bytes.find("MB") != std::string::npos) return std::stoll(bytes) * 1024 * 1024;
	if (bytes.find("kB") != std::string::npos) return std::stoll(bytes) * 1024;
	if (bytes.find( "B") != std::string::npos) return std::stoll(bytes);
	return 0;
}

uint64_t FlipCodiagonal(uint64_t b)
{
	// 9 x XOR, 6 x SHIFT, 3 x AND
	// 18 OPs

	// # # # # # # # /
	// # # # # # # / #
	// # # # # # / # #
	// # # # # / # # #
	// # # # / # # # #
	// # # / # # # # #
	// # / # # # # # #
	// / # # # # # # #<-LSB
	uint64_t t;
	t  =  b ^ (b << 36);
	b ^= (t ^ (b >> 36)) & 0xF0F0F0F00F0F0F0FULL;
	t  = (b ^ (b << 18)) & 0xCCCC0000CCCC0000ULL;
	b ^=  t ^ (t >> 18);
	t  = (b ^ (b <<  9)) & 0xAA00AA00AA00AA00ULL;
	b ^=  t ^ (t >>  9);
	return b;
}

uint64_t FlipDiagonal(uint64_t b)
{
	// 9 x XOR, 6 x SHIFT, 3 x AND
	// 18 OPs

	// \ # # # # # # #
	// # \ # # # # # #
	// # # \ # # # # #
	// # # # \ # # # #
	// # # # # \ # # #
	// # # # # # \ # #
	// # # # # # # \ #
	// # # # # # # # \.<-LSB
	uint64_t t;
	t  = (b ^ (b >>  7)) & 0x00AA00AA00AA00AAULL;
	b ^=  t ^ (t <<  7);
	t  = (b ^ (b >> 14)) & 0x0000CCCC0000CCCCULL;
	b ^=  t ^ (t << 14);
	t  = (b ^ (b >> 28)) & 0x00000000F0F0F0F0ULL;
	b ^=  t ^ (t << 28);
	return b;
}

uint64_t FlipHorizontal(uint64_t b)
{
	// 6 x SHIFT, 6 x AND, 3 x OR
	// 15 OPs

	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #<-LSB
	b = ((b >> 1) & 0x5555555555555555ULL) | ((b << 1) & 0xAAAAAAAAAAAAAAAAULL);
	b = ((b >> 2) & 0x3333333333333333ULL) | ((b << 2) & 0xCCCCCCCCCCCCCCCCULL);
	b = ((b >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((b << 4) & 0xF0F0F0F0F0F0F0F0ULL);
	return b;
}

uint64_t FlipVertical(uint64_t b)
{
	// 1 x BSwap
	// 1 OPs

	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// ---------------
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #<-LSB
	return BSwap(b);
	//b = ((b >>  8) & 0x00FF00FF00FF00FFULL) | ((b <<  8) & 0xFF00FF00FF00FF00ULL);
	//b = ((b >> 16) & 0x0000FFFF0000FFFFULL) | ((b << 16) & 0xFFFF0000FFFF0000ULL);
	//b = ((b >> 32) & 0x00000000FFFFFFFFULL) | ((b << 32) & 0xFFFFFFFF00000000ULL);
	//return b;
}