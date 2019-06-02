#include <iostream>
#include <memory>
#include "hostgui.h"
#include "transceiver.h"

int main(int, char**){
	// HostGui host;
	// std::shared_ptr<HostGui> host_ptr;
	Component cpn;
	std::shared_ptr<Component> cpn_ptr;
	Transceiver tc;
	tc.AddSubscriber("msgtest", cpn_ptr);
	// while(host.is_on());
	while(tc.is_on());

	std::cout << "~done~\n";
	return 0;
}
