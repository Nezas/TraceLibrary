#include "logger.h"

using namespace std;

int main() {
	Logger::EnableFileOutput();

	LOG_INFO("Test", "Hello World");
	LOG_WARN("Test", 8);

	return 0;
}