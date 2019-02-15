#pragma once
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#ifndef _INC_GUARD
	#define _INCL_GUARD

	const char version[] = "Data Collector - version 0.0";
	const char configuration_filename[] = "config.ini";

	const int COLLECTION_DELAY = 10;
	const int WEBSOCKET_PORT = 7878;
	const int WEBSOCKET_THREADS = 1;

#endif

