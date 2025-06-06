#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <chrono>

using namespace std;

// Упаковка структуры для точного соответствия бинарному формату файла
#pragma pack(push, 1)

void log(const string& message);

// Класс, представляющий строку переменной длины
class StringRecord {
private:
    int length;                  // Длина строки
    unique_ptr<char[]> data;     // Данные строки

public:
    StringRecord();
    explicit StringRecord(const string& str);
    void save(ostream& os) const;
    void load(istream& is);
    string to_string() const;

    friend ostream& operator<<(ostream& os, const StringRecord& record);
    friend istream& operator>>(istream& is, StringRecord& record);

    // Запрещаем копирование
    StringRecord(const StringRecord&) = delete;
    StringRecord& operator=(const StringRecord&) = delete;

    // Разрешаем перемещение
    StringRecord(StringRecord&& other) noexcept;
    StringRecord& operator=(StringRecord&& other) noexcept;
};
#pragma pack(pop)

// Класс, представляющий двоичный файл
class BinaryFile : public fstream {
private:
    int array_size;              // Размер массива указателей
    int current_count;           // Текущее количество указателей
    size_t pointer_array_offset; // Смещение массива указателей

    vector<StringRecord> records; // Список строк
    vector<size_t> pointers;      // Смещения записей в файле

    void update_pointers();       // Пересчёт указателей

public:
    BinaryFile() = default;
    explicit BinaryFile(const string& filename, ios::openmode mode);
    ~BinaryFile();

    void add_record(const string& str);
    void insert_record(size_t index, const string& str);
    void delete_record(size_t index);
    void edit_record(size_t index, const string& str);
    void save();
    void load();
    void sort_records(bool ascending);
    void display_records() const;
    void load_from_text(const string& text_filename);
    void save_to_text(const string& text_filename) const;

    friend ostream& operator<<(ostream& os, const BinaryFile& file);
    friend istream& operator>>(istream& is, BinaryFile& file);
    friend ofstream& operator<<(ofstream& ofs, const BinaryFile& file);
    friend ifstream& operator>>(ifstream& ifs, BinaryFile& file);
};

#endif // BINARYFILE_H
