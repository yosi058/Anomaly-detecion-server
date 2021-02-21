

#ifndef CLI_H_
#define CLI_H_

#include <cstring>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	vector<Command*> commands;
	commend_data * data;
	// you can add data members
public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();
};

#endif /* CLI_H_ */
