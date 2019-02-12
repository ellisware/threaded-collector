#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>

#include <boost/foreach.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "egressWebSocket.h"

namespace logging = boost::log;



using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

//------------------------------------------------------------------------------

std::vector<std::shared_ptr<Datum>> mDatumList;

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class session : public std::enable_shared_from_this<session>
{
	websocket::stream<tcp::socket> ws_;
	boost::asio::strand<
		boost::asio::io_context::executor_type> strand_;
	boost::beast::multi_buffer buffer_;

public:
	// Take ownership of the socket
	explicit
		session(tcp::socket socket)
		: ws_(std::move(socket))
		, strand_(ws_.get_executor())
	{
	}

	// Start the asynchronous operation
	void
		run()
	{
		// Accept the websocket handshake
		ws_.async_accept(
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_accept,
					shared_from_this(),
					std::placeholders::_1)));
	}

	void
		on_accept(boost::system::error_code ec)
	{
		if (ec)
			return fail(ec, "accept");

		//***************************************
		BOOST_FOREACH(std::shared_ptr<Datum> dm, mDatumList)
		{
			BOOST_LOG_TRIVIAL(trace) << "Socket Connected:";
			try {
				std::lock_guard<std::mutex> lck(dm->mtx);
			}
			catch (std::logic_error&) {
				BOOST_LOG_TRIVIAL(trace) << boost::format("Failed to lock:  %1%") % dm->name();
			}

			//boost::beast::multi_buffer buffer_;
			std::string const contents = dm->name();
			size_t n = buffer_copy(buffer_.prepare(contents.size()), boost::asio::buffer(contents));
			buffer_.commit(n);

			ws_.async_write(
				buffer_.data(),
				boost::asio::bind_executor(
					strand_,
					std::bind(
						&session::on_write,
						shared_from_this(),
						std::placeholders::_1,
						std::placeholders::_2)));

		}
		//****************************************



		// Read a message
		//do_read();
	}

	void
		do_read()
	{
		// Read a message into our buffer
		ws_.async_read(
			buffer_,
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_read,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));
	}

	void
		on_read(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == websocket::error::closed)
			return;

		if (ec)
			fail(ec, "read");

		// Echo the message
		ws_.text(ws_.got_text());
		ws_.async_write(
			buffer_.data(),
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_write,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));
	}

	void
		on_write(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "write");

		// Clear the buffer
		buffer_.consume(buffer_.size());

		// Do another read
		do_read();
	}
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
	tcp::acceptor acceptor_;
	tcp::socket socket_;

public:
	listener(
		boost::asio::io_context& ioc,
		tcp::endpoint endpoint)
		: acceptor_(ioc)
		, socket_(ioc)
	{
		boost::system::error_code ec;

		// Open the acceptor
		acceptor_.open(endpoint.protocol(), ec);
		if (ec)
		{
			fail(ec, "open");
			return;
		}

		// Allow address reuse
		acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
		if (ec)
		{
			fail(ec, "set_option");
			return;
		}

		// Bind to the server address
		acceptor_.bind(endpoint, ec);
		if (ec)
		{
			fail(ec, "bind");
			return;
		}

		// Start listening for connections
		acceptor_.listen(
			boost::asio::socket_base::max_listen_connections, ec);
		if (ec)
		{
			fail(ec, "listen");
			return;
		}
	}

	// Start accepting incoming connections
	void
		run()
	{
		if (!acceptor_.is_open())
			return;
		do_accept();
	}

	void
		do_accept()
	{
		acceptor_.async_accept(
			socket_,
			std::bind(
				&listener::on_accept,
				shared_from_this(),
				std::placeholders::_1));
	}

	void
		on_accept(boost::system::error_code ec)
	{
		if (ec)
		{
			fail(ec, "accept");
		}
		else
		{
			// Create the session and run it
			std::make_shared<session>(std::move(socket_))->run();
		}

		// Accept another connection
		do_accept();
	}
};




void wait2(int seconds)
{
	boost::this_thread::sleep_for(boost::chrono::seconds{ seconds });
}


void egressWSThread(std::vector<std::shared_ptr<Datum>> datumList)
{

	mDatumList = datumList;

	auto const address = boost::asio::ip::make_address("127.0.0.1");
	//auto const address = boost::asio::ip::address_v4::loopback;
	//auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
	auto const port = static_cast<unsigned short>(7878);
	//auto const threads = std::max<int>(1, std::atoi(argv[3]));
	auto const threads = 1;

	// The io_context is required for all I/O
	boost::asio::io_context ioc{ threads };

	// Create and launch a listening port
	std::make_shared<listener>(ioc, tcp::endpoint{ address, port })->run();

	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for (auto i = threads - 1; i > 0; --i)
		v.emplace_back(
			[&ioc]
	{
		ioc.run();
	});
	ioc.run();

}