/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                        //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Journey.h"
#ifdef JOURNEY_USE_ASIO
#include "SocketAsio.h"

namespace Net
{
	SocketAsio::SocketAsio() : resolver(ioservice), socket(ioservice) {}

	SocketAsio::~SocketAsio() 
	{
		if (socket.is_open())
		{
			error_code error;
			socket.close(error);
		}
	}

	bool SocketAsio::open(const char* adress, const char* port)
	{
		tcp::resolver::query query(adress, port);
		tcp::resolver::iterator endpointiter = resolver.resolve(query);
		error_code error;
		asio::connect(socket, endpointiter, error);
		if (!error)
		{
			size_t result = socket.read_some(asio::buffer(buffer), error);
			return !error && (result == HANDSHAKE_LEN);
		}
		return !error;
	}

	bool SocketAsio::close()
	{
		error_code error;
		socket.shutdown(tcp::socket::shutdown_both, error);
		socket.close(error);
		return !error;
	}

	size_t SocketAsio::receive(bool* recvok)
	{
		if (socket.available() > 0)
		{
			error_code error;
			size_t result = socket.read_some(asio::buffer(buffer), error);
			*recvok = !error;
			return result;
		}
		return 0;
	}

	const int8_t* SocketAsio::getbuffer() const
	{
		return buffer;
	}

	bool SocketAsio::dispatch(const int8_t* bytes, size_t length)
	{
		error_code error;
		size_t result = asio::write(socket, asio::buffer(bytes, length), error);
		return !error && (result == length);
	}
}
#endif