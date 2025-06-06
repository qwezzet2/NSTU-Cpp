#define _CRT_SECURE_NO_WARNINGS // ��������� �������������� � ������������ �������� � Visual Studio
#include "BinaryFile.h" // ����������� ������������� ����� ��� ������ � BinaryFile
#include <conio.h> // ��� ������������� ������� _getch() ��� ����� ��������
#include <windows.h> // ��� ������������� ������� system("cls") ��� ������� �������
#include <locale.h> // ��� ��������� �����������
#include <iostream> // ��� ����� � ������
#include <vector>         // ��� std::vector
#include <string>         // ��� std::string
#include <random>         // ��� std::random_device, std::mt19937, std::uniform_int_distribution
#include <algorithm>      // ��� std::sort
#include <chrono>         // ��� ��������� ������� ����������
using namespace std; // �������� ������������� ��������� ������������ ������������ ����
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
        // ������� ������ � ������� �� 1 �� size
        vector<int> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = i + 1;  // ��������� ������� �� 1 �� size
        }

        // ������� ������ BinaryFile
        BinaryFile bin_file;

        // ��������� ������ � ������ BinaryFile
        for (const auto& num : data) {
            bin_file.add_record(to_string(num));  // ��������� ����� ��� ������
        }

        auto start = chrono::high_resolution_clock::now();

        // ��������� ������ � ������� ����� ������� ����������
        bin_file.sort_records(true);  // ���������� �� �����������

        auto end = chrono::high_resolution_clock::now();

        cout << "���������� " << size << " ��������� ������ "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " �����������.\n";
    }
}





void testInsertionPerformance(const string& filename) {
    vector<size_t> sizes = { 1, 10, 100, 300, 500, 1000 };

    for (size_t size : sizes) {
        BinaryFile file(filename.c_str(), ios::out | ios::trunc);
        auto start = chrono::high_resolution_clock::now();

        for (size_t i = 0; i < size; ++i) {
            file.add_record("��������� ������ " + to_string(i));
        }

        auto end = chrono::high_resolution_clock::now();
        file.save(); // ��������� ���������
        cout << "���������� " << size << " ��������� ������ "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " �����������.\n";
    }
}


// ������� ��� ������ ���� ������������
void printMenu()
{
    // ������ ������� ����
    cout << "\n ���� \n";
    cout << "||1|| ������� ����� �������� ����\n";
    cout << "||2|| ��������� �������� ����\n";
    cout << "||3|| ���������� ��������\n";
    cout << "||4|| �������� ������\n";
    cout << "||5|| ��������� �������� ����\n";
    cout << "||6|| ��������� �� ���������� �����\n";
    cout << "||7|| ��������� � ��������� ����\n";
    cout << "||8|| C���������\n";
    cout << "||9|| �����\n\n";
    cout << "�������� ��������:\n";
}

