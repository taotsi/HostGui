#include <iostream>
#include "hostgui.h"

int main(int, char**){
	HostGui host;
	Transceiver msg;
	while(host.is_on());
	std::cout << "done~\n";
	return 0;
}
