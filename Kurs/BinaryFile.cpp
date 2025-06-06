#include "BinaryFile.h"

// ����������� �� ��������� ��� ������� StringRecord
// �������������� ���������� length (����� ������) � 0 � data (��������� �� ������) � nullptr
StringRecord::StringRecord() : length(0), data(nullptr) {}

// ����������� � ���������� ��� ������������� StringRecord �������
// ����������� ������ � ����������� ���������� ������ ��������
StringRecord::StringRecord(const string& str) {
    length = static_cast<int>(str.size()); // ������������� ����� ������
    data = make_unique<char[]>(length); // �������� ������ ��� �������� ��������
    copy(str.begin(), str.end(), data.get()); // �������� ������ � ���������� ������
}

// ������������ ����������� ��� ����������� ������ �� ������� ������� StringRecord
// ���������� ��������� �� ������, � ����� ���������� ����� � ��������� �������
StringRecord::StringRecord(StringRecord&& other) noexcept
    : length(other.length), data(move(other.data)) {
    other.length = 0; // �������� ����� � ������������� �������
}

// ������������ �������� ������������
// ���������� ������ �� ������� ������� StringRecord, ��������� ����� � ��������� �������
StringRecord& StringRecord::operator=(StringRecord&& other) noexcept {
    if (this != &other) { // �������� �� ����������������
        length = other.length; // �������� �����
        data = move(other.data); // ���������� ������
        other.length = 0; // �������� ����� � ��������� �������
    }
    return *this;
}

// ��������� ������ � ����� (����)
void StringRecord::save(ostream& os) const {
    os.write(reinterpret_cast<const char*>(&length), sizeof(length)); // ���������� ����� ������
    os.write(data.get(), length); // ���������� ������ ������
}

// ��������� ������ �� ������ (�����)
void StringRecord::load(istream& is) {
    is.read(reinterpret_cast<char*>(&length), sizeof(length)); // ������ ����� ������
    data = make_unique<char[]>(length); // �������� ������ ��� ������
    is.read(data.get(), length); // ������ ������ ������
}

// ����������� ������ � ������
string StringRecord::to_string() const {
    return string(data.get(), length); // ���������� ������, ��������� ������ �� �������
}

// �������� ������ ��� ������ � �����
ostream& operator<<(ostream& os, const StringRecord& record) {
    os << record.to_string(); // ������� ��������� ������������� ������
    return os;
}

// �������� ����� ��� ������ �� ������
istream& operator>>(istream& is, StringRecord& record) {
    string temp;
    getline(is, temp); // ������ ������
    record = StringRecord(temp); // ������� ������ �� ������ ��������� ������
    return is;
}

#pragma region BinaryFile

// ����������� ��� ��������� �����
// ��������� ���� � ��������� ������ � �������, ��������� ������, ���� �������� ������ �������
BinaryFile::BinaryFile(const string& filename, ios::openmode mode)
    : array_size(0), current_count(0), pointer_array_offset(0) {
    open(filename, mode | ios::binary); // �������� ����� � �������� ������
    if (!is_open()) {
        throw runtime_error("�� ������� ������� ����: " + filename); // ������ ��� ��������
    }
    if (mode & ios::in) {
        load(); // ��������� ������, ���� ���� ������ ��� ������
    }
}

// ����������, ������� ��������� ����
BinaryFile::~BinaryFile() {
    close(); // ��������� ���� ��� ����������� �������
}

// ������� �����������, ������������ ��������� � ���� log.txt


void log(const string& message) {
    // �������� ������� �����
    auto now = chrono::system_clock::now();
    auto time_t_now = chrono::system_clock::to_time_t(now); // ����������� � time_t

    // �������� ��������� �����
    tm local_tm;
    localtime_s(&local_tm, &time_t_now); // ��� ���������� ������ � localtime � Windows

    // ����������� ����� � ������
    stringstream time_stream;
    time_stream << put_time(&local_tm, "%Y-%m-%d %H:%M:%S"); // ����������� ���� � �����

    // �������� ����� ��� ���������� ���������
    ofstream log_file("log.txt", ios::app);
    if (log_file.is_open()) {
        log_file << time_stream.str() << " - " << message << endl; // ���������� ����� � ���������
    }
}


// ���������� ����� ������ � ����� �����
void BinaryFile::add_record(const string& str) {
    try {
        records.emplace_back(str); // ��������� ������
        update_pointers(); // ��������� ���������
        log("��������� ������: " + str); // �������� ���������� ������
    }
    catch (const exception& e) {
        throw runtime_error("������ ��� ���������� ������: "s + e.what()); // ��������� ������
    }
}

