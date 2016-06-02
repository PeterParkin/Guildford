#pragma once

#include <unordered_map>

// Example: C:\Games\Guildford\Game.exe Width=1024 Height=768 Game=GameSpinningCube FullScreen=false 
// Modified the commandline in-place to produce the necessary strings.
//
class CommandLineInfo
{
public:
	CommandLineInfo(char* command_line);
	CommandLineInfo(CommandLineInfo&& other)
		: _CommandLine(other._CommandLine)
		, _KeyValuePairs(std::move(other._KeyValuePairs))
	{
		other._CommandLine = nullptr;
	}
	template<typename T>
	T Get(const char* key, T _default) const
	{
		static_assert(false, "Type not supported");
	}
private:
	const char* _CommandLine;
	std::vector<std::pair<const char*, const char*>> _KeyValuePairs;
	const char* Value(const char* key) const;
};

extern template int CommandLineInfo::Get<int>(const char* key, int _default) const;
extern template bool CommandLineInfo::Get<bool>(const char* key, bool _default) const;
extern template float CommandLineInfo::Get<float>(const char* key, float _default) const;
extern template const char* CommandLineInfo::Get<const char*>(const char* key, const char* _default) const;
