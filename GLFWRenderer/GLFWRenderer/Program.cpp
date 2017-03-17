#include "app.h"

int main()
{
	try
	{
		App app;
		app.Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
	std::cin.get();
}