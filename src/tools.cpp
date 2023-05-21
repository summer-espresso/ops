#include "tools.hpp"
#include "globals.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>

#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

using vec_string_t = std::vector<std::string>;
using vec_json_t = std::vector<Json::Value>;

namespace glo
{
}; // namespace glo

// --------------------------------------------------------------------

#if (JSONCPP_VERSION_MAJOR==1 && JSONCPP_VERSION_MINOR<=7)

Json::Value decode_json(const std::string value)
{
	Json::Value root;

	try
	{
		Json::Reader reader;
		if (!reader.parse(value, root))
		{
			std::cerr << "error parsing json " << value << "\n";
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "error reading json " << value << "\n";
	}

	return root;
}

Json::Value load_json(const std::string filename)
{
	std::string value = load_content(filename);
	Json::Value root;

	try
	{
		Json::Reader reader;
		if (!reader.parse(value, root))
		{
			std::cerr << "error parsing json from file " << filename << "\n";
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "error reading json from file " << filename << "\n";
	}

	return root;
}

#elif (JSONCPP_VERSION_MAJOR==1 && JSONCPP_VERSION_MINOR>7)

// --------------------------------------------------------------------

Json::Value decode_json(const std::string value)
{
// 	https://github.com/open-source-parsers/jsoncpp/issues/815

// 	Json::Value root;
// 	JSONCPP_STRING errs;
// 	const auto len = static_cast<int>(value.length());
// 	Json::CharReaderBuilder builder;
// 	builder["collectComments"] = false;
// 	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
// 	if (!reader->parse(value.c_str(), value.c_str() + len, &root, &errs))
// 	{
// 		std::cerr << "decode_json error" << "\n";
// 	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

	Json::Value root;

	try
	{
		Json::Reader reader;
		if (!reader.parse(value, root, false))
		{
			std::cerr << "error parsing json " << value << "\n";
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "error reading json " << value << "\n";
	}

#pragma GCC diagnostic pop

	return root;
}

// --------------------------------------------------------------------

Json::Value load_json(const std::string filename)
{
	Json::Value root;

	try
	{
		std::ifstream ifs(filename);
		if (ifs.is_open())
		{
			Json::CharReaderBuilder builder;
			builder["collectComments"] = false;
			JSONCPP_STRING errs;
			if (!parseFromStream(builder, ifs, &root, &errs))
			{
				std::cerr << "error parsing json from file " << filename << "\n";
			}
			ifs.close();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "error reading json from file " << filename << "\n";
	}

	return root;
}

#else
#error "JSONCPP Version not supported"
#endif

// --------------------------------------------------------------------

void save_json(const std::string filename, Json::Value & node)
{
	std::ofstream ofs(filename);
	if (ofs.is_open())
	{
		ofs << node;
		ofs.close();
	}
}

// --------------------------------------------------------------------

// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c

std::string load_content(const std::string filename)
{
	// https://github.com/CrowCpp/Crow/blob/4a6d5fe74c05d87ba8742a90666dce565848fbe3/tests/template/mustachetest.cpp
	std::ifstream is(filename);
	if (is.is_open())
	{
		return { std::istreambuf_iterator<char> (is), std::istreambuf_iterator<char>() };
	}
	return "";
}

// --------------------------------------------------------------------

bool dir_exists(const std::string folder)
{
	struct stat sb;
	return (stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

// --------------------------------------------------------------------

bool file_exists(const std::string filename)
{
	struct stat sb;
	return (stat(filename.c_str(), &sb) == 0 && S_ISREG(sb.st_mode));
}

// --------------------------------------------------------------------

vector_string_t tokenize_string(const std::string & source_string, const char separator)
{
	vector_string_t l;
	std::string s;
	std::istringstream i (source_string);

	std::getline(i, s, separator);

	while (!i.eof())
	{
		l.insert (l.end(), s);
		std::getline(i, s, separator);
	}

	if (s != "")
	{
		l.insert (l.end(), s);
	}

	return l;
}

// --------------------------------------------------------------------

std::string get_args (Json::Value & param, Json::Value & ope)
{
	return "";
}

// --------------------------------------------------------------------

int log_exec (const std::string cmd)
{
	int exit_code = 0;
	const int BUF_SIZE = 1024;
	static thread_local char buf[BUF_SIZE];
	FILE *fp = nullptr;

	if ((fp = popen((cmd + " 2>&1").c_str(), "r")) != nullptr)
	{
		while (fgets(buf, BUF_SIZE, fp) != nullptr)
		{
			std::cout << buf << std::flush;
		}
		exit_code = WEXITSTATUS(pclose(fp));
	}
	else
	{
		exit_code = -errno;
	}

	return exit_code;
}

// --------------------------------------------------------------------
