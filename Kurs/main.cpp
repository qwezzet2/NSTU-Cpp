#define _CRT_SECURE_NO_WARNINGS // Отключает предупреждения о небезопасных функциях в Visual Studio
#include "BinaryFile.h" // Подключение заголовочного файла для работы с BinaryFile
#include <conio.h> // Для использования функции _getch() для ввода символов
#include <windows.h> // Для использования функции system("cls") для очистки консоли
#include <locale.h> // Для установки локализации
#include <iostream> // Для ввода и вывода
#include <vector>         // Для std::vector
#include <string>         // Для std::string
#include <random>         // Для std::random_device, std::mt19937, std::uniform_int_distribution
#include <algorithm>      // Для std::sort
#include <chrono>         // Для измерения времени выполнения
using namespace std; // Упрощает использование элементов стандартного пространства имен
vector<string> generateRandomStrings(size_t count, size_t length = 10) {
    vector<string> strings;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis('a', 'z');

    for (size_t i = 0; i < count; ++i) {
        string str;
        for (size_t j = 0; j < length; ++j) {
            str += static_cast<char>(dis(gen));
        }
        strings.push_back(str);
    }
    return strings;
}
void testSortingPerformance() {
    vector<size_t> sizes = { 1, 10, 100, 300, 500, 1000 };
    for (size_t size : sizes) {
        // Создаем вектор с числами от 1 до size
        vector<int> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = i + 1;  // Заполняем числами от 1 до size
        }

        // Создаем объект BinaryFile
        BinaryFile bin_file;

        // Добавляем данные в объект BinaryFile
        for (const auto& num : data) {
            bin_file.add_record(to_string(num));  // Добавляем числа как строки
        }

        auto start = chrono::high_resolution_clock::now();

        // Сортируем данные с помощью вашей функции сортировки
        bin_file.sort_records(true);  // Сортировка по возрастанию

        auto end = chrono::high_resolution_clock::now();

        cout << "Сортировка " << size << " элементов заняла "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " микросекунд.\n";
    }
}





void testInsertionPerformance(const string& filename) {
    vector<size_t> sizes = { 1, 10, 100, 300, 500, 1000 };

    for (size_t size : sizes) {
        BinaryFile file(filename.c_str(), ios::out | ios::trunc);
        auto start = chrono::high_resolution_clock::now();

        for (size_t i = 0; i < size; ++i) {
            file.add_record("Случайная запись " + to_string(i));
        }

        auto end = chrono::high_resolution_clock::now();
        file.save(); // Сохраняем изменения
        cout << "Добавление " << size << " элементов заняло "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " микросекунд.\n";
    }
}


// Функция для вывода меню пользователя
void printMenu()
{
    // Печать пунктов меню
    cout << "\n Меню \n";
    cout << "||1|| Создать новый двоичный файл\n";
    cout << "||2|| Загрузить двоичный файл\n";
    cout << "||3|| Управление записями\n";
    cout << "||4|| Показать записи\n";
    cout << "||5|| Сохранить двоичный файл\n";
    cout << "||6|| Загрузить из текстового файла\n";
    cout << "||7|| Сохранить в текстовый файл\n";
    cout << "||8|| Cортировка\n";
    cout << "||9|| Выход\n\n";
    cout << "Выберите действие:\n";
}

