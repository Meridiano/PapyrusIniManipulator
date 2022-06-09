namespace PIMUtility
{
	std::string CurrentDateTime()
	{
		time_t rawtime = time(nullptr);
		time(&rawtime);
		struct tm timeinfo;
		localtime_s(&timeinfo, &rawtime);
		char buffer[32];
		strftime(buffer, sizeof(buffer), "%d.%m.%Y %X", &timeinfo);
		return buffer;
	}

	std::string MergeStrings(
		std::string a = "", std::string b = "",
		std::string c = "", std::string d = "",
		std::string e = "", std::string f = "",
		std::string g = "", std::string h = "",
		std::string i = "", std::string j = "")
	{
		std::vector<std::string> vector = { a, b, c, d, e, f, g, h, i, j };
		std::string result = "";
		for (std::string add : vector)
		{
			result += add;
		}
		return result;
	}

	void SKSEPluginLog(int level = 0,
		std::string a = "", std::string b = "",
		std::string c = "", std::string d = "",
		std::string e = "", std::string f = "",
		std::string g = "", std::string h = "",
		std::string i = "", std::string j = "")
	{
		std::string message = MergeStrings(a, b, c, d, e, f, g, h, i, j);
		if (level == 0)
		{
			INFO(message.c_str());
		}
		else if (level == 1)
		{
			ERROR(message.c_str());
		}
		return;
	}

	std::string PluginConfigPath()
	{
		return MergeStrings("Data\\SKSE\\Plugins\\", Version::PROJECT.data(), ".ini");
	}

	std::string BoolToString(bool value)
	{
		return (value ? "true" : "false");
	}

	std::string StringToLower(std::string toConvert)
	{
		std::transform(toConvert.begin(), toConvert.end(), toConvert.begin(), ::tolower);
		return toConvert;
	}

	bool StringToBool(std::string value, bool defaultValue)
	{
		std::string lower = StringToLower(value);
		if (lower.compare("true") == 0 || lower.compare("1") == 0)
		{
			return true;
		}
		else if (lower.compare("false") == 0 || lower.compare("0") == 0)
		{
			return false;
		}
		return defaultValue;
	}

	bool StringEndsWith(std::string input, std::string end)
	{
		size_t il = input.length();
		size_t el = end.length();
		if (il < el)
		{
			return false;
		}
		int result = StringToLower(input).compare(il - el, el, StringToLower(end));
		return (result == 0 ? true : false);
	}
	
	bool FileExists(std::string path, bool create)
	{
		std::filesystem::path target{ path };
		if (std::filesystem::exists(target))
		{
			return true;
		}
		if (create)
		{
			std::filesystem::path target_root = target.root_path();
			std::filesystem::path target_parent = target.parent_path();
			if (target_root.compare(target_parent) != 0) // need to create folders
			{
				if (!target_root.string().empty() && !std::filesystem::exists(target_root)) // path is absolute but local drive not found
				{
					return false;
				}
				if (!std::filesystem::create_directories(target_parent)) // could not create folders
				{
					return false;
				}
			}
			std::ofstream file(path);
			file << "";
			return std::filesystem::exists(target);
		}
		return false;
	}
}

namespace PIMInternal
{
	// UsePrettyPrint() function declaration (definition is below)
	bool UsePrettyPrint();

