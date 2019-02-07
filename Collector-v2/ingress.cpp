#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <iostream>

#include "ingress.h"


void wait(int seconds)
{
	boost::this_thread::sleep_for(boost::chrono::seconds{ seconds });
}

void collectionThread(std::vector<std::shared_ptr<Datum>> datumList)
{
	for (int i = 0; i < 5; ++i)
	{
		wait(1);
		std::cout << datumList[0]->name();
		std::cout << i << '\n';

		datumList[0]->collect();
	}
}
