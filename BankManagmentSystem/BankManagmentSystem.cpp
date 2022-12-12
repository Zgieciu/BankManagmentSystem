#include <iostream>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <fstream>
#include <ctime>

using namespace std;

// zmienne globalne przechowuj�ce id u�ytkownik�w, kont i po�yczek
int personID = 1, accountID = 1, loanID = 1;

// struktura u�ywana do przechowywania daty 
struct Date {
    int day;
    int month;
    int year;
};

// struktura u�ywana do przechowywania u�ytkownik�w
struct PersonData {
    int personID;
    char firstName[30];
    char lastName[30];
    char pesel[11];
    Date date;
};

// struktura u�ywana do przechowywania kont u�ytkownik�w
struct AccountData {
    int accountID;
    int personID;
    int accountNumber;
    float amountOfMoney;
    Date openedDate;
};

// struktura u�ywana do przechowywania informacji o po�yczkach
struct LoanData {
    int loanID;
    int personID;
    int loanNumber;
    float amountOfLoan;
    float amountToRepaid;
};

// struktura kolejki dla u�ytkownik�w
struct Person {
    PersonData data;
    Person* next;
};

// struktura kolejki dla kont
struct Account {
    AccountData data;
    Account* next;
};

// struktura kolejki dla po�yczek
struct Loan {
    LoanData data;
    Loan* next;
};

// stworzenie typ�w wska�nikowych dla kolejek
typedef Person* pointer;
typedef Account* pointerA;
typedef Loan* pointerL;

// funkcja sprawdzaj�ca poprawno�� daty podanej przez u�ytkownika
bool checkDate(int day, int month, int year) {
    SYSTEMTIME st;
    GetSystemTime(&st);

    if (year < 1900 || year > st.wYear - 16) {
        cout << "Zosta� wprowadzony niepoprawny rok. U�ytkownik musi mie� conajmniej 16 lat." << endl;
        return false;
    }
    else if (month <= 0 || month > 12) {
        cout << "Zosta� wprowadzony niepoprawny miesi�c." << endl;
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
                cout << "Zosta� podany niepoprawny dzie�" << endl;
                return false;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (day <= 0 || day > 30) {
                cout << "Zosta� podany niepoprawny dzie�" << endl;
                return false;
            }
            break;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                if (day <= 0 || day > 29) {
                    cout << "Zosta� podany niepoprawny dzie�" << endl;
                    return false;
                }
            }
            else {
                if (day <= 0 || day > 28) {
                    cout << "Zosta� podany niepoprawny dzie�" << endl;
                    return false;
                }
            }
            break;
        default:
            break;
    }

    return true;
}

// funkcja sprawdzaj�ca czy u�ytkownik o podanym ID istnieje
bool checkPersonID(int id, pointer persons) {
    while (persons != NULL) {
        if (id == persons->data.personID) {
            return true;
        }
        persons = persons->next;  
    }
    return false;
}

// funkcja sprawdzaj�ca czy konto o podanym numerze istanieje
bool checkAccountNumber(int accountNumber, pointerA accounts) {
    while (accounts != NULL) {
        if (accounts->data.accountNumber == accountNumber) {
            return true;
        }
        accounts = accounts->next;
    }
    return false;
}

//funkcja sprawdzaj�ca czy po�yczka o podanym numerze istnieje
bool checkLoanNumber(int loanNumber, pointerL loans) {
    while (loans != NULL) {
        if (loans->data.loanNumber == loanNumber) {
            return true;
        }
        loans = loans->next;
    }
    return false;
}

// funkcja sprawdzaj�ca kto jest w�a�cicielem danego konta
string checkAccountOwner(pointerA accounts, pointer persons) {
    if (persons == NULL) return "";
    while (persons->data.personID != accounts->data.personID) {
        persons = persons->next;
    }
    return persons->data.pesel;
}

// funkcja sprawdzaj�ca na kogo jest wzi�ta po�yczka
string checkLoanOwner(pointerL loans, pointer persons) {
    if (persons == NULL) return "";
    while (persons->data.personID != loans->data.personID) {
        persons = persons->next;
    }
    return persons->data.pesel;
}

// funkcja u�ywana do dodawania danych u�ytkownika przy tworzeniu nowego u�ytkownika 
PersonData personData() {
    PersonData data;
    cout << "Podaj imi�: ";
    cin >> data.firstName;
    cout << "Podaj nazwisko: ";
    cin >> data.lastName;
    do {
        cout << "Podaj dat� urodzenia dd/mm/yyyy: ";
        cin >> data.date.day >> data.date.month >> data.date.year;
    } while (!checkDate(data.date.day, data.date.month, data.date.year));
    do {
        cout << "Podaj pesel: ";
        cin >> data.pesel;
    } while (strlen(data.pesel) != 11);
    return data;
}