	bool IniDataExistsInternal(std::int32_t level, std::string path, std::string section, std::string key)
	{
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini))
		{
			if (level == 0)
			{
				return true;
			}
			else if (level == 1)
			{
				return ini.has(section);
			}
			else if (level == 2)
			{
				return (ini.has(section) && ini.get(section).has(key));
			}
			return false;
		}
		return false;
	}

	bool ClearIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key)
	{
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini))
		{
			if (level == 0)
			{
				ini.clear();
				file.write(ini, UsePrettyPrint());
				return true;
			}
			else if ((level == 1) && ini.has(section))
			{
				ini[section].clear();
				file.write(ini, UsePrettyPrint());
				return true;
			}
			else if ((level == 2) && ini.has(section) && ini.get(section).has(key))
			{
				ini[section][key] = "";
				file.write(ini, UsePrettyPrint());
				return true;
			}
			return false;
		}
		return false;
	}

	bool DestroyIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key)
	{
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini))
		{
			if (level == 0)
			{
				std::filesystem::path target{ path };
				return std::filesystem::remove(target);
			}
			else if ((level == 1) && ini.has(section))
			{
				ini.remove(section);
				file.write(ini, UsePrettyPrint());
				return true;
			}
			else if ((level == 2) && ini.has(section) && ini.get(section).has(key))
			{
				ini[section].remove(key);
				file.write(ini, UsePrettyPrint());
				return true;
			}
			return false;
		}
		return false;
	}

	std::vector<std::string> GetIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key)
	{
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini))
		{
			if (level == 0)
			{
				std::vector<std::string> result;
				for (std::pair iterator : ini)
				{
					result.push_back(iterator.first);
				}
				return result;
			}
			else if ((level == 1) && ini.has(section))
			{
				std::vector<std::string> result;
				for (std::pair iterator : ini.get(section))
				{
					result.push_back(iterator.first);
				}
				return result;
			}
			else if ((level ==  2) && ini.has(section) && ini.get(section).has(key))
			{
				std::vector<std::string> result;
				for (char ch : ini.get(section).get(key))
				{
					std::string ch_s(1, ch);
					result.push_back(ch_s);
				}
				return result;
			}
			std::vector<std::string> result; return result;
		}
		std::vector<std::string> result; return result;
	}

	std::string PullStringFromIniInternal(std::string path, std::string section, std::string key, std::string defaultValue)
	{
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini) && ini.has(section) && ini.get(section).has(key))
		{
			return ini.get(section).get(key);
		}
		return defaultValue;
	}

	bool PullBoolFromIniInternal(std::string path, std::string section, std::string key, bool defaultValue)
	{
		std::string raw = PullStringFromIniInternal(path, section, key, PIMUtility::BoolToString(defaultValue));
		return PIMUtility::StringToBool(raw, defaultValue);
	}

	std::int32_t PullIntFromIniInternal(std::string path, std::string section, std::string key, std::int32_t defaultValue)
	{
		std::string raw = PullStringFromIniInternal(path, section, key, std::to_string(defaultValue));
		std::int32_t result;
		try
		{
			result = std::stol(raw);
		}
		catch (...)
		{
			return defaultValue;
		}
		return result;
	}

	float PullFloatFromIniInternal(std::string path, std::string section, std::string key, float defaultValue)
	{
		std::string raw = PullStringFromIniInternal(path, section, key, std::to_string(defaultValue));
		float result;
		try
		{
			result = std::stof(raw);
		}
		catch (...)
		{
			return defaultValue;
		}
		return result;
	}

	bool PushStringToIniInternal(std::string path, std::string section, std::string key, std::string value, bool force)
	{
		if (!PIMUtility::FileExists(path, force))
		{
			return false;
		}
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (!file.read(ini)) // process file
		{
			if (force)
			{
				file.generate(ini, UsePrettyPrint());
			}
			else
			{
				return false;
			}
		}
		if (section.length() == 0) // exit if section is ""
		{
			file.write(ini, UsePrettyPrint());
			return true;
		}
		if (!ini.has(section)) // process section
		{
			if (force)
			{
				ini[section];
			}
			else
			{
				return false;
			}
		}
		if (key.length() == 0) // exit if key is ""
		{
			file.write(ini, UsePrettyPrint());
			return true;
		}
		if (!ini.get(section).has(key)) // process key
		{
			if (force)
			{
				ini[section][key];
			}
			else
			{
				return false;
			}
		}
		if (value.length() == 0) // exit if value is ""
		{
			file.write(ini, UsePrettyPrint());
			return true;
		}
		// finally set new value
		ini[section][key] = value;
		file.write(ini, UsePrettyPrint());
		return true;
	}

	// UsePrettyPrint() function definition (declaration is above)
	bool UsePrettyPrint()
	{
		return PIMInternal::PullBoolFromIniInternal(PIMUtility::PluginConfigPath(), "General", "bUsePrettyPrint", false);
	}

	bool PushBoolToIniInternal(std::string path, std::string section, std::string key, bool value, bool force)
	{
		return PushStringToIniInternal(path, section, key, PIMUtility::BoolToString(value), force);
	}

	bool PushIntToIniInternal(std::string path, std::string section, std::string key, std::int32_t value, bool force)
	{
		return PushStringToIniInternal(path, section, key, std::to_string(value), force);
	}

	bool PushFloatToIniInternal(std::string path, std::string section, std::string key, float value, bool force)
	{
		return PushStringToIniInternal(path, section, key, std::to_string(value), force);
	}
}

