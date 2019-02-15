#pragma once

class Options {
public:
	Options(int aArgc, const char *aArgv[]);
	~Options();
	int getLogLevel();

private:
	int mLogLevel = 0;
	int  initialize_options(int aArgc, const char *aArgv[]);
};