#include <sstream>
#include "ImdbParser.h"
#ifdef USE_SQLITE3
#include <sqlite3.h> 
#endif

void split2(const std::string& in, std::pair<std::string, std::string>& out, const char delim = '\t')
{
	unsigned int posStart = in.find_first_of(delim, 0);
	unsigned int posEnd = posStart + 1;
	for (posEnd; in[posEnd] == delim && posEnd < in.length(); ++posEnd);
	out.first = in.substr(0, posStart);
	out.second = in.substr(posEnd, in.length() - posEnd);
}

bool ImdbParser::parse2(const std::string& inFilePath, void (*add)(MovieMap::iterator& itr, const std::string&))
{
	std::ifstream fin(inFilePath);
	if (fin)
	{
		std::string line;
		MovieMap::iterator movIt = mMovies.end();
		std::pair<std::string, std::string> sPair;
		while (std::getline(fin, line))
		{
			split2(line, sPair);
			movIt = mMovies.find(sPair.first);
			if (movIt != mMovies.end())
			{
				add(movIt, sPair.second);
			}
			//else
			//{
			//	std::cerr << "No Show found for Language entry: " << line << std::endl;
			//}			
		}
	}
	else
	{
		std::cerr << "Error : could not open " << inFilePath << std::endl;
		return false;
	}
	return true;
}


bool ImdbParser::parseRatings(const std::string& ratingFilePath)
{
	std::regex ratingregex("\\s*([\\d\\.\\*]+)\\s+(\\d+)\\s+([\\d\\.]+)\\s+(.*)");
	std::regex yearregex("\\((\\d{4})");
	std::regex isTVRegex("\".*\"\\s+\\(");
	//TODO: video/video game  regex
	std::ifstream fin(ratingFilePath);
	std::string line;
	if (fin)
	{
		// Skip these lines, intro top 250 bottom 10 n stuff
		while (std::getline(fin, line))
		{
			if ("MOVIE RATINGS REPORT" == line) break;
		}
		while (std::getline(fin, line))
		{
			std::smatch match;
			if (std::regex_match(line, match, ratingregex) && match.size() == 5)
			{
				std::pair<MovieMap::iterator, bool> result =
					mMovies.emplace(std::make_pair(match.str(4), MovieInfo(std::stof(match.str(3)), std::stoi(match.str(2)), match.str(1))));
				std::smatch yearmatch;
				if (std::regex_search(result.first->first, yearmatch, yearregex) && yearmatch.size() > 1)
				{
					result.first->second.mYear = std::stoi(yearmatch.str(1));
				}
				//else
				//{
				//	std::cerr << "Error Parsing year: " << line << std::endl;
				//}

				if (std::regex_match(result.first->first, isTVRegex))
				{
					result.first->second.mType = "TV";
				}
			}
			//else
			//{
			//	// Not really errors, but...
			//	std::cerr << "Error Line: " << line << std::endl;
			//}
		}
	}
	else
	{
		std::cerr << "Error : could not open " << ratingFilePath << std::endl;
		return false;
	}
	return true;
}

bool ImdbParser::parseLanguages(const std::string& languageFilePath)
{
	return parse2(languageFilePath, 
		[](MovieMap::iterator& itr, const std::string& inStr) {itr->second.mLanguages.push_back(inStr); });
}

bool ImdbParser::parseCertificates(const std::string& certificateFilePath)
{
	return parse2(certificateFilePath,
		[](MovieMap::iterator& itr, const std::string& inStr) {itr->second.mCertificates.push_back(inStr); });
}

bool ImdbParser::parseGeneres(const std::string & genereFilePath)
{
	return parse2(genereFilePath,
		[](MovieMap::iterator& itr, const std::string& inStr) {itr->second.mGenere.push_back(inStr); });
}

bool ImdbParser::parseRuntime(const std::string & runtimeFilePath)
{
	return parse2(runtimeFilePath,
		[](MovieMap::iterator& itr, const std::string& inStr) {itr->second.mRuntime.push_back(inStr); });
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& in)
{
	if (!in.empty())
	{
		out << in[0];
		for (size_t i = 1; i < in.size(); i++)
		{
			out << ", " << in[i];
		}
	}
	return out;
}