namespace PIMPapyrus
{
	std::string GetVersion(RE::StaticFunctionTag* base)
	{
		return Version::NAME.data();
	}

	bool IniDataExists(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key)
	{
		return PIMInternal::IniDataExistsInternal(level, path, section, key);
	}

	bool ClearIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key)
	{
		return PIMInternal::ClearIniDataInternal(level, path, section, key);
	}

	bool DestroyIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key)
	{
		return PIMInternal::DestroyIniDataInternal(level, path, section, key);
	}

	std::vector<std::string> GetIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key)
	{
		return PIMInternal::GetIniDataInternal(level, path, section, key);
	}

	std::string PullStringFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::string defaultValue)
	{
		return PIMInternal::PullStringFromIniInternal(path, section, key, defaultValue);
	}

	bool PullBoolFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, bool defaultValue)
	{
		return PIMInternal::PullBoolFromIniInternal(path, section, key, defaultValue);
	}

	std::int32_t PullIntFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::int32_t defaultValue)
	{
		return PIMInternal::PullIntFromIniInternal(path, section, key, defaultValue);
	}

	float PullFloatFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, float defaultValue)
	{
		return PIMInternal::PullFloatFromIniInternal(path, section, key, defaultValue);
	}

	bool PushStringToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::string value, bool force)
	{
		return PIMInternal::PushStringToIniInternal(path, section, key, value, force);
	}

	bool PushBoolToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, bool value, bool force)
	{
		return PIMInternal::PushBoolToIniInternal(path, section, key, value, force);
	}

	bool PushIntToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::int32_t value, bool force)
	{
		return PIMInternal::PushIntToIniInternal(path, section, key, value, force);
	}

	bool PushFloatToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, float value, bool force)
	{
		return PIMInternal::PushFloatToIniInternal(path, section, key, value, force);
	}
}

namespace PapyrusManager
{
	bool Register(RE::BSScript::IVirtualMachine* virtualMachine)
	{
		// general
		virtualMachine->RegisterFunction("GetVersion", "PapyrusIniManipulator", PIMPapyrus::GetVersion);

		// basic
		virtualMachine->RegisterFunction("IniDataExists", "PapyrusIniManipulator", PIMPapyrus::IniDataExists);
		virtualMachine->RegisterFunction("ClearIniData", "PapyrusIniManipulator", PIMPapyrus::ClearIniData);
		virtualMachine->RegisterFunction("DestroyIniData", "PapyrusIniManipulator", PIMPapyrus::DestroyIniData);
		virtualMachine->RegisterFunction("GetIniData", "PapyrusIniManipulator", PIMPapyrus::GetIniData);

		// pullers
		virtualMachine->RegisterFunction("PullStringFromIni", "PapyrusIniManipulator", PIMPapyrus::PullStringFromIni);
		virtualMachine->RegisterFunction("PullBoolFromIni", "PapyrusIniManipulator", PIMPapyrus::PullBoolFromIni);
		virtualMachine->RegisterFunction("PullIntFromIni", "PapyrusIniManipulator", PIMPapyrus::PullIntFromIni);
		virtualMachine->RegisterFunction("PullFloatFromIni", "PapyrusIniManipulator", PIMPapyrus::PullFloatFromIni);

		// pushers
		virtualMachine->RegisterFunction("PushStringToIni", "PapyrusIniManipulator", PIMPapyrus::PushStringToIni);
		virtualMachine->RegisterFunction("PushBoolToIni", "PapyrusIniManipulator", PIMPapyrus::PushBoolToIni);
		virtualMachine->RegisterFunction("PushIntToIni", "PapyrusIniManipulator", PIMPapyrus::PushIntToIni);
		virtualMachine->RegisterFunction("PushFloatToIni", "PapyrusIniManipulator", PIMPapyrus::PushFloatToIni);

		return true;
	}
}