// �������� ������� ���������, �������������� ������ ����
void menu()
{
    setlocale(LC_ALL, "Rus"); // ������������� ������� ����������� ��� ����������� ����������� ������
    unique_ptr<BinaryFile> bin_file; // ����� ��������� �� ������ BinaryFile, ��� ��������������� ���������� �������
    bool running = true; // ���� ��� ������ ����� ��������� ����

    // ������� ���� ���������
    while (running)
    {
        system("cls"); // ������� ������� ����� ������������ ����
        // ���� �������� ���� ��������, ���������� ��� ����������
        if (bin_file)
        {
            try {
                cout << "���������� �����:\n";
                bin_file->display_records(); // ����� ������ ��� ����������� ������� �� �����
                cout << endl;
            }
            catch (const exception& e) {
                // ��������� ������, ��������� � ������������
                cout << "������ ��� ����������� �������: " << e.what() << endl;
            }
        }
        else
        {
            // ���� ���� �� ��������, ������� ���������
            cout << "���� �� ��������.\n";
        }

        printMenu(); // ������� ���� ������������

        char choice = _getch(); // ������� ������ ������������ ��� ������������� �������� Enter
        try
        {
            switch (choice)
            {
            case '1': {
                // �������� ������ ��������� �����
                cout << "������� ��� ������ �����: ";
                string filename;
                cin >> filename;

                // ���������, ���������� �� ���� � ����� ������
                ifstream file_check(filename);
                if (file_check.is_open()) {
                    cout << "������: ���� � ����� ������ ��� ����������.\n";
                    file_check.close();
                }
                else {
                    try {
                        bin_file = make_unique<BinaryFile>(filename.c_str(), ios::out | ios::trunc); // ������� ����� ����
                        cout << "���� ������� ������.\n";
                        log("���� " + filename + " ������� ������"); // �������� ��������
                    }
                    catch (const exception& e) {
                        cout << "������ ��� �������� �����: " << e.what() << endl;
                    }
                }
                system("pause"); // ����� ����� ��������� � ����
                break;
            }
            case '2': {
                // �������� ������������� ��������� �����
                cout << "������� ��� ����� ��� ��������: ";
                string filename;
                cin >> filename;
                try {
                    bin_file = make_unique<BinaryFile>(filename.c_str(), ios::in | ios::out); // ��������� ����
                    cout << "���� ��������.\n";
                    log("���� " + filename + " ������� ��������");
                }
                catch (const exception& e) {
                    cout << "������ ��� �������� �����: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '3': {
                // ���������� �������� � �����
                if (!bin_file)
                {
                    // ���� ���� �� ��������, ������� ���������
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }

                // ������� ��� ���������� ��������
                cout << "||1|| �������� ������\n"
                    << "||2|| �������� ������ �� �������\n"
                    << "||3|| ������� ������\n"
                    << "||4|| ������������� ������\n"
                    << "�������� ��������: ";
                char sub_choice = _getch(); // ���� �������
                try {
                    switch (sub_choice)
                    {
                    case '1': {
                        // ���������� ����� ������
                        cout << "������� ������ ��� ����������: ";
                        string record;
                        if (cin.peek() == '\n') {
                            cin.ignore();
                        }
                        getline(cin, record); // ��������� ������ � ���������
                        bin_file->add_record(record); // ��������� ������ � ����
                        cout << "������ ���������.\n";
                        break;
                    }
                    case '2': {
                        // ������� ������ �� �������
                        cout << "������� ������: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("������ �����: ��������� �����");
                        }
                        cout << "������� ������ ��� �������: ";
                        string record;
                        cin.ignore();
                        getline(cin, record);
                        bin_file->insert_record(index, record); // ��������� ������
                        cout << "������ ���������.\n";
                        break;
                    }
                    case '3': {
                        // �������� ������ �� �������
                        cout << "������� ������ ��� ��������: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("������ �����: ��������� �����");
                        }
                        bin_file->delete_record(index); // ������� ������
                        cout << "������ �������.\n";
                        break;
                    }
                    case '4': {
                        // �������������� ������
                        cout << "������� ������ ��� ��������������: ";
                        int index;
                        if (!(cin >> index)) {
                            throw runtime_error("������ �����: ��������� �����");
                        }
                        cout << "������� ����� ������: ";
                        string record;
                        cin.ignore();
                        getline(cin, record);
                        bin_file->edit_record(index, record); // ����������� ������
                        cout << "������ ���������.\n";
                        break;
                    }
                    default:
                        cout << "�������� �����.\n";
                        break;
                    }
                }
                catch (const exception& e) {
                    // ��������� ������ � ���������� ��������
                    cout << "������ ��� ������ � ��������: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '4': {
                // �������� ��� ������
                if (!bin_file)
                {
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }
                try {
                    bin_file->display_records(); // ���������� ������
                }
                catch (const exception& e) {
                    cout << "������ ��� ����������� �������: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '5': {
                // ���������� ��������� �����
                if (!bin_file)
                {
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }
                try {
                    bin_file->save(); // ��������� ������ � ����
                    cout << "���� ��������.\n";
                    log("���� ������� ��������");
                }
                catch (const exception& e) {
                    cout << "������ ��� ���������� �����: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '6': {
                // �������� ������ �� ���������� �����
                if (!bin_file) {
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }
                try {
                    cout << "������� ��� ���������� ����� ��� ��������: ";
                    string text_filename;
                    cin >> text_filename;
                    bin_file->load_from_text(text_filename); // ��������� ��������� ������
                    cout << "��������� �� ���������� �����.\n";
                }
                catch (const exception& e) {
                    cout << "������ ��� �������� ���������� �����: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '7': {
                // ���������� ������ � ��������� ����
                if (!bin_file)
                {
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }
                try {
                    cout << "������� ��� ���������� ����� ��� ����������: ";
                    string text_filename;
                    cin >> text_filename;
                    bin_file->save_to_text(text_filename); // ��������� ������ � ��������� ����
                    cout << "��������� � ��������� ����.\n";
                }
                catch (const exception& e) {
                    cout << "������ ��� ���������� ���������� �����: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '8': {
                if (!bin_file) {
                    cout << "������� �������� ��� ��������� ����.\n";
                    system("pause");
                    break;
                }

                // ������� ����������
                cout << "�������� ������� ����������:\n"
                    << "||1|| �� �����������\n"
                    << "||2|| �� ��������\n"
                    << "��� �����: ";
                char sort_choice = _getch();
                try {
                    if (sort_choice == '1') {
                        bin_file->sort_records(true); // ���������� �� �����������
                        cout << "������ ������������� �� �����������.\n";
                        log("������ ������������� �� �����������");
                    }
                    else if (sort_choice == '2') {
                        bin_file->sort_records(false); // ���������� �� ��������
                        cout << "������ ������������� �� ��������.\n";
                        log("������ ������������� �� ��������");
                    }
                    else {
                        cout << "�������� �����.\n";
                    }
                }
                catch (const exception& e) {
                    cout << "������ ��� ����������: " << e.what() << endl;
                }
                system("pause");
                break;
            }
            case '9':
                // ����� �� ���������
                running = false;
                break;
            case 'q': {
                cout << "���� ����������:\n";
                testSortingPerformance();
                system("pause");
                break;
            }
            case 'w': {
                cout << "������� ��� ��������� ����� ��� �����: ";
                string filename;
                cin >> filename;
                testInsertionPerformance(filename);
                system("pause");
                break;
            }
            default:
                // ��������� ��������� �����
                cout << "�������� ����.\n";
                system("pause");
                break;
            }

        }
        catch (const exception& e)
        {
            // ��������� ������ �������� ������
            cout << "������: " << e.what() << endl;
            system("pause");
        }
    }
}

// ����� ����� � ���������
int main()
{
    menu(); // ��������� ����
    return 0; // ���������� ��������� � ����� ��������� ����������
}
