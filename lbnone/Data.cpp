#include <locale.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring> // ��� strcpy
#pragma warning(disable : 4996)
using namespace std;
class Data
{
public:
    Data()
    {
        day = new int(1);
        month = new int(1);
        year = new int(2000);
        hour = new int(0);
        minute = new int(0);
        second = new int(0);
        strData = new string();
        updateString(); // ��������� ������ ����� ��� �������� �������
        ++objectCount;
    }

    Data(int valDay, int valMonth, int valYear, int valHour, int valMinute, int valSecond)
    {
        day = new int(valDay);
        month = new int(valMonth);
        year = new int(valYear);
        hour = new int(valHour);
        minute = new int(valMinute);
        second = new int(valSecond);
        strData = new string();
        updateString(); // ��������� ������ ����� ��� �������� �������
        ++objectCount;
    }

    Data(const Data& other) // ����������� �����������
    {
        this->day = new int(*other.day);
        this->month = new int(*other.month);
        this->year = new int(*other.year);
        this->hour = new int(*other.hour);
        this->minute = new int(*other.minute);
        this->second = new int(*other.second);
        this->strData = new string(*other.strData);
        ++objectCount;
    }

    // ������������� ���������� ���������� += � -=
    friend Data& operator+=(Data& lhs, const Data& rhs);
    friend Data& operator-=(Data& lhs, const Data& rhs);

    // ���������� ���������� �����/������
    friend ostream& operator<<(ostream& out, const Data& data);
    friend istream& operator>>(istream& in, Data& data);

    // �������� ������������
    Data& operator=(const Data& other)
    {
        // �������� ������������ ������ ����
        if (this == &other)
            return *this;
        // �������� ������ ��������
        delete day;
        delete month;
        delete year;
        delete hour;
        delete minute;
        delete second;
        delete strData;

        this->day = new int(*other.day);
        this->month = new int(*other.month);
        this->year = new int(*other.year);
        this->hour = new int(*other.hour);
        this->minute = new int(*other.minute);
        this->second = new int(*other.second);
        this->strData = new string(*other.strData);
        return *this;
    }

    // ���������� ���������� ��� ���������� ��������� �����������
    Data& operator+=(int days)
    {
        *day += days;
        Normalize();
        updateString();
        return *this;
    }

    Data operator+(int hours) const
    {
        Data temp(*this);
        *temp.hour += hours;
        temp.Normalize();
        temp.updateString();
        return temp;
    }

    Data& operator-=(int days)
    {
        *day -= days;
        Normalize();
        updateString();
        return *this;
    }

    Data operator-(int hours) const
    {
        Data temp(*this);
        *temp.hour -= hours;
        temp.Normalize();
        temp.updateString();
        return temp;
    }

    // �������������� � ���� char *
    operator char* () const
    {
        char* cstr = new char[strData->length() + 1];
        strcpy(cstr, strData->c_str());
        return cstr;
    }

    void PrintData() const
    {
        cout << *strData << endl;
    }

    static int GetObjectCount()
    {
        return objectCount;
    }

    static int DaysInMonth(int month, int year)
    {
        switch (month)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return IsLeapYear(year) ? 29 : 28;
        default:
            return 0;
        }
    }

    ~Data()
    {
        delete day;
        delete month;
        delete year;
        delete hour;
        delete minute;
        delete second;
        delete strData;
        --objectCount;
    }

    // ������� ������/������ � ��������� ����
    void SaveToTextFile(const string& filename) const
    {
        ofstream outFile(filename, ios::app);
        if (outFile.is_open())
        {
            outFile << *this << endl;
        }
        outFile.close();
    }

    static bool LoadFromTextFile(const string& filename, vector<Data>& dataList)
    {
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "�� ������� ������� ���� ��� ������." << endl;
            return false;
        }
        Data data;
        while (inFile >> data)
        {
            dataList.push_back(data);
        }
        inFile.close();
        return true;
    }

    // ������� ������/������ � �������� ����
    void SaveToBinaryFile(ofstream& outFile) const
    {
        outFile.write(reinterpret_cast<const char*>(day), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(month), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(year), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(hour), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(minute), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(second), sizeof(int));

        // ���������� ����� ������ � ���� ������
        size_t strLength = strData->length();
        outFile.write(reinterpret_cast<const char*>(&strLength), sizeof(size_t));
        outFile.write(strData->c_str(), strLength);
    }

    void LoadFromBinaryFile(ifstream& inFile)
    {
        inFile.read(reinterpret_cast<char*>(day), sizeof(int));
        inFile.read(reinterpret_cast<char*>(month), sizeof(int));
        inFile.read(reinterpret_cast<char*>(year), sizeof(int));
        inFile.read(reinterpret_cast<char*>(hour), sizeof(int));
        inFile.read(reinterpret_cast<char*>(minute), sizeof(int));
        inFile.read(reinterpret_cast<char*>(second), sizeof(int));

        // ������ ������
        size_t strLength;
        inFile.read(reinterpret_cast<char*>(&strLength), sizeof(size_t));
        char* buffer = new char[strLength + 1];
        inFile.read(buffer, strLength);
        buffer[strLength] = '\0';
        *strData = buffer;
        delete[] buffer;

        updateString();
    }

    static void SaveListToBinaryFile(const string& filename, const vector<Data>& dataList)
    {
        ofstream outFile(filename, ios::binary | ios::out);
        if (!outFile.is_open())
        {
            cerr << "�� ������� ������� ���� ��� ������." << endl;
            return;
        }

        for (const auto& data : dataList)
        {
            data.SaveToBinaryFile(outFile);
        }

        outFile.close();
    }

    static bool LoadListFromBinaryFile(const string& filename, vector<Data>& dataList)
    {
        ifstream inFile(filename, ios::binary | ios::in);
        if (!inFile.is_open())
        {
            cerr << "�� ������� ������� ���� ��� ������." << endl;
            return false;
        }

        while (inFile.peek() != EOF)
        {
            Data data;
            data.LoadFromBinaryFile(inFile);
            dataList.push_back(data);
        }

        inFile.close();
        return true;
    }

