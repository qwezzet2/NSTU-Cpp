#include "Data.cpp"
int Data::objectCount = 0;
int main()
{
    setlocale(LC_ALL, "Rus");
    // Создание объектов с разными значениями
    Data data1;
    Data data2(12, 12, 2022, 23, 45, 0);
    Data data3(data2);
    Data data4(data1);
    cout << endl;

    // Печать исходных данных
    data1.PrintData();
    data2.PrintData();
    data3.PrintData();
    data4.PrintData();
    cout << endl;
    Data2 data5(12, 12, 2022, 23, 45, 0);
    data5.PrintData();
    cout << endl;
    Event event1(12, 12, 2022, 23, 45, 0, "giyuehuiefd", "dhuiud", "dhuih");
    event1.PrintData();
    return 0;
}