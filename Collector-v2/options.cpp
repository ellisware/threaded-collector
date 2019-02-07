#include <iostream>
#include <vector>
#include <fstream>

#include <boost/program_options.hpp>

#include "options.h"
#include "constants.h"

namespace logging = boost::log;
namespace po = boost::program_options;
using namespace std;



Options::Options(int aArgc, const char *aArgv[])
{
	initialize_options(aArgc, aArgv);
}


Options::~Options()
{

}

int Options::getLogLevel()
{
	return mLogLevel;
}


int Options::initialize_options(int aArgc, const char *aArgv[]) {

	try
	{
		// Declare a group of options that will be allowed
		// only on the command line
		po::options_description generic("Generic options");
		generic.add_options()
			("version,v", "print version string")
			("help", "produce help message")
			;

		// Declare a group of options that will be allowed
		// both on the command line and in a config file
		po::options_description config("Configuration");
		config.add_options()
			("log-level,L", po::value<int>(&mLogLevel)->default_value(logging::trivial::warning), "logging level")
			;

		// Hidden options, will be allowed both on command line
		// and in config file, but will not be shown to the user.
		po::options_description hidden("Hidden options");
		hidden.add_options()
			//("input-file", po::value<string_vector>(), "input file")
			;

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		//po::options_description visible("Allowed options");
		po::options_description visible;
		visible.add(generic).add(config);

		po::positional_options_description p;
		//p.add("input-file", -1);

		po::variables_map vm;
		store(po::command_line_parser(aArgc, aArgv).options(cmdline_options).positional(p).run(), vm);

		ifstream ifs(configuration_filename);
		store(parse_config_file(ifs, config_file_options), vm);
		notify(vm);

		if (vm.count("help"))
		{
			cout << "Usage: " << aArgv[0] << " [options] \n";
			cout << visible << "\n";
			return 1;
		}

		if (vm.count("version"))
		{
			cout << version;
			return 1;
		}

	}
	catch (exception &e)
	{
		cout << "Exception: " << e.what() << endl;
		return 1;
	}

	return 0;
}