#pragma once
#include <boost/any.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>

#include <mutex>


namespace logging = boost::log;

struct Datum
{
private:
	std::string mName = "";
	boost::any mValue = 0;
	bool mChanged = false;
	int mMissed = false;

public:
	std::mutex mtx;

	Datum(std::string name) : mName(name) {};
	virtual ~Datum() = default;

	std::string name() { return mName; };
	boost::any value() { return mValue; };
	bool changed() { return mChanged; };
	int collectionMissed() { return mMissed; };


	virtual void collect() = 0;
};


struct Measurement : Datum
{
	using Datum::Datum;
	Measurement(std::string name) : Datum(name) {};

	void collect() {
		

		int c = 0;
		c += 1;
	}
};

