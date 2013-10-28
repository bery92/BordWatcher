#include "ConfigOptions.hpp"
#include <string>
#include <fstream>
#include <atlstr.h>

/** 
 * Constructor
 */
ConfigOptions::ConfigOptions(void){}

/**
 * Destructor
 */
ConfigOptions::~ConfigOptions(void){}

/**
 * Trim string
 * 
 * @param  str
 * @return std::string
 */
CString ConfigOptions::Trim( CString str ){
	int i = 0, j = str.GetLength() - 1;
	while( i < str.GetLength() && ( 
		str[i] == '\n' || str[i] == '\r' || str[i] == ' ' || str[i] == '\t' ) ){
		i++;
	}
	if ( i == str.GetLength() - 1 )
		return CString("");
	while( j > 0 && ( 
		str[j] == '\n' || str[j] == '\r' || str[j] == ' ' || str[j] == '\t' ) ){
		j--;
	}
	if ( j == 0 )
		return CString("");

	return str.Mid( i, j - i + 1 );
}

/**
 * Trim string for keys, taking the word only for the first space
 * 
 * @param  str
 * @return std::string
 */
CString ConfigOptions::TrimKey( CString str ){
	CString base = ConfigOptions::Trim(str);
	int i = 0;
	while( i < base.GetLength() && (
		base[i] != '\n' && base[i] && '\r' && base[i] != ' ' && base[i] != '\t'
	)){
		i++;
	}
	return base.Mid( 0, i );
}

/**
 * Recursive function to build configuration options from input stream
 * 
 * @param *indata
 * @return ConfigOptions
 */
ConfigOptions ConfigOptions::build( std::ifstream *indata ){
	std::string data;
	ConfigOptions base = ConfigOptions();
	int keypos;
	while( std::getline( *indata, data ) ){
		keypos = data.find_first_of('{');
		if( keypos != std::string::npos ){
			// Create a new group
			base.addGroup( Trim( CString( data.substr( 0, keypos ).c_str() ) ), build( indata ) );
		} else {
			keypos = data.find_first_of( ':' );
			if( keypos != std::string::npos ){
				// Add options to current group
				CString ttt = Trim( CString( data.substr( keypos + 1 ).c_str() ) );
				//CString ttt_key = CString( trim_key( data.substr( keypos + 1 ) ).c_str() );
				//if ( ttt != ttt_key ) system( "Echo da && pause" );
				base.addOpt( TrimKey( CString(  data.substr( 0, keypos ).c_str() ) ), ttt );
			} else {
				keypos = data.find_first_of( '}' );
				if( keypos != std::string::npos )
					// Return group
					return base;
			}
		}
	}
	return base;
}

/**
 * Build configuration options from file
 *
 * @param file
 */
ConfigOptions ConfigOptions::buildFromFile( CString file ){
	std::ifstream indata( file, std::ios::binary );
	if ( !indata )
		//throw CannotOpenFileException( file );
		system("pause");
	ConfigOptions base = build( &indata );
	return base;
}

/**
 * Add option to configuration
 * 
 * @param key
 * @param value
 */
void ConfigOptions::addOpt( CString key, CString value ){
	values.insert( std::pair<CString, CString>(key, value));
}

/**
 * Add group of options to configuration
 * 
 * @param key
 * @param opt
 */
void ConfigOptions::addGroup( CString key, ConfigOptions opt ){
	groups.insert( std::pair<CString, ConfigOptions>(key, opt));
}

/**
 * Select group of options
 * 
 * @param group_key
 * @return ConfigOptions
 */
ConfigOptions ConfigOptions::from( CString group_key ){
	return groups[group_key];
}

/**
 * Add option to configuration
 * 
 * @param key
 * @return ConfigOptions
 */
CString ConfigOptions::get( CString opt_key ){
	return values[opt_key];
}

/**
 * Split String
 */
std::vector<CString> ConfigOptions::Split( CString str, CString token ){
	std::vector<CString> base;
	int nTokenPos = 0;
	CString strToken = str.Tokenize( token, nTokenPos );
	while( !strToken.IsEmpty() ){
		base.push_back( strToken );
		strToken = str.Tokenize( token, nTokenPos );
	}
	return base;
}