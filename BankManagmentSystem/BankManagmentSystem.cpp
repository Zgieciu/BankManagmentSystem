#include <iostream>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <fstream>
#include <ctime>

using namespace std;

int personID = 1, accountID = 1;

struct Date {
    int day;
    int month;
    int year;
};

struct PersonData {
    int personID;
    char firstName[30];
    char lastName[30];
    char pesel[11];
    Date date;
};

struct AccountData {
    int accountID;
    int personID;
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
    while (persons != NULL) {
        if (id == persons->data.personID) {
            return true;
        }
        persons = persons->next;  
    }
    return false;
}

bool checkAccountNumber(int accountNumber, pointerA accounts) {
    while (accounts != NULL) {
        if (accounts->data.accountNumber == accountNumber) {
            return true;
        }
        accounts = accounts->next;
    }
    return false;
}

string checkOwner(pointerA accounts, pointer persons) {
    if (persons == NULL) return "";
    while (persons->data.personID != accounts->data.personID) {
        persons = persons->next;
    }
    return persons->data.pesel;
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
    return data;
}

AccountData accountData(pointerA accounts) {
    AccountData data;
    bool check = true;
    int random;
    time_t ttime = time(0);
    tm* local_time = localtime(&ttime);
    data.openedDate.year = 1900 + local_time->tm_year;
    data.openedDate.month = 1 + local_time->tm_mon;
    data.openedDate.day = local_time->tm_mday;
    while (check) {
        random = rand() % 89999 + 10000;
        data.accountNumber = random;
        check = false;
        while (accounts != NULL) {
            if (accounts->data.accountNumber == data.accountNumber) check = true;
            accounts = accounts->next;
        }
    } 
    cout << "Podaj iloœc pieniêdzy na start: ";
    cin >> data.amountOfMoney;
    return data;
}

void addPerson(pointer* persons, PersonData data) {
    if ((*persons) == NULL){
        pointer newPerson;
        newPerson = (Person*)malloc(sizeof(Person));
        newPerson->data = data;
        newPerson->data.personID = personID;
        newPerson->next = NULL;
        (*persons) = newPerson;
        personID++;
    } 
    else
        addPerson(&(*persons)->next, data);
}

void addAccount(pointerA* accounts, AccountData data, int ID) {
    if ((*accounts) == NULL) {
        pointerA newAccount;
        newAccount = (Account*)malloc(sizeof(Account));
        newAccount->data = data;
        newAccount->data.accountID = accountID;
        newAccount->data.personID = ID;
        newAccount->next = NULL;
        (*accounts) = newAccount;
        accountID++;
    }
    else
        addAccount(&(*accounts)->next, data, ID);
}

void showPersons(pointer persons) {
    if (persons == NULL) {
        cout << "Aktualnie w bazie nie ma u¿ytkowników.";
    }
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
    if (accounts == NULL) {
        cout << "Aktualnie w bazie nie ma kont.";
    }
    while (accounts != NULL) {
        cout << accounts->data.accountID << ". Nr konta: " << accounts->data.accountNumber;
        cout << " Fundusze: " << accounts->data.amountOfMoney;
        cout << "  W³aœciciel:" << checkOwner(accounts, persons) << endl;
        accounts = accounts->next;
    }
}

void savePersonsToFile(pointer persons) {
    FILE *file;
    int size = sizeof(PersonData);
    file = fopen("persons.dat", "wb");
    if (file == NULL) 
        return;
    while (persons != NULL) {
        fwrite(&(persons->data), size, 1, file);
        persons = persons->next;
    }
    fclose(file);
}

