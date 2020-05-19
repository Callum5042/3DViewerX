
#include "Application.h"
#include <memory>

int main(int argc, char** argv)
{
	std::unique_ptr<Engine> engine(new Application());
	return engine->Execute(argc, argv);
}