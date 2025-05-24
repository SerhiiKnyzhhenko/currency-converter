#include "httpClient.hpp"

//http://api.currencylayer.com/ 


httpClient::httpClient() : resolver_(ioc_), socket_(ioc_) {
    // reslove adress and establish a connection
   asio::connect(socket_, resolver_.resolve(host_, "80")); 
}

httpClient::~httpClient() {
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both); //close the socket
}

void httpClient::get_json_body(const std::string& target) {
    //creating an HTTP GET request with a target
    http::request<http::string_body> req(http::verb::get, target, 11);
    //Set HTTP header fields
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    //Sending a request via an attached socket
    http::write(socket_, req);

    boost::beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(socket_, buffer, res);

    responseHandler rh;
    rh.save_to_file(beast::buffers_to_string(res.body().data()));
}




