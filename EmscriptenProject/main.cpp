#include <emscripten.h>

extern int main(int argc, const char **argv); 

extern "C" void RunTests()
{
	const char *args[] =
	{
		"Enscripten",
		"/"
	};
	main(2, args);
}