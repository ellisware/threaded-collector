#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <iostream>

#include "ingress.h"

namespace logging = boost::log;

void wait(int seconds)
{
	boost::this_thread::sleep_for(boost::chrono::seconds{ seconds });
}

void collectionThread(std::vector<std::shared_ptr<Datum>> datumList)
{
	BOOST_FOREACH(std::shared_ptr<Datum> dm, datumList)
	{
		BOOST_LOG_TRIVIAL(trace) << boost::format("Attempting to collect:  %1%") % dm->name();
		try {
			std::lock_guard<std::mutex> lck(dm->mtx);
		}
		catch (std::logic_error&) {
			BOOST_LOG_TRIVIAL(trace) << boost::format("Failed to lock:  %1%") % dm->name();
		}

		std::cout << "Ingress Thread";
		std::cout << dm->name();
	}
}