void readPersonsFromFile(pointer* persons) {
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

void saveAccountsToFile(pointerA accounts) {
    FILE* file;
    int size = sizeof(AccountData);
    file = fopen("accounts.dat", "wb");
    if (file == NULL) 
        return;
    while (accounts != NULL) {
        fwrite(&(accounts->data), size, 1, file);
        accounts = accounts->next;
    }
    fclose(file);
}

void readAccountsFromFile(pointerA* accounts) {
    FILE* file;
    int size = sizeof(AccountData);
    AccountData data;
    file = fopen("accounts.dat", "rb");
    if (file == NULL) 
        return;
    while (fread(&data, size, 1, file) == 1)
        addAccount(&(*accounts), data, data.personID);
    fclose(file);
}

void depositWithdrawMoney(pointerA accounts, int money, int accountNumber, int type) {
    while (accounts->data.accountNumber != accountNumber) {
        accounts = accounts->next;
    }
    if (type == 1) {
        accounts->data.amountOfMoney += money;
    }
    else if (type == 2) {
        if (money > accounts->data.amountOfMoney) {
            cout << "Brak wystarczj¹cych œrodków na koncie.";
            return;
        }
        accounts->data.amountOfMoney -= money;
    }
    cout << "Stan konta po wykonaniu transakcji: " << accounts->data.amountOfMoney;
}

int main()
{
    setlocale(LC_CTYPE, "Polish");
    pointer persons = NULL;
    pointerA accounts = NULL;
    int option = 0, breakKey, personID, depositOrWithdraw, accountNumber, amountOfMoney;
    bool check;

    readPersonsFromFile(&persons);
    readAccountsFromFile(&accounts);
    while (option != 10) {
        cout << "SYSTEM ZARZ¥DZANIA BANKIEM" << endl;
        cout << "\n1.Dodaj u¿ytkownika";
        cout << "\n2.Wyœwietl u¿ytkowników";
        cout << "\n3.Wyœwietl konta";
        cout << "\n4.Utwórz konto";
        cout << "\n5.Wp³aæ / Wyp³aæ pieni¹dze";
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
                if (persons == NULL) {
                    cout << "Nie mo¿na dodaæ konta poniewa¿ w bazie nie ma jeszcze u¿ytkowników.";
                    break;
                }
                showPersons(persons);
                cout << "\nWybierz po ID u¿ytnownika któremu chcesz utworzyæ konto: ";
                cin >> personID;
                check = checkPersonID(personID, persons);
                while (!check) {
                    cout << "Nie ma u¿yktownika o podanym ID. Podaj ID jeszcze raz: ";
                    cin >> personID;
                    check = checkPersonID(personID, persons);
                }
                AccountData aData = accountData(accounts);
                addAccount(&accounts, aData, personID);
                break;
            case 5:
                if (accounts == NULL) {
                    cout << "Aktualnie w bazie nie ma kont, dlatego funkcja wp³aty i wyp³aty pieniêdzy jest zablokowana.";
                    break;
                }
                cout << "Chcesz wyp³aciæ czy wp³aciæ pieni¹dze ? (wp³ata - 1, wyp³ata - 2): ";
                cin >> depositOrWithdraw;
                while (depositOrWithdraw != 1 && depositOrWithdraw != 2) {
                    cout << "Poda³eœ z³¹ opcjê, wybiersz jeszcze raz: ";
                    cin >> depositOrWithdraw;
                }
                cout << "WprowadŸ numer swojeg konta: ";
                cin >> accountNumber;
                check = checkAccountNumber(accountNumber, accounts);
                while (!check) {
                    cout << "Nie ma konta o wprowadzony numerze. Podaj numer konta jeszcze raz: ";
                    cin >> accountNumber;
                    check = checkAccountNumber(accountNumber, accounts);
                }
                if (depositOrWithdraw == 1) {
                    cout << "Podaj iloœæ pieniêdzy do wp³aty: ";
                }
                else if (depositOrWithdraw == 2) {
                    cout << "Podaj iloœæ pieniêdzy do wyp³aty: ";
                }
                cin >> amountOfMoney;
                depositWithdrawMoney(accounts, amountOfMoney, accountNumber, depositOrWithdraw);
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
            breakKey = getchar();
            if (cin.peek() == '\n')
                break;
        }
        system("cls");
    }
    
    savePersonsToFile(persons);
    saveAccountsToFile(accounts);

    return 0;
}