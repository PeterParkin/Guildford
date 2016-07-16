#pragma once
/*
Keys and values are case sensitive.  Example file:-
//
// Always good to put a header in.  The semicolon comments stuff out.
// Copyright (c) Peter Parkin, 2016.  Some rights reserved.
// No claiming this work as your own!
//
MouseSensitivity=0.1 // I can put a comment here!
// CommentedOut=true
Something_Else = True
		IndentsAndSpacesConsumed   =		True
Oh Bugger, this doesn't parse	=	false
*/
//
// Not thread safe - use std::lock_guard<Config> lock(config) to make it so.
// The performance implications were too severe.
// Probably best to access config data some time after creation, or
// it'll stall like a bastard.
// Implicitly Saves on exit, can also explicity call Reload and Save.
// Reload wipes all existing data.
// Need a way to monitor file change notifications, and have it respond.
// Could also work with Events/Delegates to provide data change notifications.
// I would prefer a centralized property system.
//
#include <unordered_map>
#include <mutex>
#include <stdarg.h>
#include <unordered_set>

namespace Eng
{
	class ConfigKeyValuePair
	{
		friend class Config; // Check the key is the key!
	public:
		ConfigKeyValuePair(const char* key, const char* value, unsigned line = -1) : _Key(key), _Value(value), _Line(line) {}
		ConfigKeyValuePair(ConfigKeyValuePair&& other)
			: _Key(std::move(other._Key))
			, _Value(std::move(other._Value))
			, _Line(other._Line)
		{
		}

		operator bool() const
		{
			// Fast, but a bit hacky...
			const unsigned& true1u = *reinterpret_cast<const unsigned*>("true");
			const unsigned& true2u = *reinterpret_cast<const unsigned*>("True");
			const unsigned& true3u = *reinterpret_cast<const unsigned*>("TRUE");
			const unsigned& valueu = *reinterpret_cast<const unsigned*>(_Value.c_str());
			return valueu == true1u || valueu == true2u || valueu == true3u || _Value[0] == 1;
		}
		operator float() const
		{
			return strtof(_Value.c_str(), nullptr);
		}
		operator int() const
		{
			return strtol(_Value.c_str(), nullptr, 0);
		}
		operator unsigned() const
		{
			return strtoul(_Value.c_str(), nullptr, 0);
		}
		operator const char*() const
		{
			return _Value.c_str();
		}
		template<typename T>
		T operator=(T value)
		{
			_Value = value;
			return value;
		}

	private:
		// When _Key is empty, _Value is interpreted as a comment in the config file.
		std::string _Key;
		std::string _Value;
		unsigned _Line; // Record the line in the config file, or -1 for unknown.
	};

	class Config
	{
	public:
		Config(const char* filename)
		{
			// Set filename.
			int error = strcpy_s(_Filename, filename);
			switch (error)
			{
				case ERANGE:
					ENGINE_LOG(Warning, "filename too big.");
				return;
				case EINVAL:
					ENGINE_LOG(Warning, "filename is nullptr");
				return;
			}
		}
		// Throws an exception if the data cannot be saved?
		ENGINE_API ~Config();
		ENGINE_API void Load();
		ENGINE_API void Save();
		void lock()
		{
			_Mutex.lock();
		}
		bool try_lock()
		{
			return _Mutex.try_lock();
		}
		void unlock()
		{
			_Mutex.unlock();
		}
		ConfigKeyValuePair& operator[](const char* key)
		{
			unsigned hash = Engine::CreateHash32(key);
			auto iter = _MapHashToKeyValuePair.find(hash);
			if (iter == _MapHashToKeyValuePair.end())
			{
				iter = _MapHashToKeyValuePair.emplace(hash, ConfigKeyValuePair(key, "0")).first;
			}
			else
			{
				// Found an entry, just need an assert for a hash clash.
				ENGINE_ASSERT(strcmp(key, iter->second._Key.c_str()) == 0);
			}
			return iter->second;
		}
		const ConfigKeyValuePair& operator[](const char* key) const
		{
			unsigned hash = Engine::CreateHash32(key);
			auto iter = _MapHashToKeyValuePair.find(hash);
			if (iter == _MapHashToKeyValuePair.end())
			{
				ENGINE_LOG(Warning, "Unable to find %s in config", key);
			}
			return iter->second;
		}
	private:
		// Set a key/value pair using const char* variable argument list.
		ENGINE_API void SetDataInternal(int num_params, ...);

		// Private member data
		std::unordered_map<unsigned, ConfigKeyValuePair> _MapHashToKeyValuePair;
		char _Filename[ENGINE_MAX_PATH];
		std::mutex _Mutex;

		// TODO: Fix hack.  Statics have problems with linkage - initialize with engine?
		std::string _EmptyString;
	};
}
