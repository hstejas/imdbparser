#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <thread>
#include <unordered_map>


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
		MovieInfo(float rating, int count, const std::string& distribution) :
			mRating(rating), mCount(count), mDistribution(distribution) {}
		MovieInfo(const MovieInfo& src) :
			mRating(src.mRating), mCount(src.mCount), mDistribution(src.mDistribution) {}
		float mRating;
		int mCount;
		int mYear;
		std::string mDistribution;
		std::vector<std::string> mLanguages;
		std::vector<std::string> mCertificates;
		std::string mType;
		std::vector<std::string> mGenere;
		std::vector<std::string> mRuntime;
	};

	typedef std::unordered_map<std::string, MovieInfo> MovieMap;
	std::unordered_map<std::string, MovieInfo> mMovies;

};