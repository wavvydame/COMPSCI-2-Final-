/*
Project id: Final Group Project
      Project Name: Bank Management Simulator
      Programmers and Testers: Jared Phillips, Damian Rosales, Mateo Castillo
      Date: Nov 2, 2024
      Version: 2.0
      Description: This program simulates a Bank's system by allowing you to create and delete bank accounts within your bank. Days and weeks are simulated
      and each account randomly spends money on their accounts. Creditscores and debt is simulated.
*/https://onlinegdb.com/6Eu_KjbiC (GDB LINK to use if VS does not load)

#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

//Account member variables are stored in a struct for simplicity
//By having all these variables declared in a struct, I can dynamically assign them easily using a constructor
struct AccountData {
    string name, accountID;
    int creditScore;
    double checkingBalance, savingBalance, creditBalance, amountOwed;

    //The aforementioned constructor that will do our assigning magics
    AccountData(string name, string accountID, int creditScore, double checkingBalance, double savingBalance, double creditBalance, double amountOwed = 0.0)
        : name(name), accountID(accountID), creditScore(creditScore), checkingBalance(checkingBalance), savingBalance(savingBalance), creditBalance(creditBalance), amountOwed(amountOwed) {}
};


//Bank accounts and their data is stored in a class. In hindsight I could've just kept accounts as a struct.
class Account {
private:
    AccountData data;//All of our data is stored in our handy dandy struct
public:
    Account(AccountData data) : data(data) {}
    AccountData getAccountData() { return data; }

    double calculateMinimumCreditPayment(double minPaymentPercentage = 0.02) const {
        return data.amountOwed * minPaymentPercentage;
    }

    //Calculates monthly interest on the credit balance (the loans!!)
    void calculateInterest(double annualInterestRate = 0.20) {//Our interest rate is defined in this method's parameters
        double monthlyInterestRate = annualInterestRate / 12;
        double interestAmount = data.amountOwed * monthlyInterestRate;
        data.amountOwed += interestAmount;
    }

    //Simulates a customer spending on their account either with their checking balance or credit balance
    void spendRandomAmount() {
        double spendingAmount = (rand() % 100) + 1;//Random integer between 1 and 100
        bool spentFromChecking = rand() % 2 == 0;

        if (spentFromChecking) {
            if (data.checkingBalance >= spendingAmount) {
                data.checkingBalance -= spendingAmount;
                cout << data.name << " spent " << spendingAmount << " from checking balance.\n\n";
            }
            else {
                cout << data.name << " tried to spend " << spendingAmount << " from checking, but not enough balance.\n";
            }
        }
        else {
            //If the account spends on their credit balance, the amount owed for their monthly payment goes up
            data.amountOwed += spendingAmount;
            cout << data.name << " spent " << spendingAmount << " on credit. Total owed now: " << data.amountOwed << "\n\n";
        }

        //Call the credit score method which is defined below
        increaseCreditScore();
    }
    //Credit score is increased based on account's spending
    void increaseCreditScore() {
        if (data.amountOwed < 1000 && data.creditScore < 850) {
            data.creditScore += 5;
            if (data.creditScore > 850) {
                data.creditScore = 850;
            }
        }
    }
};


//Bank class. It stores the accounts in a vector of account objects. This class contains the methods responsible for
//Creating accounts, viewing accounts and terminating (deleting) accounts.
class Bank {
private:
    string name;
    static int nextAccountID;//This is the first time I've (Jared) used a static variable. Static creates a global variable I can use throughout the whole code
    //In this case scenario, I need the accountIDs to be automatically assigned each time an account is created.
    vector<Account> accounts;
public:
    Bank(string name) { this->name = name; }
    vector<Account> getAccounts() { return accounts; }
    void addAccount(Account account) { accounts.push_back(account); }

    //Creates an accountID. Account IDs are stored as strings to avoid using a long integer datatype, but I wanna add the integer 1 to each ID that's created.
    //So we use to_string() on the global integer "nextAccountID" to convert the +1 accountID to a string
    static string generateAccountID() {
        return to_string(nextAccountID++);
    }

