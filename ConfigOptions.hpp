#pragma once

// #include "ConfigOptionsExceptions.h"
#include <map>
#include <vector>
#include <atlstr.h>
class ConfigOptions{

private:
	std::map< CString, CString > values;
	std::map< CString, ConfigOptions > groups;

	static ConfigOptions build( std::ifstream* );
	static const char SEPARATOR = ':';

public:
	static CString Trim( CString );
	static CString TrimKey( CString );
	static std::vector<CString> Split( CString str, CString token );

public:
	ConfigOptions(void);
	~ConfigOptions(void);

	static ConfigOptions buildFromFile( CString );

	void addOpt( CString, CString );
	void addGroup( CString, ConfigOptions );

	ConfigOptions from( CString );
	CString get( CString );
};

