#include "networkable.hpp"
#include "constants.hpp"
#include "networkbuffer.hpp"
#include "util.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <spdlog/spdlog.h>

namespace networkable{

	Networkable::~Networkable() {}

	std::string Networkable::encode(const std::string& 
			hexRepr) const{
		std::string decodedBytes;
	
		char currentChar = char(0x00);
		bool low = true;
		for(auto it = hexRepr.rbegin(); it < hexRepr.rend(); ++it){
			char word = char(0);
			if( '0' <= *it && *it <= '9'){
				word = *it - '0';
			}else if( 'a' <= *it && *it <= 'f'){
				word = *it - 'a' + 10;
			}else{
				spdlog::get("console")->critical("invalid char in hex string : {}", *it);
			}
			if(!low){
				word <<= 4;
				low = true;
				currentChar |= word;
				decodedBytes += currentChar;
				currentChar = char(0x00);
			}else{
				low = false;
				currentChar |= word;
			}
		}
		std::reverse(decodedBytes.begin(), decodedBytes.end());
		return decodedBytes;
	}

	Uint16::Uint16(uint16_t val) : value(val) {}
	Uint16::Uint16() {}

	std::string Uint16::byteRepr() const{
		std::ostringstream sstream;
		sstream << std::setfill('0')
			<< std::setw(4) 
			<< std::hex 
			<< value;
		return encode(sstream.str());
	}
	
	uint16_t Uint16::getValue() const{
		return value;
	}

	Uint16::Uint16(NetworkBuffer* networkBuffer) :
		value(networkBuffer->readUint16()) {}

	Uint32::Uint32(uint32_t val) : value(val) {}
	Uint32::Uint32() {}

	std::string Uint32::byteRepr() const{
		std::ostringstream sstream;
		sstream << std::setfill('0')
			<< std::setw(8)
			<< std::hex
			<< value;
		return encode(sstream.str());
	}
	uint32_t Uint32::getValue() const{
		return value;
	}
	Uint32::Uint32(NetworkBuffer* networkBuffer) :
		value(networkBuffer->readUint32()) {}

	Uint64::Uint64(uint64_t val) : value(val) {}
	std::string Uint64::byteRepr() const{
		std::ostringstream sstream;
		sstream << std::setfill('0')
			<< std::setw(16)
			<< std::hex
			<< value;
		return encode(sstream.str());
	}
	uint64_t Uint64::getValue() const{
		return value;
	}
	Uint64::Uint64(NetworkBuffer* networkBuffer) :
		value(networkBuffer->readUint64()) {}
	Uint64::Uint64() {}

	Var_uint::Var_uint(uint64_t val) : value(val) {}
	Var_uint::Var_uint() {}

	uint64_t Var_uint::getValue() const{
		return value;
	}

	std::string Var_uint::byteRepr() const{
		std::ostringstream sstream;
		sstream << std::hex << value;
		auto rawStr = sstream.str();
		if(value <= 252){
			rawStr = std::string(2-rawStr.length(), '0')
				+ rawStr;
		}
		else if(value <= 0xffff){
			rawStr = "fd" + 
				std::string(4-rawStr.length(),'0') + 
				rawStr;
			// 3 : 0xFD.value
		}
		else if(value <= 0xffffffff){
			rawStr = "fe" +
				std::string(8-rawStr.length(),'0') +
				rawStr;
			// 5 : 0xFE.value
		}
		else{
			rawStr = "ff" +
				std::string(16-rawStr.length(),'0') +
				rawStr;
			// 9 0xFF.value
		}
		return encode(rawStr);
	}

	Var_uint::Var_uint(NetworkBuffer* networkBuffer) :
		value(networkBuffer->readVar_uint()) {}

	std::string Str::getValue() const{
		return value;
	}
	std::string Str::byteRepr() const{
		return value;
	}
	Str::Str(const std::string& val) : value(val) {}
	Str::Str() {}

	Str::Str(NetworkBuffer* buffer,
			 size_t stringSize) : 
		value(buffer->readStr(stringSize)){}

	Var_str::Var_str(const std::string& val) : value(val) {}

	std::string Var_str::getValue() const{
		return value.getValue();
	}
	std::string Var_str::byteRepr() const{
		auto sizeBytes =
			Var_uint(value.getValue().size()).byteRepr();
		return sizeBytes + value.byteRepr();
	}
	Var_str::Var_str(NetworkBuffer* networkBuffer){
		auto size = Var_uint(networkBuffer).getValue();
		value = Str(networkBuffer, size);
	}

	std::string Address::getAddress() const{
		return address;
	}
	uint16_t Address::getPort() const{
		return port;
	}
	uint64_t Address::getTimestamp() const{
		return timestamp;
	}
	Address Address::getValue() const{
		return *this;
	}
	Address::Address(uint64_t lastActive,
					 std::string ipAddr,
					 uint16_t _port) :
		timestamp(lastActive),
		address(ipAddr),
		port(_port) {}
	Address::Address(NetworkBuffer* buffer) :
		timestamp(Uint64(buffer).getValue()),
		address(IP(buffer).getValue()),
		port(Uint16(buffer).getValue()) {}
	std::string Address::byteRepr() const{
		return Uint64(timestamp).byteRepr() + IP(address).byteRepr() + Uint16(port).byteRepr();
	}
	Address::Address() {}

	MessageHeader::MessageHeader(uint32_t _magic, 
			std::string _type,
			uint64_t pldSize) : 
		magic(_magic),
		type(_type),
		payloadLength(pldSize) {}
	std::string MessageHeader::byteRepr() const{
		std::string out = Uint32(magic).byteRepr();
		out += FixedStr<12>(type).byteRepr();
		out += Uint64(payloadLength).byteRepr();
		return out;
	}
	MessageHeader::MessageHeader(NetworkBuffer* buffer) :
		magic(Uint32(buffer).getValue()),
		type(FixedStr<12>(buffer).getValue()),
		payloadLength(Uint64(buffer).getValue()) {}
	MessageHeader::MessageHeader() {}

} // namespace networkable
