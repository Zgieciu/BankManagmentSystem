#include <iostream>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <fstream>

using namespace std;

int personID = 1, accountID = 1;

struct Date {
    int day;
    int month;
    int year;
};

struct PersonData {
    int personID;
    int accountID;
    char firstName[30];
    char lastName[30];
    char pesel[11];
    Date date;
};

struct AccountData {
    int accountID;
    int accountNumber;
    float amountOfMoney;
    Date openedDate;
};

struct Account {
    AccountData data;
    Account* next;
};

struct Person {
    PersonData data;
    Person* next;
};

typedef Person* pointer;
typedef Account* pointerA;

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

bool checkPersonID(int id, pointer persons) {
    while (id != persons->data.personID) {
        persons = persons->next;
        if (persons == NULL) {
            return false;
        }
    }
    return true;
}

PersonData personData() {
    PersonData data;
    cout << "Podaj imiê: ";
    cin >> data.firstName;
    cout << "Podaj nazwisko: ";
    cin >> data.lastName;
    do {
        cout << "Podaj datê urodzenia dd/mm/yyyy: ";
        cin >> data.date.day >> data.date.month >> data.date.year;
    } while (!checkDate(data.date.day, data.date.month, data.date.year));
    do {
        cout << "Podaj pesel: ";
        cin >> data.pesel;
    } while (strlen(data.pesel) != 11);
    data.accountID = NULL;
    return data;
}

AccountData accountData() {
    AccountData data;
    cout << "Podaj iloœc pieniêdzy na start:";
    cin >> data.amountOfMoney;
    return data;
}

void addPerson(pointer* persons, PersonData data) {
    pointer newPerson;
    newPerson = (Person*)malloc(sizeof(Person));
    newPerson->data = data;
    newPerson->data.personID = personID;
    newPerson->next = NULL;
    if ((*persons) == NULL){
        (*persons) = newPerson;
        personID++;
    } 
    else
        addPerson(&(*persons)->next, data);
}

void showPersons(pointer persons) {
    while (persons != NULL) {
        int day = persons->data.date.day;
        int month = persons->data.date.month;
        int year = persons->data.date.year;
        cout << persons->data.personID << "." << persons->data.firstName << " " << persons->data.lastName << " ";
        (day < 10 ? cout << "0" << day : cout << day) << "/";
        (month < 10 ? cout << "0" << month : cout << month) << "/";
        cout << year << " ";
        cout << persons->data.pesel << endl;
        persons = persons->next;
    }
}

void showAccounts(pointerA accounts, pointer persons) {
    while (accounts != NULL) {
        cout << accounts->data.accountID << "." << accounts->data.amountOfMoney;
        while (persons->data.accountID != accounts->data.accountID) {
            persons = persons->next;
        }
        cout << "  Owner:" << persons->data.pesel;
        accounts = accounts->next;
    }
}

void savePersonsToFile(pointer persons) {
    FILE *file;
    int size = sizeof(PersonData);
    file = fopen("persons.dat", "wb");
    if (file == NULL) return;
    while (persons != NULL) {
        fwrite(&(persons->data), size, 1, file);
        persons = persons->next;
    }
    fclose(file);
}

void readPersonsFromFile(pointer *persons) {
    FILE* file;
    int size = sizeof(PersonData);
    PersonData data;
    file = fopen("persons.dat", "rb");
    if (file == NULL) 
        return;
    while (fread(&data, size, 1, file) == 1)
        addPerson(&(*persons), data);
    fclose(file);
}

void addAccount(pointerA* accounts, pointer persons, AccountData data) {
    pointerA newAccount;
    newAccount = (Account*)malloc(sizeof(Account));
    newAccount->data = data;
    newAccount->data.accountID = accountID;
    persons->data.accountID = accountID;
    newAccount->next = NULL;
    if ((*accounts) == NULL) {
        (*accounts) = newAccount;
        accountID++;
    }
    else
        addAccount(&(*accounts)->next, persons, data);
}

int main()
{
    setlocale(LC_CTYPE, "Polish");
    pointer persons = NULL;
    pointerA accounts = NULL;
    int option = 0, enter, personID;
    bool check;

    readPersonsFromFile(&persons);

    while (option != 10) {
        cout << "SYSTEM ZARZ¥DZANIA BANKIEM" << endl;
        cout << "\n1.Dodaj u¿ytkownika";
        cout << "\n2.Wyœwietl u¿ytkowników";
        cout << "\n3.Wyœwietl konta";
        cout << "\n4.Utwórz konto";
        cout << "\n10.Zakoñcz program\n" << endl;

        cout << "Wybierz opcje: ";
        cin >> option;
        cout << endl;

        switch (option) {
            case 1:
                PersonData pData = personData();
                addPerson(&persons, pData);
                break;
            case 2:
                showPersons(persons);
                break;
            case 3:
                showAccounts(accounts, persons);
                break;
            case 4:
                showPersons(persons);
                cout << "\nWybierz po ID u¿ytnownika któremu chcesz utworzyæ konto: ";
                cin >> personID;
                check = checkPersonID(personID, persons);
                if (!check) {
                    cout << "Nie ma u¿yktownika o podanym ID";
                    break;
                };
                AccountData aData = accountData();
                addAccount(&accounts, persons, aData);
                break;
            case 10:
                cout << "Dziêkujemy za u¿ycie programu. Wszystkie dane zostan¹ zapisane do bazy danych.";
                break;
            default:
                cout << "Nie ma opcji o takim numerze";
                break;
        }

        if (option == 10) 
            break;
        cout << "\nAby przejœæ dalej naciœnij ENTER";
        while (true) {
            enter = getchar();
            if (cin.peek() == '\n')
                break;
        }
        system("cls");
    }
    
    savePersonsToFile(persons);

    return 0;
}