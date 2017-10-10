#include "utility.h"

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

	     if (millis >= 100) printf(buff, "%4lldms", millis);
	else if (millis >=  10) printf(buff, "%2.1fms", static_cast<double>(micros) / 1000.0);
	else if (millis >=   1) printf(buff, "%1.2fms", static_cast<double>(micros) / 1000.0);
	else if (micros >= 100) printf(buff, "%4lldus", micros);
	else if (micros >=  10) printf(buff, "%2.1fus", static_cast<double>(nanoss) / 1000.0);
	else if (micros >=   1) printf(buff, "%1.2fus", static_cast<double>(nanoss) / 1000.0);
	else if (nanoss >= 100) printf(buff, "%4lldns", nanoss);
	else if (nanoss >=  10) printf(buff, "%2.1fns", static_cast<double>(picos) / 1000.0);
	else if (nanoss >=   1) printf(buff, "%1.2fns", static_cast<double>(picos) / 1000.0);
	else if (picos        ) printf(buff, "%4lldps", picos);
	else
		printf(buff, "Error!");

	return std::string(buff);
}

std::string ThousandsSeparator(uint64_t n)
{
	class MyNumPunct : public std::numpunct<char>
	{
	protected:
		virtual char do_thousands_sep() const { return '\''; }
		virtual std::string do_grouping() const { return "\03"; }
	};

	std::ostringstream oss;
	oss.imbue(std::locale(std::locale::classic(), new MyNumPunct));
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