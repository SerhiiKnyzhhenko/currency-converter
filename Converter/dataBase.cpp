#include "dataBase.hpp"

static std::string fPath = "C:/Users/12345/OneDrive/Desktop/pass.txt";

// Constructor: establishes database connection
dataBase::dataBase() {

    std::ifstream file(fPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + fPath);
    }

    std::string conn;
    if (!getline(file, conn)) {
        throw std::runtime_error("Failed to read connection string");
    }
    file.close();

    connectionObject_ = std::make_unique<pqxx::connection>(conn);
    if (!connectionObject_->is_open()) {
        throw std::runtime_error("Failed to connect to PostgreSQL");
    }
    worker_ = std::make_unique<pqxx::work>(*connectionObject_);
}

// Loads currency rates for specified date into hashmap
void dataBase::add_resp_to_hash(const std::string& date, std::unordered_map<std::string, double>& rates) {
    try {
        if (!connectionObject_->is_open()) {
            throw std::runtime_error("Problems with connections to db");
        }

        // Execute parameterized query to prevent SQL injection
        pqxx::result response = worker_->exec(
            "SELECT currency, rate FROM rates WHERE date = $1",
            pqxx::params(date));

        // Populate rates map from query results
        for (const auto& row : response) {
            std::string currency = row[0].as<std::string>();
            double rate = row[1].as<double>();
            rates[currency] = rate;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// Stores currency rates from hashmap into database
void dataBase::add_to_db_from_hash(const std::string& date, const std::unordered_map<std::string, double>& rates) {
    try {
        if (!connectionObject_->is_open()) {
            throw std::runtime_error("Problems with connections to db");
        }

        // Batch insert currency rates
        for (const auto& element : rates) {
            std::string currency = element.first;
            double rate = element.second;

            worker_->exec(
                "INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
                pqxx::params{ date, currency, rate });
        }
        worker_->commit();  // Commit transaction after all inserts
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// Inserts single currency rate into database
void dataBase::add_to_db_row(const std::string& date, const std::string& currency, double rate) {
    try {
        if (!connectionObject_->is_open()) {
            throw std::runtime_error("Problems with connections to db");
        }

        if (isDateValid(date)) {
            worker_->exec(
                "INSERT INTO rates (date, currency, rate) VALUES ($1, $2, $3)",
                pqxx::params(date, currency, rate));

            worker_->commit();
        }
        else {
            std::cerr << "Date is no valid!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// Returns current date in YYYY-MM-DD format
std::string dataBase::get_current_date() {
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::current_zone()->to_local(now);
    auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

    return std::format("{:%Y-%m-%d}", ymd);
}

// Validates date format (YYYY-MM-DD)
bool dataBase::isDateValid(const std::string& date) {
    std::regex pattern(R"(\d{4}-\d{2}-\d{2})");
    return std::regex_match(date, pattern);
}

// Generates custom date string from components
std::string dataBase::get_custom_date(int y, int m, int d) {
    using namespace std::chrono;
    year_month_day ymd{ year(y), month(m), day(d) };
    return std::format("{:%Y-%m-%d}", ymd);
}

// Checks if data exists for specified date
bool dataBase::checkDataForDate(const std::string& date) {
    try {
        if (!connectionObject_->is_open()) {
            throw std::runtime_error("Problems with connections to db");
        }

        pqxx::result response = worker_->exec(
            "SELECT currency FROM rates WHERE date = $1",
            pqxx::params(date));

        return !response.empty();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return false;
}
