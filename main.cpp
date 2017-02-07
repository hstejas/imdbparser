#include "ImdbParser.h"


int main(int argc, char** argv)
{
	if (argc > 1)
	{
		ImdbParser parser;
		clock_t start = clock();
		if (argc > 2) parser.parseRatings(argv[2]);
		if (argc > 3) parser.parseLanguages(argv[3]);
		if (argc > 4) parser.parseCertificates(argv[4]);
		if (argc > 5) parser.parseGeneres(argv[5]);
		if (argc > 6) parser.parseRuntime(argv[6]);
		if (argc > 1) parser.printToSQLite(argv[1]);
		clock_t end = clock();
		std::cout << "Time: " << (end - start) / 1000.0 << " sec" << std::endl;
	}
	else
	{
		return 1;
	}
	return 0;
}
