#include "data_base.h"

data_base::data_base() : connectionObject(conn), worker(connectionObject) {}

pqxx::result data_base::new_response(const std::string& response_text) {

}

void data_base::add_to_db_from_hash(const std::string& date, const std::unordered_map<std::string, double>& rates) {
	try
	{
		for (const auto& element : rates)
		{
			std::string currency = element.first;
			double rate = element.second;

			worker.exec_params(
				"INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
				date, currency, rate);

			worker.commit();
		}
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void data_base::add_to_db_row(const std::string& date, const std::string& currency, double rate) {
	try
	{
		if (isDateValid(date)) {
			worker.exec_params(
				"INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
				date, currency, rate);

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

std::string data_base::get_current_date() {
	auto now = std::chrono::system_clock::now();
	auto today = std::chrono::current_zone()->to_local(now);
	auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

	return std::format("{:%Y-%m-%d}", ymd);
}

bool data_base::isDateValid(const std::string& date) {
	std::regex pattern(R"(\d{4}-\d{2}-\d{2})");
	return std::regex_match(date, pattern);
}