void ImdbParser::printToCsv(const std::string& outFilePath)
{
	std::ofstream fout(outFilePath);
	if (fout)
	{
		fout << "Name; Year; Rating; Type; Genere; Runtime; Languages; Certificates" << std::endl;
		for (MovieMap::const_iterator it = mMovies.begin(); it != mMovies.end(); ++it)
		{ 
			const MovieInfo& mi = it->second;
			fout << it->first << "; " << mi.mYear << "; " << mi.mRating << "; " << mi.mType << "; " << mi.mGenere << "; " << mi.mRuntime << "; " << mi.mLanguages << "; " << mi.mCertificates << std::endl;
		}
	}
}

template<typename T>
std::string to_string(const std::vector<T>& in)
{
	std::stringstream ss;
	ss << in;
	return ss.str();
}

void ImdbParser::printToSQLite(const std::string & outFilePath)
{
#ifdef USE_SQLITE3
#define CHECK_ERR(x,y) x = y; x != SQLITE_OK?std::cerr << "Error: " << #y <<  sqlite3_errmsg(db) << std::endl :  std::cerr << "";

	sqlite3 *db;
	//char *errMsg = 0;
	int retCode;
	retCode = sqlite3_open(outFilePath.c_str(), &db);

	if (retCode != SQLITE_OK)
	{
		std::cerr << "Can't open database: " <<  sqlite3_errmsg(db) << std::endl;
	}
	else 
	{
		sqlite3_exec(db, "DROP TABLE movies", NULL, NULL, NULL);
		std::string createTable = "CREATE TABLE movies (name TEXT PRIMARY KEY  NOT NULL  UNIQUE , year INT, rating FLOAT, vote_count type INT, type TEXT, generes TEXT, runtime TEXT, languages TEXT, certificates TEXT)";
		retCode = sqlite3_exec(db, createTable.c_str(), NULL, NULL, NULL);
		if (retCode != SQLITE_OK)
		{
			std::cerr << "exec Error: " << sqlite3_errmsg(db) << std::endl;
		}
		else
		{
			std::string insert = "INSERT INTO movies values(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)";
			sqlite3_stmt *stmt;
			retCode = sqlite3_prepare_v2(db, insert.c_str(), insert.length(), &stmt, NULL);
			if (retCode != SQLITE_OK)
			{
				std::cerr << "exec Error: " << sqlite3_errmsg(db) << std::endl;
			}
			else
			{
				sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
				for (MovieMap::const_iterator it = mMovies.begin(); it != mMovies.end(); ++it)
				{
					std::string genere	= to_string(it->second.mGenere);
					std::string runtime	= to_string(it->second.mRuntime);
					std::string languages = to_string(it->second.mLanguages);
					std::string certificates = to_string(it->second.mCertificates);

					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 1, it->first.c_str(), it->first.length(), SQLITE_TRANSIENT));
					CHECK_ERR(retCode, sqlite3_bind_int(stmt, 2, it->second.mYear));
					CHECK_ERR(retCode, sqlite3_bind_double(stmt, 3, it->second.mRating));
					CHECK_ERR(retCode, sqlite3_bind_int(stmt, 4, it->second.mCount));
					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 5, it->second.mType.c_str(), it->second.mType.length(), SQLITE_TRANSIENT));
					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 6, genere.c_str(), genere.length(), SQLITE_TRANSIENT));
					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 7, runtime.c_str(), runtime.length(), SQLITE_TRANSIENT));
					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 8, languages.c_str(), languages.length(), SQLITE_TRANSIENT));
					CHECK_ERR(retCode, sqlite3_bind_text(stmt, 9, certificates.c_str(), certificates.length(), SQLITE_TRANSIENT));
					sqlite3_step(stmt);
					sqlite3_reset(stmt);
				}
				sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
			}
			sqlite3_finalize(stmt);
		}
	}
	sqlite3_close(db);
#else
	std::cerr << "Error: Not built with sqlite3, use csv instead" << std::endl;
#endif
}