protected:
    int* day, * month, * year, * hour, * minute, * second;
    string* strData;
    static int objectCount;

    static bool IsLeapYear(int year)
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    void Normalize()
    {
        // �������
        if (*second >= 60)
        {
            *minute += *second / 60;
            *second %= 60;
        }
        else if (*second < 0)
        {
            *minute -= (1 + abs(*second) / 60);
            *second = 60 + (*second % 60);
        }

        // ������
        if (*minute >= 60)
        {
            *hour += *minute / 60;
            *minute %= 60;
        }
        else if (*minute < 0)
        {
            *hour -= (1 + abs(*minute) / 60);
            *minute = 60 + (*minute % 60);
        }

        // ����
        if (*hour >= 24)
        {
            *day += *hour / 24;
            *hour %= 24;
        }
        else if (*hour < 0)
        {
            *day -= (1 + abs(*hour) / 24);
            *hour = 24 + (*hour % 24);
        }

        // ������
        if (*month > 12)
        {
            *year += (*month - 1) / 12;
            *month = (*month - 1) % 12 + 1;
        }
        else if (*month < 1)
        {
            *year -= (1 + abs(*month) / 12);
            *month = 12 + (*month % 12);
        }

        // ���
        while (*day > DaysInMonth(*month, *year))
        {
            *day -= DaysInMonth(*month, *year);
            *month += 1;
            if (*month > 12)
            {
                *month = 1;
                *year += 1;
            }
        }
        while (*day < 1)
        {
            *month -= 1;
            if (*month < 1)
            {
                *month = 12;
                *year -= 1;
            }
            *day += DaysInMonth(*month, *year);
        }

        updateString(); // ��������� ��������� ������������� ����� ������������
    }

    void updateString()
    {
        // ��������� ������ � ����� � ��������
        *strData = to_string(*day) + "." + to_string(*month) + "." + to_string(*year) + " " +
            to_string(*hour) + ":" + to_string(*minute) + ":" + to_string(*second);
    }
};



class Data2 : public Data
{
public:
    using Data::Data;  // ������������ ������������� ��������
    void PrintData()
    {
        ConvertTo12HourFormat();
        cout << *strData << endl;  // ������������ ������ ��� ������ ������
    }
private:
    void ConvertTo12HourFormat()
    {
        *hour = *hour % 12;
        Normalize();
        updateString();
    }
};

class Event : public Data
{
public:
    using Data::Data;  // ������������ ������������� ��������

    // ����������� � ��������������� ����������� ��� �����������
    Event(int day, int month, int year, int hour, int minute, int second, const string& eventName, const string& eventDescription, const string& eventLocation)
        : Data(day, month, year, hour, minute, second), eventName(eventName), eventDescription(eventDescription), eventLocation(eventLocation) {}

    // ����� ��������� ������ � �����������
    void SetEventDetails(const string& name, const string& description, const string& location)
    {
        eventName = name;
        eventDescription = description;
        eventLocation = location;
    }
    void PrintData()
    {
        Normalize();
        updateString();
        cout << "Scheduled for: " << *strData << endl;
        cout << "Event: " << eventName << endl;
        cout << "Description: " << eventDescription << endl;
        cout << "Location: " << eventLocation << endl;
    }
private:
    string eventName;         // �������� �����������
    string eventDescription;  // �������� �����������
    string eventLocation;     // ����� ���������� �����������
};