// ������� ������ �� �������
void BinaryFile::insert_record(size_t index, const string& str) {
    try {
        if (index > records.size()) {
            throw out_of_range("������ ��� ���������: " + to_string(index)); // �������� �� ������������ �������
        }
        records.insert(records.begin() + index, StringRecord(str)); // ��������� ������
        update_pointers(); // ��������� ���������
        log("��������� ������ �� ������� " + to_string(index) + " : " + str); // �������� ����������
    }
    catch (const exception& e) {
        throw runtime_error("������ ��� ������� ������: "s + e.what()); // ��������� ������
    }
}

// �������� ������ �� �������
void BinaryFile::delete_record(size_t index) {
    try {
        string old_record = records[index].to_string(); // ��������� ������ ������ ��� ����
        if (index >= records.size()) {
            throw out_of_range("������ ��� ���������: " + to_string(index)); // �������� �� ������������ �������
        }
        records.erase(records.begin() + index); // ������� ������
        update_pointers(); // ��������� ���������
        log("������ �� ������� " + to_string(index) + " �������: " + old_record); // �������� ��������
    }
    catch (const exception& e) {
        throw runtime_error("������ ��� �������� ������: "s + e.what()); // ��������� ������
    }
}

// �������������� ������ �� �������
void BinaryFile::edit_record(size_t index, const string& str) {
    try {
        if (index >= records.size()) {
            throw out_of_range("������ ��� ���������: " + to_string(index)); // �������� �� ������������ �������
        }
        string old_record = records[index].to_string(); // ��������� ������ ������ ��� ����
        records[index] = StringRecord(str); // �������� ������
        update_pointers(); // ��������� ���������
        log("������ �� ������� " + to_string(index) + " ��������: " + old_record + " -> " + str); // �������� ���������
    }
    catch (const exception& e) {
        throw runtime_error("������ ��� �������������� ������: "s + e.what()); // ��������� ������
    }
}

// ���������� ���� ������� � ����
void BinaryFile::save() {
    // �������� �������� �����
    if (!is_open()) {
        throw runtime_error("���� �� ������ ��� ������");
    }

    // ������� ������ � ��������� ��������� ������ � ������
    clear();
    seekp(0, ios::beg);

    // ���������� ���������
    array_size = static_cast<int>(records.size());
    current_count = static_cast<int>(records.size());
    pointer_array_offset = sizeof(array_size) + sizeof(current_count) + sizeof(pointer_array_offset);

    write(reinterpret_cast<const char*>(&array_size), sizeof(array_size));
    write(reinterpret_cast<const char*>(&current_count), sizeof(current_count));
    write(reinterpret_cast<const char*>(&pointer_array_offset), sizeof(pointer_array_offset));

    // ��������� ���������
    update_pointers();

    // ���������� ������ ����������
    for (size_t pointer : pointers) {
        write(reinterpret_cast<const char*>(&pointer), sizeof(pointer));
    }

    // ���������� ������
    for (const auto& record : records) {
        record.save(*this);
    }

    // �������������� ����� ������ �� ����
    flush();
}


void BinaryFile::load() {
    // ������� ������� ������
    clear();

    // ������������� ��������� �� ������ �����
    seekg(0, std::ios::beg);

    // �������� �� �������� �����
    if (!is_open()) {
        throw std::runtime_error("�� ������� ������� ���� ��� ��������.");
    }

    // ������ ���������
    if (!read(reinterpret_cast<char*>(&array_size), sizeof(array_size))) {
        throw std::runtime_error("������ ��� ������ array_size.");
    }
    if (!read(reinterpret_cast<char*>(&current_count), sizeof(current_count))) {
        throw std::runtime_error("������ ��� ������ current_count.");
    }
    if (!read(reinterpret_cast<char*>(&pointer_array_offset), sizeof(pointer_array_offset))) {
        throw std::runtime_error("������ ��� ������ pointer_array_offset.");
    }

    // ������ ����������
    pointers.resize(array_size);
    if (!read(reinterpret_cast<char*>(pointers.data()), array_size * sizeof(size_t))) {
        throw std::runtime_error("������ ��� ������ ����������");
    }

    // ������ �������
    records.clear();
    try {
        for (int i = 0; i < current_count; ++i) {
            StringRecord record;
            record.load(*this); // ��������� ������
            records.emplace_back(std::move(record)); // ��������� ������
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("������ ��� �������� �������: ") + e.what());
    }
}


// ���������� ���������� ��� ������ ������
void BinaryFile::update_pointers() {
    pointers.clear(); // ������� ������ ���������

    // ������������� ��������� �������� ��� ������ ������
    size_t offset = pointer_array_offset + array_size * sizeof(size_t);

    // ��������� ������ ����������
    for (const auto& record : records) {
        pointers.push_back(offset);
        offset += sizeof(int) + record.to_string().size(); // �������� ��� ��������� ������
    }
}

