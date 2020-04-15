
#include "Application.h"

int main(int argc, char** argv)
{
	Engine* engine = new Application();
	return engine->Execute(argc, argv);
}