namespace PIMConsole
{
	void ConsolePrint(std::string text)
	{
		auto console = RE::ConsoleLog::GetSingleton();
		if (console && console->IsConsoleMode())
		{
			console->Print(text.c_str());
		}
	}

	bool ExecutePVFI(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
	{
		std::vector<std::string> arguments = { "", "", "" };
		auto stringChunk = a_scriptData->GetStringChunk();
		for (int index = 0; index < a_scriptData->numParams; index++)
		{
			arguments[index] = stringChunk->GetString();
			stringChunk = stringChunk->GetNext()->AsString();
		}
		std::string path = arguments[0];
		std::string section = arguments[1];
		std::string key = arguments[2];
		ConsolePrint(PIMUtility::MergeStrings("    Path = ", path, "\n    Section = ", section, "\n    Key = ", key));
		if (PIMInternal::IniDataExistsInternal(2, path, section, key))
		{
			ConsolePrint(PIMUtility::MergeStrings("        Value = ", PIMInternal::PullStringFromIniInternal(path, section, key, "")));
			return true;
		}
		ConsolePrint("        Could not find this key.");
		return false;
	}

	bool ExecutePVTI(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
	{
		std::vector<std::string> arguments = { "", "", "", "", "" };
		auto stringChunk = a_scriptData->GetStringChunk();
		for (int index = 0; index < a_scriptData->numParams; index++)
		{
			arguments[index] = stringChunk->GetString();
			stringChunk = stringChunk->GetNext()->AsString();
		}
		std::string path = arguments[0];
		std::string section = arguments[1];
		std::string key = arguments[2];
		std::string value = arguments[3];
		bool force = PIMUtility::StringToBool(arguments[4], false);
		ConsolePrint(PIMUtility::MergeStrings("    Path = ", path, "\n    Section = ", section, "\n    Key = ", key, "\n    Value = ", value, "\n    Force = ", PIMUtility::BoolToString(force)));
		if (PIMInternal::IniDataExistsInternal(2, path, section, key) || force)
		{
			bool result = PIMInternal::PushStringToIniInternal(path, section, key, value, force);
			if (result)
			{
				ConsolePrint("        Value has been pushed successfully.");
				return true;
			}
			ConsolePrint("        Could not push value to this path/section/key.");
			return false;
		}
		ConsolePrint("        Could not find this path/section/key.\n        Push without force canceled.");
		return false;
	}
}

namespace ConsoleManager
{
	bool RegisterPVFI()
	{
		using Type = RE::SCRIPT_PARAM_TYPE;
		std::string toSteal = PIMInternal::PullStringFromIniInternal(PIMUtility::PluginConfigPath(), "Console", "sConsoleCommandToStealA", "StartTrackPlayerDoors");
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand(toSteal.c_str());
		if (info)
		{
			static RE::SCRIPT_PARAMETER params[] =
			{
				{ "Path (String)", Type::kChar, false },
				{ "Section (String)", Type::kChar, true },
				{ "Key (String)", Type::kChar, true }
			};
			info->functionName = "PullValueFromIni";
			info->shortName = "PVFI";
			info->helpString = "Pulls value from ini file.";
			info->referenceFunction = false;
			info->SetParameters(params);
			info->executeFunction = PIMConsole::ExecutePVFI;
			info->conditionFunction = nullptr;

			PIMUtility::SKSEPluginLog(0, "PullValueFromIni and PVFI console commands registered");
			return true;
		}
		else
		{
			PIMUtility::SKSEPluginLog(0, "PullValueFromIni and PVFI console commands not registered");
			return false;
		}
	}

	bool RegisterPVTI()
	{
		using Type = RE::SCRIPT_PARAM_TYPE;
		std::string toSteal = PIMInternal::PullStringFromIniInternal(PIMUtility::PluginConfigPath(), "Console", "sConsoleCommandToStealB", "StopTrackPlayerDoors");
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand(toSteal.c_str());
		if (info)
		{
			static RE::SCRIPT_PARAMETER params[] =
			{
				{ "Path (String)", Type::kChar, false },
				{ "Section (String)", Type::kChar, true },
				{ "Key (String)", Type::kChar, true },
				{ "Value (String)", Type::kChar, true },
				{ "Force (Boolean)", Type::kChar, true }
			};
			info->functionName = "PushValueToIni";
			info->shortName = "PVTI";
			info->helpString = "Pushes value to ini file.";
			info->referenceFunction = false;
			info->SetParameters(params);
			info->executeFunction = PIMConsole::ExecutePVTI;
			info->conditionFunction = nullptr;

			PIMUtility::SKSEPluginLog(0, "PushValueToIni and PVTI console commands registered");
			return true;
		}
		else
		{
			PIMUtility::SKSEPluginLog(0, "PushValueToIni and PVTI console commands not registered");
			return false;
		}
	}

	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type)
		{
		case SKSE::MessagingInterface::kDataLoaded:
			if (RegisterPVFI() && RegisterPVTI())
			{
				PIMUtility::SKSEPluginLog(0, "Console commands registered");
			}
			else
			{
				PIMUtility::SKSEPluginLog(0, "Could not register console commands");
			}
			break;
		}
	}
}

