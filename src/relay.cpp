#include "relay.hpp"
#include "connection.hpp"
#include "constants.hpp"
#include "networkable.hpp"

#include <asio.hpp>
#include <functional>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

Relay::Relay(asio::io_context& io_context) : 
	logger(spdlog::stdout_color_mt("console")),
	acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(),
												 constants::LISTEN_PORT)){
	
	acceptor.listen();
	run();
	
	logger->set_level(spdlog::level::trace);
	logger->info("Relay started");
}

void Relay::run(){
	network::Connection::pointer newConnection = network::Connection::create(acceptor.get_executor().context(), this, logger);
	acceptor.async_accept(newConnection->getSocket(), std::bind( &Relay::handleAccept, this, newConnection ));
}

void Relay::handleAccept(network::Connection::pointer newConnection){
	newConnection->start();
	connections.push_back(newConnection);
	run();
}

void Relay::broadcast(const std::string& message){
	for(auto& conn : connections){
		conn->sendMessage(message);
	}
}
