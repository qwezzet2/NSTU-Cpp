#include "Data.cpp"

int Data::objectCount = 0;

// Дружественная функция для оператора +=
Data& operator+=(Data& lhs, const Data& rhs)
{
    *lhs.day += *rhs.day;
    *lhs.month += *rhs.month;
    *lhs.year += *rhs.year;
    *lhs.hour += *rhs.hour;
    *lhs.minute += *rhs.minute;
    *lhs.second += *rhs.second;
    lhs.Normalize();
    lhs.updateString();
    return lhs;
}

// Дружественная функция для оператора -=
Data& operator-=(Data& lhs, const Data& rhs)
{
    *lhs.day -= *rhs.day;
    *lhs.month -= *rhs.month;
    *lhs.year -= *rhs.year;
    *lhs.hour -= *rhs.hour;
    *lhs.minute -= *rhs.minute;
    *lhs.second -= *rhs.second;
    lhs.Normalize();
    lhs.updateString();
    return lhs;
}

// Перегрузка оператора вывода
ostream& operator<<(ostream& out, const Data& data)
{
    out << *data.day << " " << *data.month << " " << *data.year << " "
        << *data.hour << " " << *data.minute << " " << *data.second;
    return out;
}

// Перегрузка оператора ввода
istream& operator>>(istream& in, Data& data)
{
    in >> *data.day >> *data.month >> *data.year
        >> *data.hour >> *data.minute >> *data.second;
    data.Normalize(); // Нормализация и обновление строки
    data.updateString();
    return in;
}