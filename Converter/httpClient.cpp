#include "httpClient.hpp"


httpClient::httpClient(const std::string& host) : resolver_(ioc_), socket_(ioc_), host_(host) {
    // reslove adress and establish a connection
   asio::connect(socket_, resolver_.resolve(host, "80"));
}

httpClient::~httpClient() {
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both); //close the socket
}

http::response<beast::http::dynamic_body> httpClient::get(const std::string& target) {
    //creating an HTTP GET request with a target
    http::request<http::string_body> req(http::verb::get, target, 11);
    //Set HTTP header fields
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    //Sending a request via an attached socket
    http::write(socket, req);

    boost::beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(socket, buffer, res);

    return res;
}

httpClient::httpClient(const std::string& date) :
    target(std::format(
        "/historical?access_key=50f2dab13e92481fb0ae68c221ba2716&date={}&format=1",
        date)) {
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



