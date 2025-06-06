#include "BinaryFile.h"

// Конструктор по умолчанию для объекта StringRecord
// Инициализирует переменные length (длина строки) в 0 и data (указатель на данные) в nullptr
StringRecord::StringRecord() : length(0), data(nullptr) {}

// Конструктор с параметром для инициализации StringRecord строкой
// Преобразует строку в динамически выделенный массив символов
StringRecord::StringRecord(const string& str) {
    length = static_cast<int>(str.size()); // Устанавливаем длину строки
    data = make_unique<char[]>(length); // Выделяем память для хранения символов
    copy(str.begin(), str.end(), data.get()); // Копируем строку в выделенную память
}

// Перемещающий конструктор для перемещения данных из другого объекта StringRecord
// Перемещает указатель на данные, а также сбрасывает длину у исходного объекта
StringRecord::StringRecord(StringRecord&& other) noexcept
    : length(other.length), data(move(other.data)) {
    other.length = 0; // Обнуляем длину у перемещаемого объекта
}

// Перемещающий оператор присваивания
// Перемещает данные из другого объекта StringRecord, сбрасывая длину у исходного объекта
StringRecord& StringRecord::operator=(StringRecord&& other) noexcept {
    if (this != &other) { // Проверка на самоприсваивание
        length = other.length; // Копируем длину
        data = move(other.data); // Перемещаем данные
        other.length = 0; // Обнуляем длину у исходного объекта
    }
    return *this;
}

// Сохраняет запись в поток (файл)
void StringRecord::save(ostream& os) const {
    os.write(reinterpret_cast<const char*>(&length), sizeof(length)); // Записываем длину записи
    os.write(data.get(), length); // Записываем данные строки
}

// Загружает запись из потока (файла)
void StringRecord::load(istream& is) {
    is.read(reinterpret_cast<char*>(&length), sizeof(length)); // Читаем длину записи
    data = make_unique<char[]>(length); // Выделяем память для данных
    is.read(data.get(), length); // Читаем данные строки
}

// Преобразует запись в строку
string StringRecord::to_string() const {
    return string(data.get(), length); // Возвращаем строку, используя данные из массива
}

// Оператор вывода для строки в поток
ostream& operator<<(ostream& os, const StringRecord& record) {
    os << record.to_string(); // Выводим строковое представление записи
    return os;
}

// Оператор ввода для строки из потока
istream& operator>>(istream& is, StringRecord& record) {
    string temp;
    getline(is, temp); // Читаем строку
    record = StringRecord(temp); // Создаем запись на основе считанной строки
    return is;
}

#pragma region BinaryFile

// Конструктор для бинарного файла
// Открывает файл с указанным именем и режимом, загружает данные, если открытие прошло успешно
BinaryFile::BinaryFile(const string& filename, ios::openmode mode)
    : array_size(0), current_count(0), pointer_array_offset(0) {
    open(filename, mode | ios::binary); // Открытие файла в бинарном режиме
    if (!is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename); // Ошибка при открытии
    }
    if (mode & ios::in) {
        load(); // Загружаем данные, если файл открыт для чтения
    }
}

// Деструктор, который закрывает файл
BinaryFile::~BinaryFile() {
    close(); // Закрывает файл при уничтожении объекта
}

// Функция логирования, записывающая сообщения в файл log.txt


void log(const string& message) {
    // Получаем текущее время
    auto now = chrono::system_clock::now();
    auto time_t_now = chrono::system_clock::to_time_t(now); // Преобразуем в time_t

    // Получаем локальное время
    tm local_tm;
    localtime_s(&local_tm, &time_t_now); // Для безопасной работы с localtime в Windows

    // Форматируем время в строку
    stringstream time_stream;
    time_stream << put_time(&local_tm, "%Y-%m-%d %H:%M:%S"); // Форматируем дату и время

    // Открытие файла для добавления сообщений
    ofstream log_file("log.txt", ios::app);
    if (log_file.is_open()) {
        log_file << time_stream.str() << " - " << message << endl; // Записываем время и сообщение
    }
}


// Добавление новой записи в конец файла
void BinaryFile::add_record(const string& str) {
    try {
        records.emplace_back(str); // Добавляем запись
        update_pointers(); // Обновляем указатели
        log("Добавлена запись: " + str); // Логируем добавление записи
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка при добавлении записи: "s + e.what()); // Обработка ошибок
    }
}

