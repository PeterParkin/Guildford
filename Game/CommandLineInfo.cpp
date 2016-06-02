#include <string.h>
#include "CommandLineInfo.h"

CommandLineInfo::CommandLineInfo(char* command_line)
{
	const char* word = command_line;
	std::pair<const char*, const char*> pair(nullptr, nullptr);
	for (auto iter = command_line; *iter != NULL; ++iter)
	{
		if (*iter == '=')
		{
			*iter = NULL;
			pair.first = word;
			word = iter + 1;
		}
		else if (*iter == ' ')
		{
			if (pair.first != nullptr)
			{
				// Seperator between key/value pair.
				*iter = NULL;
				pair.second = word;
				word = iter + 1;
				_KeyValuePairs.push_back(pair);
				pair.first = pair.second = nullptr;
			}
			else
			{
				// Eat Whitespace before key.
				++word;
			}
		}
	}
	if (pair.first != nullptr)
	{
		pair.second = word;
		_KeyValuePairs.push_back(pair);
	}
}

const char* CommandLineInfo::Value(const char* key) const
{
	for (auto iter = _KeyValuePairs.cbegin(); iter != _KeyValuePairs.cend(); ++iter)
	{
		if (strcmp(iter->first, key) == 0)
		{
			return iter->second;
		}
	}
	return nullptr;
}

template<> int CommandLineInfo::Get<int>(const char* key, int _default) const
{
	const char* value = Value(key);
	if (value)
	{
		return atoi(value);
	}
	return _default;
}

template<> bool CommandLineInfo::Get<bool>(const char* key, bool _default) const
{
	const char* value = Value(key);
	if (value)
	{
		if ((strcmp("true", value) == 0) || (strcmp("1", value) == 0 ))
		{
			return true;
		}
		if ((strcmp("false", value) == 0) || (strcmp("0", value) == 0))
		{
			return false;
		}
	}
	return _default;
}

template<> float CommandLineInfo::Get<float>(const char* key, float _default) const
{
	const char* value = Value(key);
	if (value)
	{
		return (float)atof(value);
	}
	return _default;
}

template<> const char* CommandLineInfo::Get<const char*>(const char* key, const char* _default) const
{
	const char* value = Value(key);
	if (value)
	{
		return value;
	}
	return _default;
}
