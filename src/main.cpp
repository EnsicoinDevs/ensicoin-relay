#include "relay.hpp"
#include "networkable.hpp"
#include "util.hpp"

#include <iostream>
#include <asio.hpp>

int main(){
	asio::io_context io_context;
	Relay relay(io_context);
	io_context.run();

	return 0;
}