// Основная функция программы, обеспечивающая работу меню
void menu()
{
    setlocale(LC_ALL, "Rus"); // Устанавливает русскую локализацию для корректного отображения текста
    unique_ptr<BinaryFile> bin_file; // Умный указатель на объект BinaryFile, для автоматического управления памятью
    bool running = true; // Флаг для работы цикла основного меню

    // Главный цикл программы
    while (running)
    {
        system("cls"); // Очищает консоль перед отображением меню
        // Если двоичный файл загружен, отображаем его содержимое
        if (bin_file)
        {
            try {
                cout << "Содержимое файла:\n";
                bin_file->display_records(); // Вызов метода для отображения записей из файла
                cout << endl;
            }
            catch (const exception& e) {
                // Обработка ошибок, связанных с отображением
                cout << "Ошибка при отображении записей: " << e.what() << endl;
            }
        }
        else
        {
            // Если файл не загружен, выводим сообщение
            cout << "Файл не загружен.\n";
        }

        printMenu(); // Выводим меню пользователя

        char choice = _getch(); // Ожидаем выбора пользователя без необходимости нажимать Enter
        try
        {
            switch (choice)
            {
            case '1': {
                // Создание нового двоичного файла
                cout << "Введите имя нового файла: ";
                string filename;
                cin >> filename;

                // Проверяем, существует ли файл с таким именем
                ifstream file_check(filename);
                if (file_check.is_open()) {
                    cout << "Ошибка: файл с таким именем уже существует.\n";
                    file_check.close();
                }
                else {
                    try {
                        bin_file = make_unique<BinaryFile>(filename.c_str(), ios::out | ios::trunc); // Создаем новый файл
                        cout << "Файл успешно создан.\n";
                        log("Файл " + filename + " успешно создан"); // Логируем удаление
                    }
                    catch (const exception& e) {
                        cout << "Ошибка при создании файла: " << e.what() << endl;
                    }
                }
                system("pause"); // Пауза перед возвратом в меню
                break;
            }
            case '2': {
                // Загрузка существующего двоичного файла
                cout << "Введите имя файла для загрузки: ";
                string filename;
                cin >> filename;
                try {
                    bin_file = make_unique<BinaryFile>(filename.c_str(), ios::in | ios::out); // Загружаем файл
                    cout << "Файл загружен.\n";
                    log("Файл " + filename + " успешно загружен");
                }
                catch (const exception& e) {
                    cout << "Ошибка при загрузке файла: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '3': {
                // Управление записями в файле
                if (!bin_file)
                {
                    // Если файл не загружен, выводим сообщение
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }

                // Подменю для управления записями
                cout << "||1|| Добавить запись\n"
                    << "||2|| Вставить запись по индексу\n"
                    << "||3|| Удалить запись\n"
                    << "||4|| Редактировать запись\n"
                    << "Выберите действие: ";
                char sub_choice = _getch(); // Ввод подменю
                try {
                    switch (sub_choice)
                    {
                    case '1': {
                        // Добавление новой записи
                        cout << "Введите строку для добавления: ";
                        string record;
                        if (cin.peek() == '\n') {
                            cin.ignore();
                        }
                        getline(cin, record); // Считываем строку с пробелами
                        bin_file->add_record(record); // Добавляем запись в файл
                        cout << "Запись добавлена.\n";
                        break;
                    }
                    case '2': {
                        // Вставка записи по индексу
                        cout << "Введите индекс: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("Ошибка ввода: требуется число");
                        }
                        cout << "Введите строку для вставки: ";
                        string record;
                        cin.ignore();
                        getline(cin, record);
                        bin_file->insert_record(index, record); // Вставляем запись
                        cout << "Запись вставлена.\n";
                        break;
                    }
                    case '3': {
                        // Удаление записи по индексу
                        cout << "Введите индекс для удаления: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("Ошибка ввода: требуется число");
                        }
                        bin_file->delete_record(index); // Удаляем запись
                        cout << "Запись удалена.\n";
                        break;
                    }
                    case '4': {
                        // Редактирование записи
                        cout << "Введите индекс для редактирования: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("Ошибка ввода: требуется число");
                        }
                        cout << "Введите новую строку: ";
                        string record;
                        cin.ignore();
                        getline(cin, record);
                        bin_file->edit_record(index, record); // Редактируем запись
                        cout << "Запись обновлена.\n";
                        break;
                    }
                    default:
                        cout << "Неверный выбор.\n";
                        break;
                    }
                }
                catch (const exception& e) {
                    // Обработка ошибок в управлении записями
                    cout << "Ошибка при работе с записями: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '4': {
                // Показать все записи
                if (!bin_file)
                {
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }
                try {
                    bin_file->display_records(); // Отображаем записи
                }
                catch (const exception& e) {
                    cout << "Ошибка при отображении записей: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '5': {
                // Сохранение двоичного файла
                if (!bin_file)
                {
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }
                try {
                    bin_file->save(); // Сохраняем данные в файл
                    cout << "Файл сохранен.\n";
                    log("Файл успешно сохранен");
                }
                catch (const exception& e) {
                    cout << "Ошибка при сохранении файла: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '6': {
                // Загрузка данных из текстового файла
                if (!bin_file) {
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }
                try {
                    cout << "Введите имя текстового файла для загрузки: ";
                    string text_filename;
                    cin >> text_filename;
                    bin_file->load_from_text(text_filename); // Загружаем текстовые данные
                    cout << "Загружено из текстового файла.\n";
                }
                catch (const exception& e) {
                    cout << "Ошибка при загрузке текстового файла: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '7': {
                // Сохранение данных в текстовый файл
                if (!bin_file)
                {
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }
                try {
                    cout << "Введите имя текстового файла для сохранения: ";
                    string text_filename;
                    cin >> text_filename;
                    bin_file->save_to_text(text_filename); // Сохраняем данные в текстовый файл
                    cout << "Сохранено в текстовый файл.\n";
                }
                catch (const exception& e) {
                    cout << "Ошибка при сохранении текстового файла: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '8': {
                if (!bin_file) {
                    cout << "Сначала создайте или загрузите файл.\n";
                    system("pause");
                    break;
                }

                // Подменю сортировки
                cout << "Выберите порядок сортировки:\n"
                    << "||1|| По возрастанию\n"
                    << "||2|| По убыванию\n"
                    << "Ваш выбор: ";
                char sort_choice = _getch();
                try {
                    if (sort_choice == '1') {
                        bin_file->sort_records(true); // Сортировка по возрастанию
                        cout << "Записи отсортированы по возрастанию.\n";
                        log("Записи отсортированы по возрастанию");
                    }
                    else if (sort_choice == '2') {
                        bin_file->sort_records(false); // Сортировка по убыванию
                        cout << "Записи отсортированы по убыванию.\n";
                        log("Записи отсортированы по убыванию");
                    }
                    else {
                        cout << "Неверный выбор.\n";
                    }
                }
                catch (const exception& e) {
                    cout << "Ошибка при сортировке: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '9':
                // Выход из программы
                running = false;
                break;
            case 'q': {
                cout << "Тест сортировки:\n";
                testSortingPerformance();
                system("pause");
                break;
            }
            case 'w': {
                cout << "Введите имя бинарного файла для теста: ";
                string filename;
                cin >> filename;
                testInsertionPerformance(filename);
                system("pause");
                break;
            }
            default:
                // Обработка неверного ввода
                cout << "Неверный ввод.\n";
                system("pause");
                break;
            }

        }
        catch (const exception& e)
        {
            // Обработка ошибок верхнего уровня
            cout << "Ошибка: " << e.what() << endl;
            system("pause");
        }
    }
}

// Точка входа в программу
int main()
{
    menu(); // Запускает меню
    return 0; // Завершение программы с кодом успешного выполнения
}