    //Visually displays every account in the accounts vector and all their information.
    void displayAccounts() {
        cout << name << " bank has " << accounts.size() << " accounts:\n" << endl;
        for (int i = 0; i < accounts.size(); i++) {
            cout << "Account name: " << accounts[i].getAccountData().name <<
                "\nAccount ID: " << accounts[i].getAccountData().accountID <<
                "\nCredit Score: " << accounts[i].getAccountData().creditScore <<
                "\nChecking Balance: " << accounts[i].getAccountData().checkingBalance <<
                "\nSavings Balance: " << accounts[i].getAccountData().savingBalance <<
                "\nCredit Balance: " << accounts[i].getAccountData().creditBalance <<
                "\nAmount Owed: " << accounts[i].getAccountData().amountOwed << "\n" << endl;
        }
    }

    //This method is pretty simple you just enter all the parameters for making an account
    void createAccount() {
        string name, accountID;
        int creditScore;
        double checkingBalance, savingBalance, creditBalance;
        cout << "\nEnter the following info for adding a new account: Name, credit score, checking balance, saving balance, credit balance.\n" << endl;
        cin >> name >> creditScore >> checkingBalance >> savingBalance >> creditBalance;
        accountID = generateAccountID();//The account ID is automatically assigned.
        AccountData data = { name, accountID, creditScore, checkingBalance, savingBalance, creditBalance, 0.0 };
        addAccount(Account(data));
    }

    //Delete an account by entering their account ID.
    void deleteAccount() {
        string accountID;
        cout << "\nEnter the account ID of the account you wish to terminate\nIf you do not know the account ID, use the display all\n accounts option in the main menu" << endl;
        cin >> accountID;
        //Auto is used to automatically see get the data type. In this case when making the variable i = to accounts.begin(), the auto will determine that the
        //datatype of the i variable is the and object of the Account class. In starts from accounts.begin() and ends at account.end();
        for (auto i = accounts.begin(); i != accounts.end(); ++i) {
            if (i->getAccountData().accountID == accountID) {
                accounts.erase(i);
                cout << "Account with ID: " << accountID << " has been successfully terminated." << endl;
                return;
            }
        }
        cout << "No account found with ID " << accountID << "." << endl;
    }

    //The menu the bank administrator (YOU) will see. You enter the corresponding numbers that are next to the option you wish to use.
    void bankMenu() {
        string bankName = name;
        int option;
        bool running = true;
        //Capitalize the bank name used in the title of the options menu
        transform(bankName.begin(), bankName.end(), bankName.begin(), [](unsigned char c) {return toupper(c); });
        while (running) {
            cout << "----------- " << bankName << " BANK -----------" << endl;
            cout << "1. Display all accounts." << endl;
            cout << "2. Add a new account." << endl;
            cout << "3. Terminate an account." << endl;
            cout << "10. Exit." << endl;
            cout << "------------------------------------------------" << endl;
            cin >> option;
            switch (option) {//Switch statement used for our options
            case 1:
                displayAccounts();
                break;
            case 2:
                createAccount();
                break;
            case 3:
                deleteAccount();
                break;
            case 10:
                running = false;
                break;
            default:
                cout << "\nINVALID RESPONSE: Please type in a corresponding number to the option you wish to select.\n" << endl;
                break;
            }
        }
    }

    //A method which simulates random daily spending
    void simulateDay() {
        for (auto& account : accounts) {
            account.spendRandomAmount();//Each day each account will spend a random
            account.calculateInterest();
        }
    }
};

//The global variable from our Bank class being defined here within the scope of the Bank class. Refer to the comment above in the bank class for more info
int Bank::nextAccountID = 10001;

int main() {
    //Random seed to generate random numbers
    srand(time(0));
    Bank chase("Chase");
    string option;
    int days = 0, weeks = 1;
    bool running = true;
    while (running) {
        //Simulate time passing by counting days and weeks
        days++;
        if (days > 7) {
            days = 1;
            weeks++;
        }
        cout << "\nWeek: " << weeks << "\nDay: " << days << "\n" << endl;
        chase.simulateDay();
        chase.bankMenu();
        cout << "\nContinue? Enter NO to end the program" << endl;
        cin >> option;
        //Transform allows us to convert the string option into uppercase. This is used elsewhere in the code above.
        transform(option.begin(), option.end(), option.begin(), [](unsigned char c) { return toupper(c); });
        if (option == "NO") {
            running = false;
        }
        else {
            cout << "You entered: " << option << endl;
        }
    }
    return 0;
}
