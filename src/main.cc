#include <iostream>
#include "hostgui.h"

int main(int, char**){
	HostGui host;
	while(host.is_on());
	std::cout << "done~\n";
	return 0;
}
