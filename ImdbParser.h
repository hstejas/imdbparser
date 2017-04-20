#pragma once

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#ifdef USE_SQLITE3
#include <sqlite3.h> 
#define CHECK_ERR(x,y) x = y; x != SQLITE_OK?std::cerr << __LINE__ << " Error: " << #y <<  sqlite3_errmsg(db) << std::endl :  std::cerr << "";
#endif

class IndexedStr
{
public:
	IndexedStr();
	unsigned int getId(const std::string & in);
	void printToSqlite(sqlite3 *db, const std::string & tableName);
protected:
	unsigned int mIndex;
	std::map<std::string, unsigned int> mMap;
};

class ImdbParser
{
public:
	bool parseRatings(const std::string& ratingFilePath);
	bool parseLanguages(const std::string& languageFilePath);
	bool parseCertificates(const std::string& certificateFilePath);
	bool parseGeneres(const std::string& genereFilePath);
	bool parseRuntime(const std::string & runtimeFilePath);

	void printToCsv(const std::string& outFilePath);
	void printToSQLite(const std::string& outFilePath);

protected:

	struct MovieInfo
	{
		MovieInfo() {}
		MovieInfo(unsigned int index, float rating, int count, const std::string& distribution) :
			mIndex(index), mRating(rating), mCount(count), mDistribution(distribution) {}
		MovieInfo(const MovieInfo& src) :
			mIndex(src.mIndex), mRating(src.mRating), mCount(src.mCount), mDistribution(src.mDistribution) {}
		float mRating;
		int mCount;
		int mYear;
		std::string mDistribution;
		std::vector<unsigned int> mLanguages;
		std::vector<unsigned int> mCertificates;
		std::string mType;
		std::vector<unsigned int> mGenere;
		std::vector<unsigned int> mRuntime;
		unsigned int mIndex;
	};

	IndexedStr mGenere;
	IndexedStr mLanguages;
	IndexedStr mCertificates;
	IndexedStr mRuntime;

	typedef std::unordered_map<std::string, MovieInfo> MovieMap;
	MovieMap mMovies;

	bool parse2(const std::string & inFilePath, std::function<void (MovieMap::iterator& itr, const std::string&)>);
	void createJuntion(sqlite3 *db, std::string tableName, std::function<const std::vector<unsigned int>&(const MovieInfo& info)> getVector);
};