// funkcja u�ywana do dodawania danych konta przy tworzeniu noweg konta
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
    cout << "Podaj ilo�c pieni�dzy na start: ";
    cin >> data.amountOfMoney;
    return data;
}

// funkcja u�ywana do dodawania danych po�yczki przy tworzeniu nowej po�yczki
LoanData loanData(pointerL loans) {
    LoanData data;
    bool check = true;
    int random;
    cout << "Podaj wysoko�� po�yczki: ";
    cin >> data.amountOfLoan;
    data.amountToRepaid = data.amountOfLoan;
    while (check) {
        random = rand() % 8999 + 1000;
        data.loanNumber = random;
        check = false;
        while (loans != NULL) {
            if (loans->data.loanNumber == data.loanNumber) check = true;
            loans = loans->next;
        }
    }
    return data;
}

// funckja dodaj�ca uzytkownika do kolejki
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

// funkcja dodaj�ca konta do kolejki
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

// funkcja dodaj�ca po�yczki do kolejki
void addLoan(pointerL* loans, LoanData data, int ID) {
    if ((*loans) == NULL) {
        pointerL newLoan;
        newLoan = (Loan*)malloc(sizeof(Loan));
        newLoan->data = data;
        newLoan->data.loanID = loanID;
        newLoan->data.personID = ID;
        newLoan->next = NULL;
        (*loans) = newLoan;
        loanID++;
    }
    else
        addLoan(&(*loans)->next, data, ID);
}

