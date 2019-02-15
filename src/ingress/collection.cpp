// Requires Boost Packages
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/foreach.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>

// Application headers are in ../include
// be sure to set location as complier option
#include "collection.h"
#include "constants.h"


/**
Wait for the specified number of seconds.

If the collection time needs to be slowed use this wait() function.

@param int seconds: The number of seconds to wait
@return void:
*/
void wait(int seconds)
{
	boost::this_thread::sleep_for(boost::chrono::seconds{ seconds });
}

/**
Entry point to the collection thread.

Iterate through the list of list of datum, and collect().

@param std::vector<std::shared_ptr<Datum>> datumList: The shared_pointers, shared by reference
@return void:
*/
void collectionThread(std::vector<std::shared_ptr<Datum>> datumList)
{
	BOOST_LOG_TRIVIAL(trace) << "Collection Thread Started";

	while (1) {
		BOOST_FOREACH(std::shared_ptr<Datum> dm, datumList)
		{
			dm->pre_collect();

			try {
				std::lock_guard<std::mutex> lck(dm->mtx);
				BOOST_LOG_TRIVIAL(trace) << boost::format("Attempting to collect:  %1%") % dm->name();
				dm->collect();
			}
			catch (std::logic_error&) {
				BOOST_LOG_TRIVIAL(trace) << boost::format("Failed to lock a datum.");
			}

			dm->post_collect();

		}
		wait(COLLECTION_DELAY);
	}
}
