#include "InputHandler.h"
#include "CText.h"

#include <string>


InputHandler::InputHandler(int argc, char ** argv)
	: DEFAULT_OPTION("input"),
	FILE_EXT(R"(((?:[^/]*/))*(.*)\.(\w+)$)")
//	FILE_EXT(R"(.*\/?(.*)\.(\w+)$)") // TODO: not "Raw string"
{
#ifdef _DEBUG
	std::cout << " -> [InputHandler]: Parsing input for executable '" << CText(argv[0], CText::Color::GREEN) << '\'' << std::endl;
#endif // DEBUG

	std::string option = DEFAULT_OPTION;
	std::vector<std::string> arguments;

	size_t i = 1;
	while (true)
	{
		// add arguments
		if (i < static_cast<size_t>(argc) && argv[i][0] != '-')
		{
			arguments.push_back(argv[i]);
		}

		else
		{
			// try insert into map
			bool emplaced;
			std::unordered_map <std::string, std::vector<std::string>>::iterator it;
			std::tie(it, emplaced) = option_arguments.emplace(option, arguments);

			// when key exists update it
			if (!emplaced)
			{
				for (auto &a : arguments)
					(*it).second.push_back(a);
			}

			if (i == static_cast<size_t>(argc))
				break;

			// clean arguments for new option
			arguments.clear();
			option = argv[i];
			option.erase(0, 1);
		}
		++i;
	}
}


bool InputHandler::isset(const std::vector<std::string> &options)
{
	for (const auto &opt : options)
	{
		if (option_arguments.count(opt) == 1)
		{
			used.emplace(opt);
			return true;
		}
	}
	return false;
}

bool InputHandler::isset(const char * option)
{
	if (option_arguments.count(option) == 1)
	{
		used.emplace(option);
		return true;
	}
	return false;
}

std::vector<std::string> InputHandler::get(const std::string &option)
{
	auto search = option_arguments.find(option);
	if (!used.count(option) && search != option_arguments.end())
	{
		used.emplace(option);
		return std::move((*search).second);
	}
	return std::vector<std::string>();
}

std::vector<std::string> InputHandler::get(const char * option)
{
	return get(std::string(option));
}

bool InputHandler::empty() const
{
	return option_arguments.size() == used.size();
}

std::tuple<bool, std::string, std::string, std::string> InputHandler::match_extensions(const std::string & filepath, const std::vector<std::string>& extensions) const
{
	bool matched = false;
	std::smatch match;

	if (std::regex_match(filepath, match, FILE_EXT))
	{
		std::string ext = match[3].str();
		for (const auto &e : extensions)
		{
			if (ext == e)
			{
				matched = true;
				break;
			}
		}
		return std::make_tuple(matched, match[1].str(), match[2].str(), ext);
	}

	return std::make_tuple(matched, std::string(""), std::string(""), std::string(""));
}

void InputHandler::print(std::ostream & o)
{
	o << "\n";
	for (const auto &vec : option_arguments)
	{
		o << "     -> " << CText(vec.first, CText::Color::CYAN) << std::endl;
		for (const auto &s : vec.second)
		{
			o << "        - " << s << std::endl;
		}
	}
	o << std::endl;
}
