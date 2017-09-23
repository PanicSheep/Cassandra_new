#include "utility.h"

//ddd:hh:mm:ss.ccc
std::string time_format(const std::chrono::milliseconds duration)
{
	typedef std::chrono::duration<int, std::ratio<24 * 3600> > days_t;
	auto millis = duration.count() % 1000;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;
	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count() % 24;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60;
	auto days = std::chrono::duration_cast<days_t>(duration).count();
	
	std::ostringstream oss;
	oss << std::setfill(' ') << std::setw(3) << days;
	if (days) oss << ":" << std::setfill('0');
	oss << std::setw(2) << hours;
	if (hours) oss << ":" << std::setfill('0');
	oss << minutes;
	if (minutes) oss << ":" << std::setfill('0');
	oss << seconds << "." << std::setfill('0') << std::setw(3) << millis;
	return oss.str();
}

std::string short_time_format(std::chrono::duration<long long, std::pico> duration)
{
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

void replace_all(std::string& source, const std::string& find, const std::string& replace)
{
	std::size_t i = 0;
	while ((i = source.find(find, i)) != std::string::npos)
	{
		source.replace(i, find.length(), replace);
		i += replace.length();
	}
}

std::string GetCurrentWorkingDirectory()
{
	char* cwd = _getcwd(nullptr, 0);
	std::string ret(cwd);
	std::free(cwd);
	return ret;
}