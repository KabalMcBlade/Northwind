#include "CommandLineParser.h"

#include "../Core/SettingsDefines.h"

UD_NAMESPACE_BEGIN

Option::OptionAllocator* Option::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Generic::kClientOptionAllocatorSize);
	static OptionAllocator memoryAllocator(memoryArea, "ClientOptionAllocator");

	return &memoryAllocator;
}

CommandLineParser::CommandLineParser() {}

CommandLineParser::~CommandLineParser()
{
	eos::Map<udString, Option*, Option::OptionAllocator, Option::GetAllocator>::iterator it;
	for (it = m_options.begin(); it != m_options.end(); ++it)
	{
		eosDelete(it->second, Option::GetAllocator());
	}
	m_options.clear();
}

void CommandLineParser::Add(const udString& _option, bool _mandatory)
{
	Option* opt = eosNew(Option, Option::GetAllocator());
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

bool CommandLineParser::Parse(int32 argc, const char * const argv[])
{
	// Parse the command line and set the value to the option
	// can fail if any errors occurred
	for (int32 i = 1; i < argc; ++i)
	{
		if (strncmp(argv[i], "-", 1) == 0)
		{
			udString name(argv[i]);
			if (m_options.count(name) == 0)
			{
				if (m_options[name]->IsMandatory())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
			else
			{
				m_options[name]->Set();
			}

			if (i + 1 >= argc)
			{
				if (m_options[name]->NeedValue())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (m_options[name]->NeedValue())
				{
					++i;
					if (strncmp(argv[i], "-", 1) != 0)
					{
						m_options[name]->SetStringValue(argv[i]);
					}
					else if (m_options[name]->HasDefault())
					{
						continue;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool CommandLineParser::HasOption(const udString& _option)
{
	return m_options.count(_option) > 0;
}

bool CommandLineParser::HasValue(const udString& _option)
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		return search->second->HasValue();
	}
	else
	{
		return false;
	}
}

bool CommandLineParser::IsSet(const udString& _option)
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		return search->second->IsSet();
	}
	else
	{
		return false;
	}
}

UD_NAMESPACE_END