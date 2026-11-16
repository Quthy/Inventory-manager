#include "Date.h"

#include <cstdio>
#include <ctime>
#include <sstream>
#include <iomanip>

Date::Date() : year_(1970), month_(1), day_(1) {}

Date::Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

static bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int daysInMonth(int y, int m) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeapYear(y)) return 29;
    return days[m - 1];
}

bool Date::parse(const std::string& text, Date& out) {
    int y, m, d;
    char dash1, dash2;
    std::istringstream in(text);
    if (!(in >> y >> dash1 >> m >> dash2 >> d)) return false;
    if (dash1 != '-' || dash2 != '-') return false;
    if (y < 1900 || y > 2200 || m < 1 || m > 12) return false;
    if (d < 1 || d > daysInMonth(y, m)) return false;
    out = Date(y, m, d);
    return true;
}

Date Date::today() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    return Date(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
}

// Standard "days from civil" algorithm: turns a date into a running day count.
long Date::toDayNumber() const {
    int y = year_;
    int m = month_;
    if (m <= 2) { y -= 1; m += 12; }
    long era = y / 400;
    long yearOfEra = y - era * 400;
    long dayOfYear = (153 * (m - 3) + 2) / 5 + day_ - 1;
    long dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
    return era * 146097 + dayOfEra;
}

int Date::daysUntil(const Date& other) const {
    return static_cast<int>(other.toDayNumber() - toDayNumber());
}

std::string Date::toString() const {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << year_ << '-'
        << std::setw(2) << month_ << '-' << std::setw(2) << day_;
    return out.str();
}

bool Date::operator<(const Date& other) const {
    return toDayNumber() < other.toDayNumber();
}

bool Date::operator==(const Date& other) const {
    return toDayNumber() == other.toDayNumber();
}
