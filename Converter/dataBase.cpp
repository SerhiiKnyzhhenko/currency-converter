#include "dataBase.hpp"

std::string conn = "host=localhost port=5432 dbname=converter user=postgres password=12345Kikki";

dataBase::dataBase() : connectionObject(conn), worker(connectionObject) {
	if (!connectionObject.is_open()) {
		throw std::runtime_error("Failed to connect to PostgreSQL");
	}
}

void dataBase::add_resp_to_hash(const std::string& date, std::unordered_map<std::string, double>& rates) {
	try
	{
		if (!connectionObject.is_open()) {
			throw std::runtime_error("Problems with connections to db");
		}

		pqxx::result response = worker.exec(
			"SELECT currency, rate FROM rates WHERE date = $1",  
			pqxx::params(date));

		for (const auto& row : response) {
			std::string currency = row[0].as<std::string>();
			double rate = row[1].as<double>();		
			rates[currency] = rate;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}	
}

void dataBase::add_to_db_from_hash(const std::string& date, const std::unordered_map<std::string, double>& rates) {
	try
	{
		if (!connectionObject.is_open()) {
			throw std::runtime_error("Problems with connections to db");
		}

		for (const auto& element : rates)
		{
			std::string currency = element.first;
			double rate = element.second;

			worker.exec(
				"INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
				pqxx::params{ date, currency, rate });
		}	
		worker.commit();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void dataBase::add_to_db_row(const std::string& date, const std::string& currency, double rate) {
	try
	{
		if (!connectionObject.is_open()) {
			throw std::runtime_error("Problems with connections to db");
		}

		if (isDateValid(date)) {
			worker.exec(
				"INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
				pqxx::params(date, currency, rate));

			worker.commit();
		}
		else {
			std::cerr << "Date is no valid!" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

}

std::string dataBase::get_current_date() {
	auto now = std::chrono::system_clock::now();
	auto today = std::chrono::current_zone()->to_local(now);
	auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

	return std::format("{:%Y-%m-%d}", ymd);
}

bool dataBase::isDateValid(const std::string& date) {
	std::regex pattern(R"(\d{4}-\d{2}-\d{2})");
	return std::regex_match(date, pattern);
}

std::string dataBase::get_custom_date(int y, int m, int d) {
	using namespace std::chrono;
	year_month_day ymd{ year(y), month(m), day(d) };
	return std::format("{:%Y-%m-%d}", ymd);
}

bool dataBase::сheckDataForDate(const std::string& date) {
	try
	{
		if (!connectionObject.is_open()) {
			throw std::runtime_error("Problems with connections to db");
		}

		pqxx::result response = worker.exec(
			"SELECT currency FROM rates WHERE date = $1",
			pqxx::params(date));
		
		if (response.size() > 0)
			return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return false;
}
