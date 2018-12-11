#include "relay.hpp"
#include "networkable.hpp"
#include "util.hpp"

#include <iostream>
#include <asio.hpp>

int main(){
	networkable::Uint16 test(0xf104);
	util::printBinaryString(test.byteRepr());

	asio::io_context io_context;
	Relay relay(io_context);
	io_context.run();

	return 0;
}
