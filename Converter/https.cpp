#include "https.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace http = boost::beast::http;


//http://apilayer.net/api/live
//http://api.currencylayer.com/live?access_key=50f2dab13e92481fb0ae68c221ba2716&format=1
//? access_key = 50f2dab13e92481fb0ae68c221ba2716
//& currencies = EUR, GBP, CAD, PLN
//& source = USD
//& format = 1

void https_get() {

    const std::string host = "api.currencylayer.com";
    const std::string target = "/live?access_key=50f2dab13e92481fb0ae68c221ba2716&format=1";

    // I/O контекст, необходимый для всех I/O операций
    boost::asio::io_context ioc;

    // Resolver для определения endpoint'ов
    boost::asio::ip::tcp::resolver resolver(ioc);
    // Tcp сокет, использующейся для соединения
    boost::asio::ip::tcp::socket socket(ioc);

    // Резолвим адрес и устанавливаем соединение
    boost::asio::connect(socket, resolver.resolve(host, "80"));

    // Дальше необходимо создать HTTP GET реквест с указанием таргета
    http::request<http::string_body> req(http::verb::get, target, 11);
    // Задаём поля HTTP заголовка
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Отправляем реквест через приконекченный сокет
    http::write(socket, req);

    // Часть, отвечающая за чтение респонса
    {
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        //"C:/Users/12345/OneDrive/Desktop/file.json"
        std::string file_name = "C:/Users/12345/OneDrive/Desktop/file.json";
        std::ofstream outf(file_name);
        
        if (!outf.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_name);
        }

        outf << res; //wreete to file

        outf.close();
    }
    // Закрываем соединение
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}