#pragma once
#include <boost/any.hpp>

#include <mutex>

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
		try {
			std::lock_guard<std::mutex> lck(mtx);
		}
		catch (std::logic_error&) {
			std::cout << "[exception caught]\n";
		}

		int c = 0;
		c += 1;
	}
};