// ����� ���� ������� �� �����
void BinaryFile::display_records() const {
    for (size_t i = 0; i < records.size(); ++i) {
        cout << i + 1 << ". " << records[i].to_string() << endl; // �������� ������ � ���������� ������
    }
}
void BinaryFile::sort_records(bool ascending) {
    for (size_t i = 1; i < records.size(); ++i) {
        StringRecord key = std::move(records[i]); // ���������� ����������� ��� �����������
        size_t j = i;

        // ���������� ������ � ������ ������� ����������
        while (j > 0 &&
            (ascending ? records[j - 1].to_string() > key.to_string()
                : records[j - 1].to_string() < key.to_string())) {
            records[j] = std::move(records[j - 1]); // ����������� ��� �������������� �����������
            --j;
        }

        records[j] = std::move(key); // ��������� ������� ������� �� ��� �����
    }
}






// �������� ������ �� ���������� �����
void BinaryFile::load_from_text(const string& text_filename) {
    try {
        ifstream text_file(text_filename); // ��������� ��������� ����
        if (!text_file.is_open()) {
            throw runtime_error("�� ������� ������� ��������� ����: " + text_filename);
        }
        records.clear(); // ������� ������� ������
        StringRecord record;
        while (text_file >> record) {
            records.emplace_back(std::move(record)); // ��������� ������ �� �����
        }
        if (text_file.bad()) {
            throw runtime_error("������ ��� ������ ���������� �����");
        }
        update_pointers(); // ��������� ���������
        log("������ ��������� �� ���������� �����: " + text_filename); // ��������
    }
    catch (const exception& e) {
        log("������ ��� �������� �� ���������� �����: " + text_filename + ". �������: " + e.what());
        throw runtime_error("������ ��� �������� �� ���������� �����: "s + e.what());
    }
}

// ���������� ������� � ��������� ����
void BinaryFile::save_to_text(const string& text_filename) const {
    try {
        ofstream text_file(text_filename); // ��������� ��������� ���� ��� ������
        if (!text_file.is_open()) {
            throw runtime_error("�� ������� ������� ��������� ���� ��� ������: " + text_filename);
        }
        for (const auto& record : records) {
            text_file << record << endl; // ���������� ������ ������ � ����
        }
        if (text_file.bad()) {
            throw runtime_error("������ ��� ������ � ��������� ����");
        }
        log("������ ��������� � ��������� ����: " + text_filename); // ��������
    }
    catch (const exception& e) {
        log("������ ��� ���������� � ��������� ����: " + text_filename + ". �������: " + e.what());
        throw runtime_error("������ ��� ���������� � ��������� ����: "s + e.what());
    }
}

// ���������� ��������� ������ ��� ��������� �����
ostream& operator<<(ostream& os, const BinaryFile& file) {
    os << file.records.size() << '\n'; // ������� ���������� �������
    for (const auto& record : file.records) {
        os << record << '\n'; // ������� ������ ������
    }
    return os;
}

// ���������� ��������� ����� ��� ��������� �����
istream& operator>>(istream& is, BinaryFile& file) {
    file.records.clear(); // ������� ������� ������
    size_t count;
    is >> count; // ������ ���������� �������
    is.ignore(); // ���������� ������ ����� ������
    for (size_t i = 0; i < count; ++i) {
        StringRecord record;
        if (is >> record) {
            file.records.emplace_back(move(record)); // ��������� ������
        }
        else {
            throw runtime_error("������ ��� ������ ������ �� ���������� �����"); // ��������� ������
        }
    }
    return is;
}

// ���������� ��������� ������ � �������� ����
ofstream& operator<<(ofstream& ofs, const BinaryFile& file) {
    int count = static_cast<int>(file.records.size());
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(count)); // ���������� ���������� �������
    for (const auto& record : file.records) {
        record.save(ofs); // ��������� ������ ������
    }
    return ofs;
}

// ���������� ��������� ����� �� ��������� �����
ifstream& operator>>(ifstream& ifs, BinaryFile& file) {
    file.records.clear(); // ������� ������� ������
    int count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(count)); // ������ ���������� �������
    if (count < 0) {
        throw runtime_error("������������ ���������� ������� � �������� �����");
    }
    for (int i = 0; i < count; ++i) {
        StringRecord record;
        record.load(ifs); // ��������� ������
        file.records.emplace_back(move(record)); // ��������� ������
    }
    return ifs;
}

#pragma endregion