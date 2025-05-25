#include "dataBase.hpp"

static std::string fPath = "C:/Users/12345/OneDrive/Desktop/conn.txt";

// Constructor: establishes database connection
dataBase::dataBase() : connectionObject_{ nullptr }, worker_{ nullptr } {

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
    if (!worker_)
        throw std::runtime_error("Database transaction is not initialized");
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
        throw;
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
        throw;
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
            throw std::invalid_argument("Invalid date format: " + date);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

// Returns current date in YYYY-MM-DD format
std::string dataBase::get_current_date() const {
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::current_zone()->to_local(now);
    auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

    return std::format("{:%Y-%m-%d}", ymd);
}

// Validates date format (YYYY-MM-DD)
bool dataBase::isDateValid(const std::string& date) const {
    try {
        std::chrono::year_month_day ymd{};
        std::istringstream iss(date);
        iss >> std::chrono::parse("%F", ymd);
        return ymd.ok();
    }
    catch (...) {
        return false;
    }
}

// Generates custom date string from components
std::string dataBase::get_custom_date(int y, int m, int d) const {
    using namespace std::chrono;
    year_month_day ymd{ year(y), month(m), day(d) };
    return std::format("{:%Y-%m-%d}", ymd);
}

// Checks if data exists for specified date
bool dataBase::checkDataForDate(const std::string& date) const {
    try {

        if (!isDateValid(date)) {
            throw std::invalid_argument("Invalid date format: " + date);
        }


        if (!connectionObject_->is_open()) {
            throw std::runtime_error("Problems with connections to db");
        }

        pqxx::result response = worker_->exec(
            "SELECT currency FROM rates WHERE date = $1",
            pqxx::params(date));

        return !response.empty();
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] checkDataForDate: " << e.what() << std::endl;
        throw;
    }
    return false;
}