// Вставка записи по индексу
void BinaryFile::insert_record(size_t index, const string& str) {
    try {
        if (index > records.size()) {
            throw out_of_range("Индекс вне диапазона: " + to_string(index)); // Проверка на корректность индекса
        }
        records.insert(records.begin() + index, StringRecord(str)); // Вставляем запись
        update_pointers(); // Обновляем указатели
        log("Добавлена запись по индексу " + to_string(index) + " : " + str); // Логируем добавление
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка при вставке записи: "s + e.what()); // Обработка ошибок
    }
}

// Удаление записи по индексу
void BinaryFile::delete_record(size_t index) {
    try {
        string old_record = records[index].to_string(); // Сохраняем старую запись для лога
        if (index >= records.size()) {
            throw out_of_range("Индекс вне диапазона: " + to_string(index)); // Проверка на корректность индекса
        }
        records.erase(records.begin() + index); // Удаляем запись
        update_pointers(); // Обновляем указатели
        log("Запись по индексу " + to_string(index) + " удалена: " + old_record); // Логируем удаление
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка при удалении записи: "s + e.what()); // Обработка ошибок
    }
}

// Редактирование записи по индексу
void BinaryFile::edit_record(size_t index, const string& str) {
    try {
        if (index >= records.size()) {
            throw out_of_range("Индекс вне диапазона: " + to_string(index)); // Проверка на корректность индекса
        }
        string old_record = records[index].to_string(); // Сохраняем старую запись для лога
        records[index] = StringRecord(str); // Заменяем запись
        update_pointers(); // Обновляем указатели
        log("Запись по индексу " + to_string(index) + " изменена: " + old_record + " -> " + str); // Логируем изменение
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка при редактировании записи: "s + e.what()); // Обработка ошибок
    }
}

// Сохранение всех записей в файл
void BinaryFile::save() {
    // Проверка открытия файла
    if (!is_open()) {
        throw runtime_error("Файл не открыт для записи");
    }

    // Очистка потока и установка указателя записи в начало
    clear();
    seekp(0, ios::beg);

    // Записываем заголовок
    array_size = static_cast<int>(records.size());
    current_count = static_cast<int>(records.size());
    pointer_array_offset = sizeof(array_size) + sizeof(current_count) + sizeof(pointer_array_offset);

    write(reinterpret_cast<const char*>(&array_size), sizeof(array_size));
    write(reinterpret_cast<const char*>(&current_count), sizeof(current_count));
    write(reinterpret_cast<const char*>(&pointer_array_offset), sizeof(pointer_array_offset));

    // Обновляем указатели
    update_pointers();

    // Записываем массив указателей
    for (size_t pointer : pointers) {
        write(reinterpret_cast<const char*>(&pointer), sizeof(pointer));
    }

    // Записываем строки
    for (const auto& record : records) {
        record.save(*this);
    }

    // Принудительный сброс буфера на диск
    flush();
}


