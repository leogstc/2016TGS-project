#pragma once

#include <Windows.h> //GetPrivateProfile...
#include <string>

class IniReader {
	std::string fileName_;
public:
	IniReader(char* fileName) : fileName_(fileName) {}

	int readInteger(char* section, char* key, int defaultValue = 0)
	{
		return GetPrivateProfileInt(section, key, defaultValue, fileName_.c_str());
	}

	float readFloat(char* section, char* key, float defaultValue = 0)
	{
		std::string defaultVal = std::to_string(defaultValue);
		char result[255];
		GetPrivateProfileString(section, key, defaultVal.c_str(), result, 255, fileName_.c_str());
		return atof(result);
	}

	std::string readString(char* section, char* key, std::string defaultValue = "")
	{
		char result[255];
		//memset(result, 0x00, 255);
		GetPrivateProfileString(section, key, defaultValue.c_str(), result, 255, fileName_.c_str());
		std::string s(result);
		return s;
	}

	bool readBoolean(char* section, char* key, bool defaultValue = false)
	{
		std::string defaultVal = (defaultValue ? "true" : "false");
		char result[255];
		GetPrivateProfileString(section, key, defaultVal.c_str(), result, 255, fileName_.c_str());
		for (int i = 0; i < strlen(result); ++i)
			result[i] = tolower(result[i]);
		return (strcmp(result, "true") == 0 ? true : false);
	}

};