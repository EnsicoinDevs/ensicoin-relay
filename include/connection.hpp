#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "networkable.hpp"
#include "networkbuffer.hpp"

#include <asio.hpp>
#include <memory>
#include <spdlog/spdlog.h>

class Relay;

/// \brief Objects to handle communiction between Node
namespace network{

	/// \brief A Connection to a peer
	class Connection : 
		public std::enable_shared_from_this<Connection>{
		public:
			/// \brief Shared pointer to a Connection
			using pointer = std::shared_ptr<Connection>;

			/// \brief Creates a shared_ptr of Connection
			/// \param io_context io_context for async operation
			/// \param relay pointer to a Relay to broadcast
			static pointer create(asio::io_context& io_context,
								  Relay* relay,
								  std::shared_ptr<spdlog::logger> lg);
			/// \brief IP Adress of remote connection
			std::string remote() const;
			/// \brief Reference to the socket
			asio::ip::tcp::socket& getSocket();
			/// \brief Initial state of Connection
			void start();
			/// \brief Send a Message
			/// \param rawBytes to be sent by this Connection
			void sendMessage(const std::string& rawBytes);
			/// \brief Idle loop of the Connection
			void idle();

		private:
			std::string readAll();

			/// \brief Construct a Connection
			Connection(asio::io_context& io_context,
					   Relay* relay,
					   std::shared_ptr<spdlog::logger> logger);

			/// \brief Called when recevieving a message
			void handleHeader();

			/// \brief Called when reading a Message
			void handleMessage(const networkable::MessageHeader&
					header);

			/// \brief Called when writing a Message
			void handleWrite();
			/// \brief Socket used for network access
			asio::ip::tcp::socket socket;

			/// \brief Pointer to the node to handle Message actions
			Relay* relay;
			/// \brief NetworkBuffer for the connection
			NetworkBuffer netBuffer;

			/// \brief Buffered content read from the node
			asio::streambuf buffer;

			std::shared_ptr<spdlog::logger> logger;
	};

} // namespace network

#endif /* CONNECTION_HPP */
