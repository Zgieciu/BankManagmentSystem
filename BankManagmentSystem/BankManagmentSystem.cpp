#include <iostream>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <fstream>
#include <ctime>

using namespace std;

// zmienne globalne przechowuj¹ce id u¿ytkowników i kont
int personID = 1, accountID = 1;

// struktura u¿ywana do przechowywania daty 
struct Date {
    int day;
    int month;
    int year;
};

// struktura u¿ywana do przechowywania u¿ytkowników
struct PersonData {
    int personID;
    char firstName[30];
    char lastName[30];
    char pesel[11];
    Date date;
};

// struktura u¿ywana do przechowywania kont u¿ytkowników
struct AccountData {
    int accountID;
    int personID;
    int accountNumber;
    float amountOfMoney;
    Date openedDate;
};

// struktura kolejki dla u¿ytkowników
struct Person {
    PersonData data;
    Person* next;
};

// struktura kolejki dla kont
struct Account {
    AccountData data;
    Account* next;
};

// stworzenie typów wskaŸnikowych dla kolejek
typedef Person* pointer;
typedef Account* pointerA;

// funkcja sprawdzaj¹ca poprawnoœæ daty podanej przez u¿ytkownika
bool checkDate(int day, int month, int year) {
    SYSTEMTIME st;
    GetSystemTime(&st);

    if (year < 1900 || year > st.wYear - 16) {
        cout << "Zosta³ wprowadzony niepoprawny rok. U¿ytkownik musi mieæ conajmniej 16 lat." << endl;
        return false;
    }
    else if (month <= 0 || month > 12) {
        cout << "Zosta³ wprowadzony niepoprawny miesi¹c." << endl;
        return false;
    }

    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (day <= 0 || day > 31) {
                cout << "Zosta³ podany niepoprawny dzieñ" << endl;
                return false;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (day <= 0 || day > 30) {
                cout << "Zosta³ podany niepoprawny dzieñ" << endl;
                return false;
            }
            break;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                if (day <= 0 || day > 29) {
                    cout << "Zosta³ podany niepoprawny dzieñ" << endl;
                    return false;
                }
            }
            else {
                if (day <= 0 || day > 28) {
                    cout << "Zosta³ podany niepoprawny dzieñ" << endl;
                    return false;
                }
            }
            break;
        default:
            break;
    }

    return true;
}

// funkcja sprawdzaj¹ca czy u¿ytkownik o podanym ID istnieje
bool checkPersonID(int id, pointer persons) {
    while (persons != NULL) {
        if (id == persons->data.personID) {
            return true;
        }
        persons = persons->next;  
    }
    return false;
}

// funkcja sprawdzaj¹ca czy konto o podanym numerze istanieje
bool checkAccountNumber(int accountNumber, pointerA accounts) {
    while (accounts != NULL) {
        if (accounts->data.accountNumber == accountNumber) {
            return true;
        }
        accounts = accounts->next;
    }
    return false;
}

// funkcja sprawdzaj¹ca kto jest w³aœcicielem danego konta
string checkOwner(pointerA accounts, pointer persons) {
    if (persons == NULL) return "";
    while (persons->data.personID != accounts->data.personID) {
        persons = persons->next;
    }
    return persons->data.pesel;
}

// funkcja u¿ywana do dodawania danych u¿ytkownika przy tworzeniu nowego u¿ytkownika 
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

// funkcja u¿ywana do dodawania danych konta przy tworzeniu noweg konta
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

// funckja dodaj¹ca uzytkownika do kolejki
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

// funkcja dodaj¹ca konta do kolejki
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

// funkcja wyœwietlaj¹ca u¿ytkowników z kolejki
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

// funkcja wyœwietlaj¹ca konta z kolejki
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

// funkcja zapisuj¹ca u¿ytkowników do pliku
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

// funkcja odczytuj¹ca u¿ytkowników z pliku
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

// funkcja zapisujaca konta do pliku
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

// funkcja odczytuj¹ca konta z pliku
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

// funkcja pozwalaj¹ca wp³adaæ lub wyp³acaæ pieni¹dze z konta 
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
    // ustawienie polskich znaków
    setlocale(LC_CTYPE, "Polish");
    // przypisanie 
    pointer persons = NULL;
    pointerA accounts = NULL;
    int option = 0, breakKey, personID, depositOrWithdraw, accountNumber, amountOfMoney;
    bool check;
    // option - przechwuje opcje wybrane przez u¿ytkownika w menu
    // breakKey - u¿ywane do anulowania dzia³ania czynnoœci lub potwierdzenia klikaj¹c ENTER
    // personID - przechwuje ID osoby podane przez u¿ytkownika
    // depositOrWithdraw - zmienna przechwuj¹ca wartoœci 1 lub 2, sprawdza czy u¿ytkownik chce wp³aciæ czy wyp³aciæ pieni¹dze
    // accountNumber - zmienna przechowuj¹ca podany przez u¿ytkownika numer konta
    // amountOfMoney - zmienna przechowuj¹ca iloœc piêniêdzy 

    // pobieranie danych z plików je¿eli takie istniej¹
    readPersonsFromFile(&persons);
    readAccountsFromFile(&accounts);

    // interfejs dla u¿utkownika - menu
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
                    cout << "Nie ma konta o wprowadzony numerze, chcesz przerwaæ ? (T - przerwaæ, N - kontynuuj)" << endl;
                    do {
                        breakKey = getchar();
                    } while (breakKey != 84 && breakKey != 116 && breakKey != 78 && breakKey != 110);
                    if (breakKey == 84 || breakKey == 116) break;
                    cout << "Podaj numer konta jeszcze raz: ";
                    cin >> accountNumber;
                    check = checkAccountNumber(accountNumber, accounts);
                }
                if (breakKey == 84 || breakKey == 116) break;
                depositOrWithdraw == 1 ? cout << "Podaj iloœæ pieniêdzy do wp³aty: " : cout << "Podaj iloœæ pieniêdzy do wyp³aty: ";
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

        // aby konsola nie czyœci³a siê po wykonaniu operacji i u¿ytkownik móg³ zobaczyæ zmiany program czeka na wciœniêcie ENTER
        if (option == 10) 
            break;
        cout << "\nAby przejœæ dalej naciœnij ENTER";
        while (true) {
            breakKey = getchar();
            if (cin.peek() == '\n')
                break;
        }

        // czyszczenie okna konsoli 
        system("cls");
    }
    
    // zapisywanie danych do plików
    savePersonsToFile(persons);
    saveAccountsToFile(accounts);

    return 0;
}