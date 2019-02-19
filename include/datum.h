#pragma once

// Requires Boost Packages
#include <boost/any.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>

// Using C11 packages for mutex (adopted from boost)
#include <mutex>

/**
The base datum all are inherited from.

The one datum to rule them all.  All datum inheret this basic struct, and then modify the collect() at a minimum.

*/
struct Datum
{
private:
	std::string mName = "";
	std::string mValue = "0";
	bool mChanged = false;
	int mMissed = false;

public:
	std::mutex mtx;

	Datum(std::string name) : mName(name) {};
	virtual ~Datum() = default;

	std::string name() { return mName; };
	std::string value() { mChanged = false; return mValue; };
	void setValue(std::string v) { setChanged();  mValue = v; return; };
	bool changed() { return mChanged; };
	void setChanged() { mChanged = true; };
	int collectionMissed() { return mMissed; };

	virtual void pre_collect() = 0;
	virtual void collect() = 0;
	virtual void post_collect() = 0;

};

/**
Example Datum.

Measurement class inherets the datum struct, then introduces its custom collect()

*/
struct Measurement : Datum
{
	using Datum::Datum;

private:
	int mMeasure = 0;

public:
	Measurement(std::string name) : Datum(name) {};

	void pre_collect() {

		// Simple Demonstration
		// Increment the internal member variable representing the value
		mMeasure += 1;
		setChanged();
	}


	void collect() {

		// Set the collected value as a string
		setValue(std::to_string(mMeasure));
	}

	void post_collect() {

	}
};

