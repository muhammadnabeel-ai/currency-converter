
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>

using namespace std;

class Logger
{
public:
    static void log(const string &message)
    {
        ofstream file("admin_log.txt", ios::app);
        file << message << endl;
        file.close();
    }
};

class Currency
{
protected:
    string code;
    double rateToUSD;

public:
    Currency()
    {
        code = "";
        rateToUSD = 1;
    }

    Currency(string c, double r)
    {
        code = c;
        rateToUSD = r;
    }

    string getCode() const
    {
        return code;
    }

    double getRate() const
    {
        return rateToUSD;
    }

    void setRate(double r)
    {
        rateToUSD = r;
    }

    virtual double convert(double amount, Currency *target) = 0;
    virtual ~Currency() {}
};

class ForexCurrency : public Currency
{
public:
    ForexCurrency() {}

    ForexCurrency(string c, double r) : Currency(c, r) {}

    double convert(double amount, Currency *target) override
    {
        double inUSD = amount / this->rateToUSD;
        return inUSD * target->getRate();
    }
};

class CurrencySystem
{
private:
    Currency *currencies[100];
    int count = 0;

public:
    CurrencySystem() = default;

    CurrencySystem(const CurrencySystem &) = delete;
    CurrencySystem &operator=(const CurrencySystem &) = delete;

    void loadFile(const string &filename)
    {
        ifstream file(filename);

        if (!file)
        {
            cout << "Error: Cannot open rates file!\n";
            return;
        }

        string code;
        double rate;

        while (file >> code >> rate)
        {
            if (count >= 100)
            {
                cout << "Limit reached!\n";
                break;
            }

            if (rate > 0 && !exists(code))
            {
                currencies[count] = new ForexCurrency(code, rate);
                count++;
            }
        }

        file.close();
    }

    Currency *find(const string &code) const
    {
        for (int i = 0; i < count; i++)
        {
            if (currencies[i]->getCode() == code)
                return currencies[i];
        }
        return nullptr;
    }

    double convert(const string &from, const string &to, double amount)
    {
        Currency *c1 = find(from);
        Currency *c2 = find(to);

        if (!c1 || !c2)
        {
            cout << "Invalid currency!\n";
            return -1;
        }

        double result = c1->convert(amount, c2);

        time_t now = time(0);
        char *dt = ctime(&now);

        string dateTime(dt);
        if (!dateTime.empty())
            dateTime.pop_back();

        ofstream history("conversion_history.txt", ios::app);

        history << dateTime
                << " | " << from << " -> " << to
                << " | Amount: " << amount
                << " | Result: " << result
                << endl;

        history.close();

        return result;
    }

    void showAll()
    {
        cout << "\n      CURRENCIES       \n\n";
        for (int i = 0; i < count; i++)
        {
            cout << currencies[i]->getCode()
                 << " | Rate: " << currencies[i]->getRate() << endl;
        }
    }
    void showCurrencyList()
    {
        ifstream file("currency_list.txt");

        if (!file)
        {
            cout << "Cannot open currency list file!\n";
            return;
        }

        string country, code;

        cout << "\n      AVAILABLE CURRENCIES      \n\n";

        while (file >> country >> code)
        {
            cout << country << " -> " << code << endl;
        }

        file.close();
    }

    bool exists(const string &code) const
    {
        for (int i = 0; i < count; i++)
        {
            if (currencies[i]->getCode() == code)
                return true;
        }
        return false;
    }

    void addCurrency(const string &country, const string &code, double rate)
    {

        if (exists(code) || rate <= 0)
        {
            cout << "Currency already exists or invalid rate!\n";
            return;
        }

        currencies[count] = new ForexCurrency(code, rate);
        count++;

        ofstream rateFile("rates.txt", ios::app);
        rateFile << code << " " << rate << endl;
        rateFile.close();

        ofstream listFile("currency_list.txt", ios::app);
        listFile << country << " " << code << endl;
        listFile.close();

        Logger::log("ADDED: " + country +
                    " (" + code + ") Rate: " +
                    to_string(rate));
    }