// funkcja wy�wietlaj�ca u�ytkownik�w z kolejki
void showPersons(pointer persons) {
    if (persons == NULL) {
        cout << "Aktualnie w bazie nie ma u�ytkownik�w.";
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

// funkcja wy�wietlaj�ca konta z kolejki
void showAccounts(pointerA accounts, pointer persons) {
    if (accounts == NULL) {
        cout << "Aktualnie w bazie nie ma kont.";
    }
    while (accounts != NULL) {
        cout << accounts->data.accountID << ". Nr konta: " << accounts->data.accountNumber;
        cout << " Fundusze: " << accounts->data.amountOfMoney;
        cout << "  W�a�ciciel: " << checkAccountOwner(accounts, persons) << endl;
        accounts = accounts->next;
    }
}

// funkcja wy�wietlaj�ca po�yczki z kolejki
void showLoans(pointerL loans, pointer persons) {
    if (loans == NULL) {
        cout << "Aktualnie w bazie nie ma po�yczek.";
    }
    while (loans != NULL) {
        cout << loans->data.loanID << ". Nr po�yczki: " << loans->data.loanNumber;
        cout << " Wysoko�� po�yczki: " << loans->data.amountOfLoan << " Do sp�aty: " << loans->data.amountToRepaid;
        cout << " W�a�ciciel: " << checkLoanOwner(loans, persons);
        loans->data.amountToRepaid > 0 ?  cout << "" : cout << " (SP�ACONA)";
        cout << endl;
        loans = loans->next;
    }
}

// funkcja zapisuj�ca u�ytkownik�w do pliku
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

// funkcja odczytuj�ca u�ytkownik�w z pliku
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

// funkcja odczytuj�ca konta z pliku
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

// funkcja zapisujaca po�yczki do pliku
void saveLoansToFile(pointerL loans) {
    FILE* file;
    int size = sizeof(LoanData);
    file = fopen("loans.dat", "wb");
    if (file == NULL)
        return;
    while (loans != NULL) {
        fwrite(&loans->data, size, 1, file);
        loans = loans->next;
    }
    fclose(file);
}

// funkcja odczutuj�ca po�yczki z pliku
void readLoansFromFile(pointerL* loans) {
    FILE* file;
    int size = sizeof(LoanData);
    LoanData data;
    file = fopen("loans.dat", "rb");
    if (file == NULL)
        return;
    while (fread(&data, size, 1, file) == 1)
        addLoan(&(*loans), data, data.personID);
    fclose(file);
}

// funkcja pozwalaj�ca wp�ada� lub wyp�aca� pieni�dze z konta 
void depositWithdrawMoney(pointerA accounts, int money, int accountNumber, int type) {
    while (accounts->data.accountNumber != accountNumber) {
        accounts = accounts->next;
    }
    if (type == 1) {
        accounts->data.amountOfMoney += money;
    }
    else if (type == 2) {
        if (money > accounts->data.amountOfMoney) {
            cout << "Brak wystarczj�cych �rodk�w na koncie.";
            return;
        }
        accounts->data.amountOfMoney -= money;
    }
    cout << "Stan konta po wykonaniu transakcji: " << accounts->data.amountOfMoney;
}

// funkcja u�ywana do sp�acania po�yczki
void payBackLoan(pointerL loans, int money, int loanNumber) {
    while (loans->data.loanNumber != loanNumber) {
        loans = loans->next;
    }
    while (money > loans->data.amountToRepaid) {
        cout << "Chcesz sp�aci� wi�ksz� kwot� ni� zosta�a do sp�acenia, podaj kwot� jeszcze raz (do sp�acenia: " << loans->data.amountToRepaid << "z�):";
        cin >> money;
    }
    loans->data.amountToRepaid -= money;
    if (loans->data.amountToRepaid == 0) {
        cout << "Po�yczka zosta�a sp�acona w ca�o�ci.";
        return;
    }
    cout << "Do sp�aty zosta�o: " << loans->data.amountToRepaid << "z�, po�yczka by�a wzi�ta na: " << loans->data.amountOfLoan << "z�";
}

// funkcja czekaj�ca a� u�ytkownik naci�nie ENTER
void waitForEnter() {
    int breakKey;
    cout << "\nAby przej�� dalej naci�nij ENTER";
    while (true) {
        breakKey = getchar();
        if (cin.peek() == '\n')
            break;
    }
}

int main()
{
    // ustawienie polskich znak�w
    setlocale(LC_CTYPE, "Polish");
    // przypisanie do pierwszych element�w kolejki NULL
    pointer persons = NULL;
    pointerA accounts = NULL;
    pointerL loans = NULL;
    int option = 0, breakKey, personID, depositOrWithdraw, accountNumber, loanNumber;
    float amountOfMoney;
    bool check; // zmienna u�ywana do sprawdzania warunk�w
    // option - przechwuje opcje wybrane przez u�ytkownika w menu
    // breakKey - u�ywane do anulowania dzia�ania czynno�ci lub potwierdzenia klikaj�c ENTER
    // personID - przechwuje ID osoby podane przez u�ytkownika
    // depositOrWithdraw - zmienna przechwuj�ca warto�ci 1 lub 2, sprawdza czy u�ytkownik chce wp�aci� czy wyp�aci� pieni�dze
    // accountNumber - zmienna przechowuj�ca podany przez u�ytkownika numer konta
    // amountOfMoney - zmienna przechowuj�ca ilo�c pi�ni�dzy 
    // loanNumber - zmienna przechowuj�ca podany przez u�ytkownika numer po�yczki

    int loginOption = 0;
    string login, password; // zmienne u�ywane do logowania

    // pobieranie danych z plik�w je�eli takie istniej�
    readPersonsFromFile(&persons);
    readAccountsFromFile(&accounts);
    readLoansFromFile(&loans);

    // interfejs do logowania
    while (loginOption != 2) {
        cout << "SYSTEM ZARZ�DZANIA BANKIEM" << endl;
        cout << "\n1.Zaloguj si�";
        cout << "\n2.Zako�cz program\n" << endl;
        cout << "Wybierz opcj�: ";
        cin >> loginOption;
        
        switch (loginOption) {
            case 1:
                cout << "Podaj login: ";
                cin >> login;
                cout << "Podaj has�o: ";
                cin >> password;
                if (true) {
                    cout << "Logowanie przebieg�o pomy�lnie." << endl;
                    waitForEnter();
                    system("cls");
                    // interfejs dla u�utkownika - menu
                    while (option != 10) {
                        cout << "SYSTEM ZARZ�DZANIA BANKIEM" << endl;
                        cout << "\n1.Dodaj u�ytkownika";
                        cout << "\n2.Wy�wietl u�ytkownik�w";
                        cout << "\n3.Wy�wietl konta";
                        cout << "\n4.Wy�wietl po�yczki";
                        cout << "\n5.Utw�rz konto";
                        cout << "\n6.Wp�a� / Wyp�a� pieni�dze";
                        cout << "\n7.Udziel po�yczki";
                        cout << "\n8.Sp�a� po�yczk�";
                        cout << "\n10.Wyloguj si�\n" << endl;

                        cout << "Wybierz opcje: ";
                        cin >> option;
                        cout << endl;

                        switch (option) {
                        case 1: // dodawanie u�ytkownik�w
                            PersonData pData = personData();
                            addPerson(&persons, pData);
                            break;
                        case 2: // wy�wietlanie u�ytkownik�w
                            showPersons(persons);
                            break;
                        case 3: // wy�wietlanie kont
                            showAccounts(accounts, persons);
                            break;
                        case 4: // wy�wietl po�yczki
                            showLoans(loans, persons);
                            break;
                        case 5: // tworzenie kont
                            if (persons == NULL) {
                                cout << "Nie mo�na doda� konta poniewa� w bazie nie ma jeszcze u�ytkownik�w.";
                                break;
                            }
                            showPersons(persons);
                            cout << "\nWybierz po ID u�ytnownika kt�remu chcesz utworzy� konto: ";
                            cin >> personID;
                            check = checkPersonID(personID, persons);
                            while (!check) {
                                cout << "Nie ma u�yktownika o podanym ID. Podaj ID jeszcze raz: ";
                                cin >> personID;
                                check = checkPersonID(personID, persons);
                            }
                            AccountData aData = accountData(accounts);
                            addAccount(&accounts, aData, personID);
                            break;
                        case 6: // wp�acanie / wyp�acanie pieni�dzy z kont
                            if (accounts == NULL) {
                                cout << "Aktualnie w bazie nie ma kont, dlatego funkcja wp�aty i wyp�aty pieni�dzy jest zablokowana.";
                                break;
                            }
                            cout << "Chcesz wyp�aci� czy wp�aci� pieni�dze ? (wp�ata - 1, wyp�ata - 2): ";
                            cin >> depositOrWithdraw;
                            while (depositOrWithdraw != 1 && depositOrWithdraw != 2) {
                                cout << "Poda�e� z�� opcj�, wybiersz jeszcze raz: ";
                                cin >> depositOrWithdraw;
                            }
                            cout << "Wprowad� numer konta: ";
                            cin >> accountNumber;
                            check = checkAccountNumber(accountNumber, accounts);
                            while (!check) {
                                cout << "Nie ma konta o wprowadzonmy numerze, chcesz przerwa� ? (T - przerwa�, N - kontynuuj)" << endl;
                                do {
                                    breakKey = getchar();
                                } while (breakKey != 84 && breakKey != 116 && breakKey != 78 && breakKey != 110);
                                if (breakKey == 84 || breakKey == 116) break;
                                cout << "Podaj numer konta jeszcze raz: ";
                                cin >> accountNumber;
                                check = checkAccountNumber(accountNumber, accounts);
                            }
                            if (breakKey == 84 || breakKey == 116) break;
                            depositOrWithdraw == 1 ? cout << "Podaj ilo�� pieni�dzy do wp�aty: " : cout << "Podaj ilo�� pieni�dzy do wyp�aty: ";
                            cin >> amountOfMoney;
                            depositWithdrawMoney(accounts, amountOfMoney, accountNumber, depositOrWithdraw);
                            break;
                        case 7: // udzielanie po�yczek
                            if (persons == NULL) {
                                cout << "Nie mo�na doda� po�yczki poniewa� w bazie nie ma jeszcze u�ytkownik�w.";
                            }
                            showPersons(persons);
                            cout << "\nPodaj ID u�ytkownika kt�remu chcesz udzieli� po�yczki: ";
                            cin >> personID;
                            check = checkPersonID(personID, persons);
                            while (!check) {
                                cout << "Nie ma u�yktownika o podanym ID. Podaj ID jeszcze raz: ";
                                cin >> personID;
                                check = checkPersonID(personID, persons);
                            }
                            LoanData lData = loanData(loans);
                            addLoan(&loans, lData, personID);
                            break;
                        case 8: // sp�acanie po�yczek
                            if (loans == NULL) {
                                cout << "Akutalnie w bazie nie ma po�yczek, dlatego funkcja wp�aty jest zablokowana.";
                                break;
                            }
                            cout << "Wprowad� numer po�yczki: ";
                            cin >> loanNumber;
                            check = checkLoanNumber(loanNumber, loans);
                            while (!check) {
                                cout << "Nie ma po�yczki o wprowadzonym numerze, chcesz przerwa� ? (T - przerwa�, N - kontynuuj)" << endl;
                                do {
                                    breakKey = getchar();
                                } while (breakKey != 84 && breakKey != 116 && breakKey != 78 && breakKey != 110);
                                if (breakKey == 84 || breakKey == 116) break;
                                cout << "Podaj numer po�yczki jeszcze raz: ";
                                cin >> loanNumber;
                                check = checkLoanNumber(loanNumber, loans);
                            }
                            cout << "Podaj ilo�� pieni�dzy do sp�aty: ";
                            cin >> amountOfMoney;
                            payBackLoan(loans, amountOfMoney, loanNumber);
                            break;
                        case 10: // wylogowanie u�ytkownika
                            cout << "Nast�pi�o wylogowanie.";
                            break;
                        default:
                            cout << "Nie ma opcji o takim numerze";
                            break;
                        }
                        // program czeka na ENTER aby u�ytkownik m�g� zobaczy� wynik na konsoli przed jej wyczyszczeniem
                        waitForEnter();
                        system("cls");
                    }
                }
                break;
            case 2: 
                cout << "Dzi�kujemy za u�ycie naszego programu.";
                break;
            default: 
                cout << "Zosta�a wybrana niepoprawna opcja!";
                break;
        }
    }
    
    // zapisywanie danych do plik�w
    savePersonsToFile(persons);
    saveAccountsToFile(accounts);
    saveLoansToFile(loans);

    return 0;
}