#include "timer.h"

int userMain(int argc, char* argv[]);

class userThread : public Thread {
public:
	userThread(int argc, char** argv) : Thread() {
		this->argc = argc;
		this->argv = argv;
		ret = 0;
	}

	~userThread() {
		waitToComplete();
	}

	Thread* clone() const {
		return new userThread(argc, argv);
	}

	int getRet() {
		return ret;
	}
protected:
	void run() {
		ret = userMain(argc, argv);
	}

private:
	int argc;
	char** argv;
	int ret;
};

int main(int argc, char* argv[]) {
	Timer::initialize();
	userThread* user = new userThread(argc, argv);
	user->start();
	user->waitToComplete();
	int ret = user->getRet();
	delete user;
	Timer::restore();
	return ret;
}

