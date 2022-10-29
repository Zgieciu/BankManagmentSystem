#include <iostream>
#include <string>
#include <windows.h>
#include <winbase.h>

using namespace std;

int ID = 1;

struct Date {
    int day;
    int month;
    int year;
};

struct Data {
    char firstName[30];
    char lastName[30];
    char pesel[11];
    Date date;
};

struct Person {
    int personID;
    Data data;
    Person* next;
};

typedef Person* pointer;

bool checkDate(int day, int month, int year) {
    SYSTEMTIME st;
    GetSystemTime(&st);
    if (day <= 0 || day > 31)
        return false;
    else if (month <= 0 || month > 12)
        return false;
    else if (year < 1900 || year > st.wYear - 16)
        return false;
    else
        return true;
}

Data personData() {
    Data data;
    cout << "Podaj imiê: ";
    cin >> data.firstName;
    cout << "Podaj nazwisko: ";
    cin >> data.lastName;
    do {
        cout << "Podaj datê urodzenia dd/mm/yyyy: ";
        cin >> data.date.day >> data.date.month >> data.date.year;
    } while (!checkDate(data.date.day, data.date.month, data.date.year));
    cout << "Podaj pesel: ";
    cin >> data.pesel;
    return data;
}

void addPerson(pointer* persons, Data data) {
    pointer newPerson;
    newPerson = (Person*)malloc(sizeof(Person));
    newPerson->personID = ID;
    newPerson->data = data;
    newPerson->next = NULL;
    if ((*persons) == NULL){
        (*persons) = newPerson;
        ID++;
    } 
    else
        addPerson(&(*persons)->next, data);
}

void showPersons(pointer persons) {
    while (persons != NULL) {
        int day = persons->data.date.day;
        int month = persons->data.date.month;
        int year = persons->data.date.year;
        cout << persons->personID << "." << persons->data.firstName << " " << persons->data.lastName << " ";
        (day < 10 ? cout << "0" << day : cout << day) << "/";
        (month < 10 ? cout << "0" << month : cout << month) << "/";
        cout << year << " ";
        cout << persons->data.pesel << endl;
        persons = persons->next;
    }
}

int main()
{
    setlocale(LC_CTYPE, "Polish");
    pointer persons = NULL;
    int option = 0, enter;

    while (option != 3) {
        cout << "SYSTEM ZARZ¥DZANIA BANKIEM" << endl;
        cout << "\n1.Dodaj u¿ytkownika";
        cout << "\n2.Wyœwietl u¿ytkowników";
        cout << "\n3.Zakoñcz program\n" << endl;

        cout << "Wybierz opcje: ";
        cin >> option;
        cout << endl;

        switch (option) {
            case 1:
                Data data = personData();
                addPerson(&persons, data);
                break;
            case 2:
                showPersons(persons);
                break;
            case 3:
                cout << "Dziêkujemy za u¿ycie programu.";
                break;
            default:
                cout << "Nie ma opcji o takim numerze";
                break;
        }

        if (option == 3) 
            break;
        cout << "\nAby przejœæ dalej naciœnij ENTER";
        enter = getchar();
        while (true) {
            if (cin.peek() == '\n')
                break;
        }
        system("cls");
    }
    
    return 0;
}