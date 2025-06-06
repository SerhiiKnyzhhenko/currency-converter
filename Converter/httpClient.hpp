﻿#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <iostream>
#include <fstream>
#include <format>
#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "responeHandler.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = boost::beast::http;
using tcp = asio::ip::tcp;

class httpClient {
private:
	asio::io_context ioc_;
	std::string host_ = "api.currencylayer.com";
	tcp::resolver resolver_;
	tcp::socket socket_;

public:
	httpClient();
	~httpClient();

	void get_json_body(const std::string& target);
};

#endif // !HTTPS_H_

