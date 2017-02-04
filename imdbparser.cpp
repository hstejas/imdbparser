#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <thread>
#include <unordered_map>



class IMDBParser
{
public:
	bool parseRatings(std::string ratingFilePath)
	{
		std::regex ratingregex("\\s*([\\d\\.\\*]+)\\s+(\\d+)\\s+([\\d\\.]+)\\s+(.*)");
		std::regex yearregex("\\((\\d{4})");
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
					std::pair<std::unordered_map<std::string, MovieInfo>::iterator, bool> result =
						mMovies.emplace( std::make_pair(match.str(4), MovieInfo(std::stof(match.str(3)), std::stoi(match.str(2)), match.str(1))) );
					std::smatch yearmatch;
					if (std::regex_match(result.first->first, yearmatch, yearregex) && yearmatch.size() > 1)
					{
						result.first->second.mYear = std::stoi(yearmatch.str(1));
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

	bool parseLanguages(std::string languageFilePath)
	{
		std::ifstream fin(languageFilePath);
		std::regex split("^(.*?)\\s{2,}(.*)$");
		if (fin)
		{
			std::string line;
			std::unordered_map<std::string, MovieInfo>::iterator movIt = mMovies.end();
			while (std::getline(fin, line))
			{
				std::smatch match;
				if (std::regex_match(line, match, split) && match.size() == 3)
				{
					movIt = mMovies.find(match.str(1));
					if (movIt != mMovies.end())
					{
						movIt->second.mLanguages = match.str(2);
					}
				}
			}
		}
		else
		{
			std::cerr << "Error : could not open " << languageFilePath << std::endl;
			return false;
		}
		return true;
	}

	bool parseCertificates(std::string certificateFilePath)
	{
		std::ifstream fin(certificateFilePath);
		std::regex split("^(.*?)\\s{2,}(.*)$");
		if (fin)
		{
			std::string line;
			std::unordered_map<std::string, MovieInfo>::iterator movIt = mMovies.end();
			while (std::getline(fin, line))
			{
				std::smatch match;
				if (std::regex_match(line, match, split) && match.size() == 3)
				{
					movIt = mMovies.find(match.str(1));
					if (movIt != mMovies.end())
					{
						movIt->second.mCertificates = match.str(2);
					}
				}
			}
		}
		else
		{
			std::cerr << "Error : could not open " << certificateFilePath << std::endl;
			return false;
		}
		return true;
	}
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
		std::string mLanguages;
		std::string mCertificates;;
	};

	std::unordered_map<std::string, MovieInfo> mMovies;
};

int main(int argc, char** argv)
{	
	if (argc > 1)
	{
		IMDBParser parser;
		clock_t start = clock();
		parser.parseRatings(argv[1]);
		parser.parseLanguages(argv[2]);
		parser.parseCertificates(argv[3]);
		clock_t end = clock();
		std::cout << "Time: " << (end - start) / 1000.0 << " sec"<< std::endl;
	}
	else
	{
		return 1;
	}
	//return 0;
}
