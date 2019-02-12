
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>
#include <boost/ptr_container/ptr_deque.hpp>

#include <vector>
#include <thread>

#include "options.h"
#include "datum.h"
#include "ingress.h"
#include "egressWebSocket.h"

namespace logging = boost::log; 

typedef std::shared_ptr<Datum> DatumPtr;
typedef std::vector<DatumPtr> DatumList;


void initialize_logging(int logging_level)
{
	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging_level
	);

	BOOST_LOG_TRIVIAL(trace) << boost::format("Logging level set to:  %1%") % logging_level;
}


void initialize(int aArgc, const char *aArgv[]) {

	BOOST_LOG_TRIVIAL(trace) << "Application initialization started.";

	Options opt(aArgc, aArgv);
	initialize_logging(opt.getLogLevel());
	
}


int main(int aArgc, const char *aArgv[])
{

	initialize(aArgc, aArgv);
	

	// Create the vector all of the shared pointers(datum) will be stored in.
	DatumList datumVect;

	// Create the datum that will be collected.
	DatumPtr datum_ptr = std::make_shared<Measurement>("Measurement1");  //cout << datum_ptr->name();
	datumVect.push_back(datum_ptr); //cout << datumVect[0]->name();

	// Create the working threads and pass the datum vector
	std::thread inThread = std::thread(collectionThread, std::ref(datumVect));
	std::thread out1Thread = std::thread(egressWSThread, std::ref(datumVect));

	inThread.join();
	out1Thread.join();

	return 0;
}
