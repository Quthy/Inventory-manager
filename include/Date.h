#ifndef DATE_H
#define DATE_H

#include <string>

// A simple calendar date (YYYY-MM-DD) used for product expiry dates.
class Date {
public:
    Date();
    Date(int year, int month, int day);

    // Parses "YYYY-MM-DD". Returns false if the text is not a valid date.
    static bool parse(const std::string& text, Date& out);

    // Today's date according to the system clock.
    static Date today();

    // Number of days from this date to 'other' (negative if 'other' is earlier).
    int daysUntil(const Date& other) const;

    std::string toString() const;

    bool operator<(const Date& other) const;
    bool operator==(const Date& other) const;

private:
    int year_;
    int month_;
    int day_;

    // Converts the date to a day count so two dates can be subtracted.
    long toDayNumber() const;
};

#endif
