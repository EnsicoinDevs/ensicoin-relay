#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/// \brief Constants
namespace constants{
	/// \brief Maximum size of a Block
	constexpr int MAX_BLOCK_SIZE = 2*1024;
	/// \brief Maximum number of characters in a Message
	constexpr int MESSAGE_LIMIT = 2*MAX_BLOCK_SIZE;
	/// \brief Magic number of the network
	constexpr int MAGIC = 422021;
	/// \brief Port used on the network
	constexpr int PORT = 4224;
	/// \brief Port used to liste
	constexpr int LISTEN_PORT = 4224;
} // namespace constants

#endif /* CONSTANTS_HPP */