void BinaryFile::load() {
    // Очищаем текущие данные
    clear();

    // Устанавливаем указатель на начало файла
    seekg(0, std::ios::beg);

    // Проверка на открытие файла
    if (!is_open()) {
        throw std::runtime_error("Не удалось открыть файл для загрузки.");
    }

    // Чтение заголовка
    if (!read(reinterpret_cast<char*>(&array_size), sizeof(array_size))) {
        throw std::runtime_error("Ошибка при чтении array_size.");
    }
    if (!read(reinterpret_cast<char*>(&current_count), sizeof(current_count))) {
        throw std::runtime_error("Ошибка при чтении current_count.");
    }
    if (!read(reinterpret_cast<char*>(&pointer_array_offset), sizeof(pointer_array_offset))) {
        throw std::runtime_error("Ошибка при чтении pointer_array_offset.");
    }

    // Чтение указателей
    pointers.resize(array_size);
    if (!read(reinterpret_cast<char*>(pointers.data()), array_size * sizeof(size_t))) {
        throw std::runtime_error("Ошибка при чтении указателей");
    }

    // Чтение записей
    records.clear();
    try {
        for (int i = 0; i < current_count; ++i) {
            StringRecord record;
            record.load(*this); // Загружаем запись
            records.emplace_back(std::move(record)); // Добавляем запись
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Ошибка при загрузке записей: ") + e.what());
    }
}


// Обновление указателей для каждой записи
void BinaryFile::update_pointers() {
    pointers.clear(); // Очищаем старые указатели

    // Устанавливаем начальное смещение для первой записи
    size_t offset = pointer_array_offset + array_size * sizeof(size_t);

    // Заполняем массив указателей
    for (const auto& record : records) {
        pointers.push_back(offset);
        offset += sizeof(int) + record.to_string().size(); // Смещение для следующей записи
    }
}

// Вывод всех записей на экран
void BinaryFile::display_records() const {
    for (size_t i = 0; i < records.size(); ++i) {
        cout << i + 1 << ". " << records[i].to_string() << endl; // Печатаем индекс и содержимое записи
    }
}
void BinaryFile::sort_records(bool ascending) {
    for (size_t i = 1; i < records.size(); ++i) {
        StringRecord key = std::move(records[i]); // Используем перемещение для оптимизации
        size_t j = i;

        // Сравниваем строки с учётом порядка сортировки
        while (j > 0 &&
            (ascending ? records[j - 1].to_string() > key.to_string()
                : records[j - 1].to_string() < key.to_string())) {
            records[j] = std::move(records[j - 1]); // Перемещение для предотвращения копирования
            --j;
        }

        records[j] = std::move(key); // Вставляем текущий элемент на своё место
    }
}






// Загрузка данных из текстового файла
void BinaryFile::load_from_text(const string& text_filename) {
    try {
        ifstream text_file(text_filename); // Открываем текстовый файл
        if (!text_file.is_open()) {
            throw runtime_error("Не удалось открыть текстовый файл: " + text_filename);
        }
        records.clear(); // Очищаем текущие записи
        StringRecord record;
        while (text_file >> record) {
            records.emplace_back(std::move(record)); // Добавляем запись из файла
        }
        if (text_file.bad()) {
            throw runtime_error("Ошибка при чтении текстового файла");
        }
        update_pointers(); // Обновляем указатели
        log("Данные загружены из текстового файла: " + text_filename); // Логируем
    }
    catch (const exception& e) {
        log("Ошибка при загрузке из текстового файла: " + text_filename + ". Причина: " + e.what());
        throw runtime_error("Ошибка при загрузке из текстового файла: "s + e.what());
    }
}

// Сохранение записей в текстовый файл
void BinaryFile::save_to_text(const string& text_filename) const {
    try {
        ofstream text_file(text_filename); // Открываем текстовый файл для записи
        if (!text_file.is_open()) {
            throw runtime_error("Не удалось открыть текстовый файл для записи: " + text_filename);
        }
        for (const auto& record : records) {
            text_file << record << endl; // Записываем каждую запись в файл
        }
        if (text_file.bad()) {
            throw runtime_error("Ошибка при записи в текстовый файл");
        }
        log("Данные сохранены в текстовый файл: " + text_filename); // Логируем
    }
    catch (const exception& e) {
        log("Ошибка при сохранении в текстовый файл: " + text_filename + ". Причина: " + e.what());
        throw runtime_error("Ошибка при сохранении в текстовый файл: "s + e.what());
    }
}

// Перегрузка оператора вывода для бинарного файла
ostream& operator<<(ostream& os, const BinaryFile& file) {
    os << file.records.size() << '\n'; // Выводим количество записей
    for (const auto& record : file.records) {
        os << record << '\n'; // Выводим каждую запись
    }
    return os;
}

// Перегрузка оператора ввода для бинарного файла
istream& operator>>(istream& is, BinaryFile& file) {
    file.records.clear(); // Очищаем текущие записи
    size_t count;
    is >> count; // Читаем количество записей
    is.ignore(); // Игнорируем символ новой строки
    for (size_t i = 0; i < count; ++i) {
        StringRecord record;
        if (is >> record) {
            file.records.emplace_back(move(record)); // Добавляем запись
        }
        else {
            throw runtime_error("Ошибка при чтении записи из текстового файла"); // Обработка ошибок
        }
    }
    return is;
}

// Перегрузка оператора вывода в бинарный файл
ofstream& operator<<(ofstream& ofs, const BinaryFile& file) {
    int count = static_cast<int>(file.records.size());
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(count)); // Записываем количество записей
    for (const auto& record : file.records) {
        record.save(ofs); // Сохраняем каждую запись
    }
    return ofs;
}

// Перегрузка оператора ввода из бинарного файла
ifstream& operator>>(ifstream& ifs, BinaryFile& file) {
    file.records.clear(); // Очищаем текущие записи
    int count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(count)); // Читаем количество записей
    if (count < 0) {
        throw runtime_error("Некорректное количество записей в бинарном файле");
    }
    for (int i = 0; i < count; ++i) {
        StringRecord record;
        record.load(ifs); // Загружаем запись
        file.records.emplace_back(move(record)); // Добавляем запись
    }
    return ifs;
}

#pragma endregion