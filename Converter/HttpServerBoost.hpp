#ifndef HTTP_SERVER_BOOST_HPP
#define HTTP_SERVER_BOOST_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <memory>
#include <thread>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

class HttpServerBoost {
public:
    HttpServerBoost(asio::io_context& io_context, unsigned short port)
        : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        ssl_context_(ssl::context::tlsv13_server) {
        configure_ssl_context();
    }

    void start() {
        accept_connection();
        std::cout << "Server started on port " << acceptor_.local_endpoint().port() << std::endl;
    }

    void stop() {
        acceptor_.close();
    }

private:
    void configure_ssl_context() {
        ssl_context_.set_options(
            ssl::context::default_workarounds |
            ssl::context::no_sslv2 |
            ssl::context::single_dh_use);

        ssl_context_.use_certificate_chain_file(certificatePath_);
        ssl_context_.use_private_key_file(keyPath_, ssl::context::pem);

        if (!caPath_.empty()) {
            ssl_context_.load_verify_file(caPath_);
            ssl_context_.set_verify_mode(ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);
        }
    }

    void accept_connection() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (ec) {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                    return;
                }

                auto ssl_stream = std::make_shared<ssl::stream<tcp::socket>>(
                    std::move(socket), ssl_context_
                );

                ssl_stream->async_handshake(
                    ssl::stream_base::server,
                    [ssl_stream, this](const boost::system::error_code& ec) {
                        if (ec) {
                            std::cerr << "Handshake error: " << ec.message() << std::endl;
                            return;
                        }
                        handle_request(ssl_stream);
                    });

                accept_connection();
            });
    }

    void handle_request(std::shared_ptr<ssl::stream<tcp::socket>> ssl_stream) {
        auto buffer = std::make_shared<beast::flat_buffer>();
        auto parser = std::make_shared<http::request_parser<http::string_body>>();

        http::async_read(
            *ssl_stream,
            *buffer,
            *parser,
            [this, ssl_stream, buffer, parser](boost::system::error_code ec, size_t) { // Добавлено this
                if (ec) {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                    return;
                }
                send_response(ssl_stream, parser->get());
            });
    }

    void send_response(std::shared_ptr<ssl::stream<tcp::socket>> ssl_stream,
        const http::request<http::string_body>& req) {
        std::cout << req.target().data() << std::endl;
        auto response = std::make_shared<http::response<http::string_body>>(
            http::status::ok, req.version(), "Hello, Boost!");

        response->set(http::field::content_type, "text/plain");
        response->body() = "Hello from Boost.Asio!";
        response->prepare_payload();

        http::async_write(
            *ssl_stream,
            *response,
            [ssl_stream, response](boost::system::error_code ec, size_t) {
                if (ec) {
                    std::cerr << "Write error: " << ec.message() << std::endl;
                    return;
                }
                ssl_stream->async_shutdown(
                    [ssl_stream](const boost::system::error_code& ec) {
                        if (ec) {
                            std::cerr << "Shutdown error: " << ec.message() << std::endl;
                        }
                    });
            });
    }


    // Пути к сертификатам (можно добавить сеттеры)
    std::string certificatePath_ = "C:/Users/12345/source/repos/Converter/Converter/certificates/certificate.crt";
    std::string keyPath_ = "C:/Users/12345/source/repos/Converter/Converter/certificates/private.key";
    std::string caPath_ = "";

    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    ssl::context ssl_context_;
    std::shared_ptr<ssl::stream<tcp::socket>> ssl_stream_;
    beast::flat_buffer buffer_;

};

#endif // !HTTP_SERVER_BOOST_HPP