    void updateRate(const string &code, double newRate)
    {
        double oldRate = -1;
        bool found = false;

        for (int i = 0; i < count; i++)
        {
            if (currencies[i]->getCode() == code)
            {
                oldRate = currencies[i]->getRate();

                currencies[i]->setRate(newRate);

                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "Currency not found!\n";
            return;
        }

        ifstream inFile("rates.txt");
        ofstream tempFile("temp.txt");

        string c;
        double r;

        while (inFile >> c >> r)
        {
            if (c == code)
                tempFile << c << " " << newRate << endl;
            else
                tempFile << c << " " << r << endl;
        }

        inFile.close();
        tempFile.close();

        remove("rates.txt");
        rename("temp.txt", "rates.txt");

        Logger::log("UPDATED: " + code +
                    " Old: " + to_string(oldRate) +
                    " New: " + to_string(newRate));
    }

    void deleteCurrency(const string &code)
    {
        bool found = false;

        for (int i = 0; i < count; i++)
        {
            if (currencies[i]->getCode() == code)
            {
                delete currencies[i];

                for (int j = i; j < count - 1; j++)
                {
                    currencies[j] = currencies[j + 1];
                }

                count--;
                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "Currency not found!\n";
            return;
        }

        ifstream in1("rates.txt");
        ofstream temp1("temp1.txt");

        string c;
        double r;

        while (in1 >> c >> r)
        {
            if (c != code)
                temp1 << c << " " << r << endl;
        }

        in1.close();
        temp1.close();

        remove("rates.txt");
        rename("temp1.txt", "rates.txt");

        ifstream in2("currency_list.txt");
        ofstream temp2("temp2.txt");

        string country, cc;

        while (in2 >> country >> cc)
        {
            if (cc != code)
                temp2 << country << " " << cc << endl;
        }

        in2.close();
        temp2.close();

        remove("currency_list.txt");
        rename("temp2.txt", "currency_list.txt");

        Logger::log("DELETED: " + code);
    }

    void showHistory()
    {
        ifstream file("conversion_history.txt");

        if (!file)
        {
            cout << "No history file found!\n";
            return;
        }

        cout << "\n      CONVERSION HISTORY \n\n";

        string line;
        int linecount = 0;

        while (getline(file, line))
        {
            cout << line << endl;
            linecount++;
        }

        if (linecount == 0)
        {
            cout << "No conversions yet.\n";
        }

        file.close();
    }

    ~CurrencySystem()
    {
        for (int i = 0; i < count; i++)
        {
            delete currencies[i];
        }
    }
};

int main()
{
    CurrencySystem system;
    system.loadFile("rates.txt");

    int choice;

    do
    {
        cout << "\n=========================\n";
        cout << "   CURRENCY CONVERTER\n";
        cout << "=========================\n";

        cout << "1. Convert Currency\n";
        cout << "2. Show Currency Rates\n";
        cout << "3. Show Currency Codes List\n";
        cout << "4. Admin Panel\n"; // Combined option for cleaner main UX
        cout << "5. History\n";
        cout << "6. Exit\n\n"; // Shifted down because menu options reduced from 7 to 6

        cout << "Please enter your choice: ";
        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n'); // Clear the buffer to prevent string-skipping later

        if (choice == 1)
        {
            string from, to;
            double amount;

            cout << "From (UPPERCASE Code): ";
            cin >> from;

            cout << "To (UPPERCASE Code): ";
            cin >> to;

            cout << "Enter Amount: ";
            cin >> amount;

            double result = system.convert(from, to, amount);

            if (result != -1)
            {
                cout << fixed << setprecision(2)
                     << "Conversion Result: " << result << endl;
            }
        }
        else if (choice == 2)
        {
            system.showAll();
        }
        else if (choice == 3)
        {
            system.showCurrencyList();
        }
        else if (choice == 4)
        {
            // --- ISOLATED ADMIN SUB-MENU ---
            int adminChoice;
            do
            {
                cout << "\n--- ADMIN CONTROL PANEL ---\n";
                cout << "1. Add New Currency\n";
                cout << "2. Update Currency Rate\n";
                cout << "3. Delete Currency\n";
                cout << "4. Return to Main Menu\n\n";

                if (!(cin >> adminChoice))
                {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear();
                    cin.ignore(10000, '\n');
                    continue;
                }
                cin.ignore(10000, '\n'); // Safely consume newline character

                if (adminChoice == 1)
                {
                    string country, code;
                    double rate;

                    cout << "Enter country name: ";
                    getline(cin, country); // Safe now due to earlier cin.ignore() calls

                    cout << "Enter currency code: ";
                    cin >> code;

                    cout << "Enter rate to USD: ";
                    cin >> rate;

                    system.addCurrency(country, code, rate);
                    cout << "Currency added successfully!\n";
                }
                else if (adminChoice == 2)
                {
                    string code;
                    double rate;

                    cout << "Enter currency code: ";
                    cin >> code;

                    cout << "Enter new rate: ";
                    cin >> rate;

                    system.updateRate(code, rate);
                    cout << "Updated successfully!\n";
                }
                else if (adminChoice == 3)
                {
                    string code;

                    cout << "Enter currency code to delete: ";
                    cin >> code;

                    system.deleteCurrency(code);

                    cout << "Delete operation completed.\n";
                }
                else if (adminChoice != 4)
                {
                    cout << "Invalid selection. Choose 1, 2, 3 or 4.\n";
                }

            } while (adminChoice != 4);
        }
        else if (choice == 5)
        {
            system.showHistory();
        }
        else if (choice != 6)
        {
            cout << "Invalid choice. Please select a valid option from the menu.\n";
        }

    } while (choice != 6); // Matches new exit selection index

    cout << "Thanks for using our Currency Convertor!\n";

    return 0;
}