#ifndef RELAY_HPP
#define RELAY_HPP

#include "connection.hpp"

#include <asio.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

/// \brief Relay handling connections
class Relay{
	private:
		/// \brief Logger
		std::shared_ptr<spdlog::logger> logger;
		
		/// \brief Accept TCP connections from other nodes
		asio::ip::tcp::acceptor acceptor;
		std::vector<network::Connection::pointer> connections; ///< Vector of all current connections
		

		/// \brief Handles a connection
		/// \param newConnection a shared pointer to the connection
		void handleAccept(network::Connection::pointer newConnection);
	public:
		/// \brief Construct a Node
		/// \param io_context an asio::io_context
		/// \details Executes the startup 
		/// routine and creates the mempool
		explicit Relay(asio::io_context& io_context);

		void broadcast(const std::string& message);
		
		/// \brief Main loop of Node
		///
		void run();
};

#endif /* RELAY_HPP */
