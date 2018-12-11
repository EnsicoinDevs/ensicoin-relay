#include "constants.hpp"
#include "networkable.hpp"
#include "networkbuffer.hpp"
#include "relay.hpp"

#include <asio.hpp>
#include <functional>
#include <memory>
#include <spdlog/spdlog.h>

namespace network{

	Connection::pointer Connection::create(
			asio::io_context& io_context,
			Relay* relay,
			std::shared_ptr<spdlog::logger> lgr){
		return Connection::pointer(new Connection(io_context, relay, lgr));
	}

	asio::ip::tcp::socket& Connection::getSocket(){
		return socket;
	}

	std::string Connection::remote() const{
		return socket.remote_endpoint().address().to_string();
	}

	void Connection::start(){
		idle();
	}

	void Connection::sendMessage(const std::string& binaryString){
		asio::async_write(socket, asio::buffer(binaryString), 
						  std::bind(&Connection::handleWrite, 
									shared_from_this()));
	}

	Connection::Connection(asio::io_context& io_context,
			    		   Relay* relayPtr,
						   std::shared_ptr<spdlog::logger> logger_) :
		socket(io_context),
		relay(relayPtr), 
		netBuffer(logger_),
		logger(logger_) {}

	std::string Connection::readAll(){
		std::istream is(&buffer);
		std::string stringData(
				std::istreambuf_iterator<char>(is), {});
		return stringData;
	}

	void Connection::handleHeader(){
		logger->debug("[{}] reading header", remote());
		auto stringData = readAll();
		netBuffer.appendRawData(stringData);
		auto header = networkable::MessageHeader(&netBuffer);
		asio::async_read(socket, buffer, 
				asio::transfer_exactly(header.payloadLength), 
				std::bind(&Connection::handleMessage, 
					shared_from_this(), header) );
	}

	void Connection::handleMessage(const networkable::MessageHeader& header){
		logger->info("{} from {}", header.type, remote());
		logger->debug("[{}] reading {} bytes in payload", remote(), header.payloadLength);
		auto stringData = readAll();
		std::string message = header.byteRepr() + stringData;
		relay->broadcast(message);
		idle();
	}

	void Connection::handleWrite(){
		idle();
	}

	void Connection::idle(){
		asio::async_read(socket, buffer, 
				asio::transfer_exactly(networkable::\
					MessageHeader::size), 
				std::bind(&Connection::handleHeader, 
					shared_from_this()) );

		logger->trace("[{}] waiting mesage", remote());
	}
} //namespace network
