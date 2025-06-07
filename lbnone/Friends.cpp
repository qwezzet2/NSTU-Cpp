#include "Data.cpp"

int Data::objectCount = 0;

// ������������� ������� ��� ��������� +=
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

// ������������� ������� ��� ��������� -=
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

// ���������� ��������� ������
ostream& operator<<(ostream& out, const Data& data)
{
    out << *data.day << " " << *data.month << " " << *data.year << " "
        << *data.hour << " " << *data.minute << " " << *data.second;
    return out;
}

// ���������� ��������� �����
istream& operator>>(istream& in, Data& data)
{
    in >> *data.day >> *data.month >> *data.year
        >> *data.hour >> *data.minute >> *data.second;
    data.Normalize(); // ������������ � ���������� ������
    data.updateString();
    return in;
}