#if ANNIVERSARY_EDITION

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData data{};

	data.PluginVersion(Version::MAJOR);
	data.PluginName(Version::NAME);
	data.AuthorName("Meridiano"sv);

	data.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	data.UsesAddressLibrary(true);

	return data;
}();

#else

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor())
	{
		PIMUtility::SKSEPluginLog(1, "Loaded in editor, marking as incompatible");
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39)
	{
		PIMUtility::SKSEPluginLog(1, "Unable to load this plugin, incompatible runtime version!\nExpected: 1.5.39-1.5.97 ('Old' Special Edition).\nDetected: ", ver.string());
		return false;
	}
	
	return true;
}

#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#if ANNIVERSARY_EDITION
	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	if (REL::Module::get().version() < SKSE::RUNTIME_1_6_317)
	{
		PIMUtility::SKSEPluginLog(1, "Unable to load this plugin, incompatible runtime version!\nExpected: 1.6.317-1.6.X ('New' Special Edition).\nDetected: ", REL::Module::get().version().string());
		return false;
	}
#endif
	PIMUtility::SKSEPluginLog(0, Version::PROJECT.data(), " plugin loaded, version ", Version::NAME.data());
	SKSE::Init(a_skse);
	
	bool registerPapyrusFunctionsResult = false;
	bool registerConsoleCommandsResult = false;

	if (PIMInternal::PullBoolFromIniInternal(PIMUtility::PluginConfigPath(), "Papyrus", "bRegisterPapyrusFunctions", false))
	{
		auto papyrusInterface = SKSE::GetPapyrusInterface();
		registerPapyrusFunctionsResult = papyrusInterface && papyrusInterface->Register(PapyrusManager::Register);
		if (registerPapyrusFunctionsResult)
		{
			PIMUtility::SKSEPluginLog(0, "Papyrus functions registered");
		}
		else
		{
			PIMUtility::SKSEPluginLog(0, "Could not register Papyrus functions");
		}
	}
	else
	{
		PIMUtility::SKSEPluginLog(0, "Papyrus functions disabled");
	}

	if (PIMInternal::PullBoolFromIniInternal(PIMUtility::PluginConfigPath(), "Console", "bRegisterConsoleCommands", false))
	{
		auto messagingInterface = SKSE::GetMessagingInterface();
		registerConsoleCommandsResult = messagingInterface && messagingInterface->RegisterListener("SKSE", ConsoleManager::MessageHandler);
		if (registerConsoleCommandsResult)
		{
			PIMUtility::SKSEPluginLog(0, "Trying to register console commands...");
		}
		else
		{
			PIMUtility::SKSEPluginLog(0, "Could not register console commands");
		}
	}
	else
	{
		PIMUtility::SKSEPluginLog(0, "Console commands disabled");
	}

	if (registerPapyrusFunctionsResult || registerConsoleCommandsResult)
	{
		PIMUtility::SKSEPluginLog(0, "Initial pretty-print mode is ", PIMUtility::BoolToString(PIMInternal::UsePrettyPrint()));
		return true;
	}

	PIMUtility::SKSEPluginLog(1, "Both papyrus functions and console commands are off, unload plugin");
	return false;
}
