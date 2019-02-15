// Requires Boost Packages
//
// Visual Studio (Windows Build) - Nuget Package Manager Console
// PM> Install-Package boost
// PM> Install-Package boost_date_time-vc141
// PM> Install-Package boost_filesystem-vc141
// PM> Install-Package boost_log-vc141
// PM> Install-Package boost_system-vc141
// PM> Install-Package boost_thread-vc141
// PM> Install-Package boost_chrono-vc141
// PM> Install-Package boost_log_setup-vc141
// PM> Install-Package boost_atomic-vc141
// PM> Install-Package boost_program_options-vc141

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>
#include <boost/ptr_container/ptr_deque.hpp>

// Using C11 packages for vector and thread (adopted from boost)
#include <vector>
#include <thread>

// Application headers are in ../include
// be sure to set location as complier option
#include "options.h"
#include "datum.h"
#include "collection.h"
#include "webSocket.h"


// Naming the shared_ptr and vector to improve readability
typedef std::shared_ptr<Datum> DatumPtr;
typedef std::vector<DatumPtr> DatumList;


/** 
Set the logging level.

Sets the logging level for the application.

@param int logging_level: This is the enumartion from boost::log::trivial::severity_level
   trace(0),debug(1),info(2),warning(3),error(4),fatal(5)
@return void:
*/
void initialize_logging(int logging_level)
{
	BOOST_LOG_TRIVIAL(trace) << boost::format("Setting log level set to:  %1%") % logging_level;

	boost::log::core::get()->set_filter
	(
		boost::log::trivial::severity >= logging_level
	);

}

/**
Application initialization call.

Iterates through all application setup such as logging and options.

@param int aArgc: count of the number of elements in aArgv[]
@param const char *aArgv[]: array of argument strings passed by the command line
@return void:
*/
void initialize(int aArgc, const char *aArgv[]) {

	BOOST_LOG_TRIVIAL(trace) << "Application initialization started.";

	Options opt(aArgc, aArgv);
	initialize_logging(opt.getLogLevel());

}


/**
Application entry point.

Start of the application.  Initialize, setup datum list, start ingress and egress threads.

@param int aArgc: count of the number of elements in aArgv[]
@param const char *aArgv[]: array of argument strings passed by the command line
@return int: exit code of the application
*/
int main(int aArgc, const char *aArgv[])
{
	// Initialize applicaiton settings
	initialize(aArgc, aArgv);


	// Create the vector all of the shared pointers(datum) will be stored in.
	DatumList datumVect;

	// Create the datum that will be collected.
	DatumPtr datum_ptr = std::make_shared<Measurement>("Measurement1");
	datumVect.push_back(datum_ptr);

	// Create the working threads and pass the datum vector
	std::thread inThread = std::thread(collectionThread, std::ref(datumVect));
	std::thread out1Thread = std::thread(webSocketThread, std::ref(datumVect));

	// Wait for all threads to exit
	inThread.join();
	out1Thread.join();

	return 0;
}