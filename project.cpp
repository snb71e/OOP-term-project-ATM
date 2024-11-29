#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <exception>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

class Account;
class Bank;
class ATM;
class Interface;
class card;
class singleATM;
class multiATM;
class record;

int num_of_ATM = 0;
int num_of_banks = 0;

std::vector<Bank*> bank_list;
std::vector<ATM*> atm_list;
void display_history(bool exit, const vector<record*>& transaction_records, const Interface& ui);
int getIntegerInput(const string& prompt, Interface* ui);
string globalinput_string(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui);
double globalinput_double(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui);
int globalinput_int(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui);

class Account {
private:
    string account_number;
    string password;
    string owner_name;
    string card_number;
    long long available_fund = 0;
    Bank* account_bank;
    Bank* accountNumber;

public:
    Account(string account_num, string owner, string card_num, double init_balance, Bank* bank, string pwd) : account_number(account_num), owner_name(owner), card_number(card_num), available_fund(init_balance), account_bank(bank), password(pwd) {
        account_number = account_num;
        owner_name = owner;
        card_num = card_number;
        available_fund = init_balance;
        account_bank = bank;
        password = pwd;
    }
    ~Account() {}
    double increase_account_balance(double amount) { return available_fund += amount; }
    double decrease_account_balance(double amount) { return available_fund -= amount; }
    string getAccountNumber() { return account_number; }
    string getOwnerName() { return owner_name; };
    double getAvailableFund() { return available_fund; }
    string getCardNumber() { return card_number; }
    string getPassword() { return password; }
    string getBankName();
};

class Bank {
private:
    string bank_name;
    string bank_number;
    string history[100];
    int num_of_transactions = 0;
    Account* accounts[100];
    int num_of_accounts = 0;
    Interface* ui;

public:
    Bank(string bankname, string banknumber, Interface* interface)
        : bank_name(bankname), bank_number(banknumber), ui(interface) {
        num_of_banks += 1;
        for (int i = 0; i < 100; ++i) {
            accounts[i] = nullptr;
        }
    }
    ~Bank() {
        for (int i = 0; i < num_of_accounts; ++i) {
            delete accounts[i];
        }
    }
    bool isPositive(int input);
    string getBankName() const { return bank_name; }
    string getBankNumber() const { return bank_number; }
    Account* createAccount(const string& owner, double balance, const string& password);
    bool deleteAccount(const string& account_number);
    void listAccounts() const;
    Account* make_account(string new_owner_name, string bankname, long long int initial_balance, string password);
    Account* getAccount(int index) const;
    int getNumOfAccounts() const;
    Account* access_to_account(int i);
    Account* access_to_account_by_num(string num);
    int calculateFee(string transaction_type, string cardBank, string target_bank);
    bool hasAccount(string accountnumber);
    void increase_receiver_balance(string receiver_account_number, double amount);
    bool show_authorize(string account_num, string input_password);
};

class Interface {
private:
    bool isEng;
    void printSeparator(int width = 70) const {
        std::cout << std::string(width, '=') << std::endl;
    }
    void printCentered(const std::string& text, int width = 70) const {
        int padding = (width - static_cast<int>(text.length())) / 2;
        std::cout << std::string(padding, ' ') << text << std::endl;

    }
    std::string createBox(const std::string& text, int boxWidth = 30, int boxHeight = 5) const {
        std::ostringstream boxStream;
        boxStream << "=" << std::string(boxWidth - 2, '=') << "=" << std::endl;
        int emptyLines = (boxHeight - 1) / 2;
        for (int i = 0; i < emptyLines; ++i) {
            boxStream << "=" << std::string(boxWidth - 2, ' ') << "=" << std::endl;
        }
        int textPadding = (boxWidth - 2 - static_cast<int>(text.length())) / 2;
        boxStream << "=" << std::string(textPadding, ' ') << text
            << std::string(boxWidth - 2 - textPadding - text.length(), ' ') << "=" << std::endl;
        for (int i = 0; i < emptyLines; ++i) {
            boxStream << "=" << std::string(boxWidth - 2, ' ') << "=" << std::endl;
        }
        boxStream << "=" << std::string(boxWidth - 2, '=') << "=" << std::endl;
        return boxStream.str();
    }

    void printTwoBoxes(const std::string& leftText, const std::string& rightText, int boxWidth = 30, int totalWidth = 70) const {
        std::string leftBox = createBox(leftText, boxWidth);
        std::string rightBox = createBox(rightText, boxWidth);

        std::istringstream leftStream(leftBox), rightStream(rightBox);
        std::string leftLine, rightLine;

        while (std::getline(leftStream, leftLine) && std::getline(rightStream, rightLine)) {
            int padding = (totalWidth - (boxWidth * 2)) / 3;
            std::cout << std::string(padding, ' ') << leftLine
                << std::string(padding, ' ') << rightLine << std::endl;
        }
    }
    void printFourBoxes(const std::vector<std::string>& options, int boxWidth = 30, int totalWidth = 70) const {
        std::vector<std::string> filledOptions = options;
        while (filledOptions.size() < 4) {
            filledOptions.push_back("");
        }
        std::string leftBox = createBox(filledOptions[0], boxWidth);
        std::string rightBox = createBox(filledOptions[1], boxWidth);
        std::istringstream leftStream(leftBox), rightStream(rightBox);
        std::string leftLine, rightLine;
        while (std::getline(leftStream, leftLine) && std::getline(rightStream, rightLine)) {
            int padding = (totalWidth - (boxWidth * 2)) / 3;
            std::cout << std::string(padding, ' ') << leftLine
                << std::string(padding, ' ') << rightLine << std::endl;
        }
        leftBox = createBox(filledOptions[2], boxWidth);
        rightBox = createBox(filledOptions[3], boxWidth);
        leftStream = std::istringstream(leftBox);
        rightStream = std::istringstream(rightBox);
        while (std::getline(leftStream, leftLine) && std::getline(rightStream, rightLine)) {
            int padding = (totalWidth - (boxWidth * 2)) / 3;
            std::cout << std::string(padding, ' ') << leftLine
                << std::string(padding, ' ') << rightLine << std::endl;
        }
    }

    void printMenu(const std::string& title, const std::vector<std::string>& options) const {
        clearScreen();
        printSeparator();
        printCentered(title, 70);
        printSeparator();
        printFourBoxes(options, 30, 70);
        printSeparator();
    }

public:
    Interface(bool lang = true) : isEng(lang) {}
    void clearScreen() const {

#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[H\033[J";
#endif
    }
    void showTransitionMessage(const std::string& message) const {
        clearScreen();
        printSeparator();
        printCentered(message, 70);
        printSeparator();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void setLanguage(bool lang) {
        isEng = lang;
    }

    bool getLanguage() const {
        return isEng;
    }

    void showStartScreen() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Welcome to the System" : "시스템에 오신 것을 환영합니다", 70);
        printSeparator();
        printTwoBoxes(isEng ? "1. Bank Management" : "1. 은행 관리", isEng ? "2. ATM Management" : "2. ATM 관리");
        printTwoBoxes(isEng ? "3. Language Settings" : "3. 언어 설정", isEng ? "4. Insert Card" : "4. 카드 삽입");
        printTwoBoxes(isEng ? "5. Transaction History" : "5. 거래 내역", isEng ? "6. Exit" : "6. 종료");
        printSeparator();
    }
    void showBankManagementMenu() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Bank Management" : "은행 관리", 70);
        printSeparator();
        printTwoBoxes(isEng ? "1. Add Bank" : "1. 은행 추가", isEng ? "2. Remove Bank" : "2. 은행 삭제");
        printTwoBoxes(isEng ? "3. Create Account" : "3. 계좌 생성", isEng ? "4. Delete Account" : "4. 계좌 삭제");
        printTwoBoxes(isEng ? "5. View Accounts" : "5. 계좌 목록", isEng ? "6. Return to Main Menu" : "6. 메인 메뉴");
        printSeparator();
    }

    void showAMTManagementMenu() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "ATM Management" : "ATM 관리", 70);
        printSeparator();
        printTwoBoxes(isEng ? "1. Add ATM" : "1. ATM 추가", isEng ? "2. Remove ATM" : "2. ATM 삭제");
        printTwoBoxes(isEng ? "3. View ATM" : "3. ATM 목록", isEng ? "4. Return to Main Menu" : "4. 메인 메뉴");
        printSeparator();
    }

    void showLanguageSettingsMenu() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Language Settings" : "언어 설정", 70);
        printSeparator();
        printTwoBoxes(isEng ? "1. English" : "1. 영어", isEng ? "2. Korean" : "2. 한국어");
        printSeparator();
        cout << (isEng ? "Please select your preferred language: " : "원하시는 언어를 선택해 주세요: ");
    }

    void showDepositMenu() const {
        printMenu(isEng ? "Deposit Menu" : "입금 메뉴", { isEng ? "1. Cash Deposit" : "1. 현금 입금" ,isEng ? "2. Check Deposit" : "2. 수표 입금",isEng ? "3. Exit" : "3. 거래 취소" });
    }

    void showTransferCashUI(int transferAmount) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Insert cash." : "현금을 투입하세요.", 70);
        printSeparator();
        printCentered((isEng ? "Transfer amount: " : "송금 금액: ") + std::to_string(transferAmount) + (isEng ? " won." : "원"), 70);
        printSeparator();
    }

    void showTransferSuccessUI(int accountBalance) const {
        printSeparator();
        printCentered(isEng ? "Transfer Successful!" : "송금 성공!", 70);
        printSeparator();
        printCentered((isEng ? "Current Balance: " : "현재 계좌 잔액: ") + std::to_string(accountBalance) + (isEng ? " won." : "원"), 70);
        printSeparator();
    }

    void showErrorUI(const std::string& message) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Error" : "오류", 70);
        printSeparator();
        printCentered(message, 70);
        printSeparator();
    }

    void showTransferOptions() const {
        printMenu(isEng ? "Transfer Menu" : "송금 메뉴", { isEng ? "1. Cash Transfer" : "1. 현금 송금", isEng ? "2. Account Transfer" : "2. 계좌 송금", isEng ? "3. Cancel" : "3. 취소" });
    }

    void showErrorMessage() const {
        printMenu(isEng ? "Error" : "오류", { isEng ? "Invalid input." : "잘못된 입력입니다.", isEng ? "Try again." : "다시 시도해주세요." });
    }

    void transactionCancelled() const {
        printMenu(isEng ? "Cancelled" : "취소됨", { isEng ? "Transaction" : "거래가", isEng ? "cancelled." : "취소되었습니다" });
    }

    void showDepositAmount(int amount) const {
        printMenu(isEng ? "Depost completed" : "입금 완료", { (isEng ? "Depost amount" : "총 입금 금액: "), std::to_string(amount) + (isEng ? "won" : "원") });
    }

    void displayBalance(int balance) const {
        printMenu(isEng ? "Balance Inquiry" : "잔액 조회", { (isEng ? "Current Balance: " : "현재 잔액: ") + std::to_string(balance) + (isEng ? " won." : "원") });
    }

    void promptForCheckDeposit() const {
        printMenu(isEng ? "Check Deposit" : "수표 입금", { isEng ? "Insert check." : "수표 삽입" ,isEng ? "0 to Exit" : "취소 : 0" });
    }

    void requestFeePayment(int fee) const {
        printMenu(isEng ? "Fee Process" : "수수료 안내", { std::to_string(fee) + (isEng ? "won" : "원"), isEng ? "Pay to proceed." : "진행하려면 납부" });
    }

    void showUserMenu() const {
        printMenu(isEng ? "User Menu" : "사용자 메뉴", { isEng ? "1. Deposit" : "1. 입금", isEng ? "2. Withdraw" : "2. 출금", isEng ? "3. Transfer" : "3. 송금", isEng ? "4. Exit" : "4. 종료" });
    }

    void showAdminMenu() const {
        printMenu(isEng ? "Admin Menu" : "관리자 메뉴", { isEng ? "1. View Transaction History" : "1. 거래 내역 보기", isEng ? "2. Exit" : "2. 종료" });
    }
};

class record {
private:
    string transactionID;
    string cardNumber;
    string transactionType;
    int amount;
    string atm_bank;
    string deposit_type;
    string SN;
    string owner;
    string owner_bank;
    string owner_account;
    string receiver_bank;
    string receiver_account;
    string transfer_type;

public:
    record(const string& parameter1, const string& parameter2,
        const string& parameter3, const int& parameter4,
        const string& parameter5, const string& parameter6,
        const string& parameter7, const string& parameter8,
        const string& parameter9, const string& parameter10 = "") {
        transactionID = parameter1;
        cardNumber = parameter2;
        transactionType = parameter3;
        amount = parameter4;
        if (parameter3 == "Deposit") {
            deposit_type = parameter5;
            atm_bank = parameter6;
            SN = parameter7;
            owner = parameter8;
            owner_bank = parameter9;
            owner_account = parameter10;
        }
        else if (parameter3 == "Withdraw") {
            atm_bank = parameter5;
            SN = parameter6;
            owner = parameter7;
            owner_bank = parameter8;
            owner_account = parameter9;
        }
        else if (parameter3 == "Cash Transfer") {
            atm_bank = parameter5;
            SN = parameter6;
            receiver_bank = parameter7;
            receiver_account = parameter8;
            transfer_type = parameter9;
        }
        else if (parameter3 == "Account Transfer") {
            atm_bank = parameter5;
            SN = parameter6;
            owner = parameter7;
            owner_bank = parameter8;
            owner_account = parameter9;
            transfer_type = parameter10;
        }
        else {
            throw invalid_argument("Invalid transaction type");
        }
    }
    string getTransactionID() { return transactionID; }
    string getCardNumber() { return cardNumber; }
    string getTransactionType() { return transactionType; }
    int getAmount() { return amount; }
    string getAtmBank() { return atm_bank; }
    string getOwner() { return owner; }
    string write_one_transaction(const Interface* ui, const std::string& transaction_type) const {
        stringstream output;
        output << (ui->getLanguage() ? "Transaction ID: " : "거래 ID: ") << transactionID << "\n";
        output << (ui->getLanguage() ? "Card Number: " : "카드 번호: ") << cardNumber << "\n";
        output << (ui->getLanguage() ? "Transaction Type: " : "거래 유형: ") << transactionType << "\n";
        output << (ui->getLanguage() ? "Amount: " : "금액: ") << amount << "\n";
        if (transaction_type == "Deposit") {
            output << (ui->getLanguage() ? "Deposit Type: " : "입금 유형: ") << deposit_type << "\n";
            output << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << "\n";
            output << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << "\n";
            output << (ui->getLanguage() ? "Owner Name: " : "계좌주명: ") << owner << "\n";
            output << (ui->getLanguage() ? "Owner Bank: " : "계좌 은행: ") << owner_bank << "\n";
            output << (ui->getLanguage() ? "Owner Account: " : "계좌 번호: ") << owner_account << "\n";
        }
        else if (transaction_type == "Withdraw") {
            output << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << "\n";
            output << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << "\n";
            output << (ui->getLanguage() ? "Owner Name: " : "계좌주명: ") << owner << "\n";
            output << (ui->getLanguage() ? "Owner Bank: " : "계좌 은행: ") << owner_bank << "\n";
            output << (ui->getLanguage() ? "Owner Account: " : "계좌 번호: ") << owner_account << "\n";
        }
        else if (transaction_type == "Cash Transfer") {
            output << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << "\n";
            output << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << "\n";
            output << (ui->getLanguage() ? "Receiver Bank: " : "받는 계좌 은행: ") << receiver_bank << "\n";
            output << (ui->getLanguage() ? "Receiver Account: " : "받는 계좌 번호: ") << receiver_account << "\n";
            output << (ui->getLanguage() ? "Transfer Type: " : "이체 유형: ") << transfer_type << "\n";
        }
        else if (transaction_type == "Account Transfer") {
            output << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << "\n";
            output << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << "\n";
            output << (ui->getLanguage() ? "Owner Name: " : "보내는 계좌주명: ") << owner << "\n";
            output << (ui->getLanguage() ? "Owner Bank: " : "보내는 계좌 은행: ") << owner_bank << "\n";
            output << (ui->getLanguage() ? "Owner Account: " : "보내는 계좌 번호: ") << owner_account << "\n";
            output << (ui->getLanguage() ? "Transfer Type: " : "이체 유형: ") << transfer_type << "\n";
        }
        else {
            output << (ui->getLanguage() ? "Unknown Transaction Type" : "알 수 없는 거래 유형") << "\n";
        }
        output << "-------------------------------------------\n";
        return output.str();
    }
    // 주혁 2455 재헌 2300 규원 2500 선애 2000
    void display_one_transaction(const Interface* ui, string transaction_type) const {
        cout << (ui->getLanguage() ? "Transaction ID: " : "거래 ID: ") << transactionID << endl;
        cout << (ui->getLanguage() ? "Card Number: " : "카드 번호: ") << cardNumber << endl;
        cout << (ui->getLanguage() ? "Transaction Type: " : "거래 유형: ") << transactionType << endl;
        cout << (ui->getLanguage() ? "Amount: " : "금액: ") << amount << endl;
        if (transaction_type == "Deposit") {
            cout << (ui->getLanguage() ? "Deposit Type: " : "입금 유형: ") << deposit_type << endl;
            cout << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << endl;
            cout << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << endl;
            cout << (ui->getLanguage() ? "Owner Name: " : "계좌주명: ") << owner << endl;
            cout << (ui->getLanguage() ? "Owner Bank: " : "계좌 은행: ") << owner_bank << endl;
            cout << (ui->getLanguage() ? "Owner Account: " : "계좌 번호: ") << owner_account << endl;
        }
        else if (transaction_type == "Withdraw") {
            cout << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << endl;
            cout << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << endl;
            cout << (ui->getLanguage() ? "Owner Name: " : "계좌주명: ") << owner << endl;
            cout << (ui->getLanguage() ? "Owner Bank: " : "계좌 은행: ") << owner_bank << endl;
            cout << (ui->getLanguage() ? "Owner Account: " : "계좌 번호: ") << owner_account << endl;
        }
        else if (transaction_type == "Cash Transfer") {
            cout << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << endl;
            cout << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << endl;
            cout << (ui->getLanguage() ? "Receiver Bank: " : "받는 계좌 은행: ") << receiver_bank << endl;
            cout << (ui->getLanguage() ? "Receiver Account: " : "받는 계좌 번호: ") << receiver_account << endl;
            cout << (ui->getLanguage() ? "Transfer Type: " : "이체 유형: ") << transfer_type << endl;
        }
        else if (transaction_type == "Account Transfer") {
            cout << (ui->getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_bank << endl;
            cout << (ui->getLanguage() ? "ATM Serial Number: " : "ATM SN 번호: ") << SN << endl;
            cout << (ui->getLanguage() ? "Owner Name: " : "보내는 계좌주명: ") << owner << endl;
            cout << (ui->getLanguage() ? "Owner Bank: " : "보내는 계좌 은행: ") << owner_bank << endl;
            cout << (ui->getLanguage() ? "Owner Account: " : "보내는 계좌 번호: ") << owner_account << endl;
            cout << (ui->getLanguage() ? "Transfer Type: " : "이체 유형: ") << transfer_type << endl;
        }
        else {
            cout << (ui->getLanguage() ? "Unknown Transaction Type" : "알 수 없는 거래 유형") << endl;
        }
        cout << "-------------------------------------------" << endl;
    }
};
class Card {
public:
    string cardNumber;
    string password;
    bool isAdmin;
    Card() : cardNumber(""), password(""), isAdmin(false) {}
    Card(string number, string pwd, bool admin = false)
        : cardNumber(number), password(pwd), isAdmin(admin) {}
};

class ATM {
private:
    string atmBank;
    string atmBankName;
    Account* account;
    Bank* bank;
    string atmNumber;
    string atmID;
    Interface* ui;
    int cash[4]{ 0, 0, 0, 0 };
    int transaction_order = 0;
    int num_of_transaction = 0;
    vector<record*> transaction_records;
    map<string, string> userPasswords;
    map<string, Card> cards;
    map<string, string> userInfo;
    int loginAttempts = 0;
    Bank* primaryBank;
    bool isSingleBankMode;
    bool isBilingual;
    Interface* atmInterface;

public:
    ATM(Bank* atmBank, bool issingle, bool isBilingual, int arr[4], const string& atmId, Interface* uiinterface)
        : bank(atmBank), isSingleBankMode(issingle), isBilingual(isBilingual), atmID(atmId), ui(uiinterface) {
        for (int i = 0; i < 4; ++i) {
            cash[i] = arr[i];
        }
        if (bank) {
            atmBankName = bank->getBankName();
        }
        atmInterface = new Interface(ui->getLanguage());
    }
    string getatmNumber() { return atmNumber; }
    string getremainingcash(int i) { return to_string(cash[i]); }
    void transactionHistory(bool exit) const {
        display_history(exit, transaction_records, *ui);
    }
    void singleBankMode(bool SingleBankMode);
    void addCard(const string& cardNumber, const string& password, const string& userID, bool isAdmin);
    string getBankNumber();
    string getatmID() { return atmID; }
    string getatmbank() const { return atmBankName; }
    bool issinglemode() { return isSingleBankMode; }
    int cashinatm() const { return cash[0] * 1000 + cash[1] * 5000 + cash[2] * 10000 + cash[3] * 50000; }
    void adminMenu();
    void userMenu(ATM* selectedATM);
    bool payFee(int fee);
    bool isInteger(const std::string& input);
    bool isPositive(int input);
    bool hasSufficientCash(int amount);
    void dispenseCash(int amount);
    void addCashToATM(int m1, int m2, int m3, int m4, int fee = 0);
    int getValidInput(const string& prompt, Interface* ui);
    bool depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4, int& totalBills);
    bool deposit();
    bool fee_cash_calculator(int fee);
    bool change_ATM_dec(int change);
    bool fee_account_calculator(int fee);
    bool withdraw();
    bool transfer();
    void processTransaction(int depositAmount, const string& cardBank);
    string transactionid();
    void transaction_recording(const string& parameter1, const string& parameter2,
        const string& parameter3, const int& parameter4,
        const string& parameter5, const string& parameter6,
        const string& parameter7, const string& parameter8,
        const string& parameter9, const string& parameter10 = "");
    void setAccount(Account* acc) {
        account = acc;
    }
    const vector<record*>& getTransactionRecords() const {
        return transaction_records;
    }
    bool getIsBilingual() const { return isBilingual; }  
    string getLanMode() {
        if (isBilingual) { return "Bilingual";}
        else {return "Unilingual";}
        }
    Interface* getAtmInterface() { return atmInterface; }
    void setAtmLanguage(bool isEng) {
        atmInterface->setLanguage(isEng);
    }
};

class singleATM : public ATM {
public:
    singleATM(Bank* bank, bool issingle, bool isBilingual, int arr[4], const string& id, Interface* ui)
        : ATM(bank, issingle, isBilingual, arr, id, ui) {
        num_of_ATM += 1;
    }
};

class multiATM : public ATM {
public:
    multiATM(Bank* bank, bool issingle, bool isBilingual, int arr[4], const string& id, Interface* ui)
        : ATM(bank, issingle, isBilingual, arr, id, ui) {
        num_of_ATM += 1;
    }
};

void display_atm(vector<ATM*> atm_list, Interface ui) {
    if (atm_list.empty()) {
        cout << (ui.getLanguage() ? "**********There are no ATMs created**********" : "**********생성된 ATM이 없습니다**********") << endl;
    }
    else {
        cout << "==============================ATM==============================\n";
        for (int i = 0; i < atm_list.size(); i++) {
            cout << "ATM [SN:" << atm_list[i]->getatmID() << "]\nremaining cash : {KRW 50000 : " << atm_list[i]->getremainingcash(3) << "KRW 10000 : " << atm_list[i]->getremainingcash(2) << ", KRW 5000 : " << atm_list[i]->getremainingcash(1)
                << ", KRW 1000 : " << atm_list[i]->getremainingcash(0) << "} " << endl;
            cout << "---------------------------------------------------------------\n";
        }
        cout << "" << endl;
    }

}

void display_account(vector<Bank*> bank_list, Interface ui) {
    if (bank_list.empty()) {
        cout << (ui.getLanguage() ? "**********There are no Banks created**********"
            : "**********생성된 은행이 없습니다.**********") << endl;
    }
    else {
        bool account_exist = false;
        for (int i = 0; i < bank_list.size(); i++) {
            if (bank_list[i]->getNumOfAccounts() != 0) {
                account_exist = true;
            }
        }
        if (!account_exist) {
            cout << (ui.getLanguage() ? "**********There are no Accounts created**********"
                : "**********생성된 계좌가 없습니다**********") << endl;
        }
        else {
            cout << (ui.getLanguage() ? "============================ACCOUNT============================\n"
                : "============================계좌 정보============================\n");
            for (int i = 0; i < bank_list.size(); i++) {
                for (int j = 0; j < bank_list[i]->getNumOfAccounts(); j++) {
                    cout << (ui.getLanguage() ? "Account [Bank:" : "계좌 [은행:")
                        << bank_list[i]->getBankName()
                        << (ui.getLanguage() ? ", No : " : ", 계좌 번호: ")
                        << bank_list[i]->access_to_account(j)->getAccountNumber()
                        << (ui.getLanguage() ? ", Owner : " : ", 소유자: ")
                        << bank_list[i]->access_to_account(j)->getOwnerName()
                        << "]\n";

                    cout << (ui.getLanguage() ? " balance: " : " 잔액: ")
                        << std::fixed << std::setprecision(0)
                        << bank_list[i]->access_to_account(j)->getAvailableFund()
                        << (ui.getLanguage() ? " won" : " 원") << endl;

                    cout << (ui.getLanguage() ? "---------------------------------------------------------------\n"
                        : "---------------------------------------------------------------\n");
                }
            }
        }
    }
}

bool Bank::isPositive(int input) {
    if (input < 0) {
        cout << (ui->getLanguage() ? "Invalid Number\nTry Again" : "올바른 숫자가 아닙니다.\n다시 시도해 주세요.") << endl;
        return false;
    }
    return true;
}

bool Bank::deleteAccount(const string& accountNumber) {
    for (int i = 0; i < 100; ++i) {
        if (accounts[i] != nullptr && accounts[i]->getAccountNumber() == accountNumber) {
            delete accounts[i];
            accounts[i] = nullptr;
            return true;
        }
    }
    return false;
}

void Bank::listAccounts() const {
    if (num_of_accounts == 0) {
        cout << this->getBankName() << (ui->getLanguage() ? ": No registered accounts." : ": 등록된 계좌가 없습니다.") << endl;
        return;
    }
    cout << (ui->getLanguage() ? "Bank " : "은행 ") << bank_name
        << (ui->getLanguage() ? "'s account list" : "의 계좌 목록:") << endl;
    for (int i = 0; i < num_of_accounts; ++i) {
        Account* account = accounts[i];
        cout << std::fixed << std::setprecision(0);
        cout << "======================================================================" << endl;
        cout << (ui->getLanguage() ? "Account information.\n" : "계좌 정보.\n");
        cout << (ui->getLanguage() ? "- Owner: " : "- 소유자: ") << account->getOwnerName() << "\n";
        cout << (ui->getLanguage() ? "- Bank Name: " : " - 은행 명: ") << bank_name << "\n";
        cout << (ui->getLanguage() ? "- Account Number: " : " - 계좌 번호: ") << account->getAccountNumber() << "\n";
        cout << (ui->getLanguage() ? "- Card Number: " : " - 카드 번호: ") << account->getCardNumber() << "\n";
        cout << (ui->getLanguage() ? "- Balance: " : " - 잔액: ") << account->getAvailableFund() << (ui->getLanguage() ? "won\n" : "원\n");
        cout << "======================================================================" << endl;
    }

}

Account* Bank::make_account(string new_owner_name, string bankname, long long int initial_balance, string password) {
    if (num_of_accounts >= 100) {
        cout << "계좌 생성 실패: 최대 계좌 수를 초과했습니다.\n";
        return nullptr;
    }
    string account_number = bank_number + to_string(num_of_accounts + 1);
    string card_number = bank_number;
    for (int i = 0; i < 8; ++i) {
        card_number += to_string(rand() % 10);
    }
    Account* new_account = new Account(account_number, new_owner_name, card_number, initial_balance, this, password);
    accounts[num_of_accounts++] = new_account;
    cout << (ui->getLanguage() ? "Account is initialized.\n" : "계좌가 생성되었습니다.\n");
    cout << "======================================================================" << endl;
    cout << (ui->getLanguage() ? "Account information.\n" : "계좌 정보.\n");
    cout << (ui->getLanguage() ? "- Owner: " : "- 소유자: ") << new_owner_name << "\n";
    cout << (ui->getLanguage() ? "- Bank Name: " : " - 은행 명: ") << bankname << "\n";
    cout << (ui->getLanguage() ? "- Account Number: " : " - 계좌 번호: ") << account_number << "\n";
    cout << (ui->getLanguage() ? "- Card Number: " : " - 카드 번호: ") << card_number << "\n";
    cout << (ui->getLanguage() ? "- Initial Balance: " : " - 초기 잔액: ") << initial_balance << (ui->getLanguage() ? "won\n" : "원\n");
    cout << "======================================================================" << endl;
    return new_account;
}

Account* Bank::getAccount(int index) const {
    if (index >= 0 && index < num_of_accounts) {
        return accounts[index];
    }
    return nullptr;
}

int Bank::getNumOfAccounts() const {
    return num_of_accounts;
}
Account* Bank::access_to_account(int i) { return accounts[i]; }
Account* Bank::access_to_account_by_num(string num) {
    for (int i = 0; i < this->getNumOfAccounts(); i++) {
        if (num == accounts[i]->getAccountNumber()) {
            return accounts[i];
        }
    }
    return nullptr;
}

int Bank::calculateFee(string transaction_type, string cardBank = "", string target_bank = "") {
    if (transaction_type == "transfer") {
        if (!cardBank.empty() && !target_bank.empty()) {
            if (cardBank == bank_number && target_bank == bank_number) {
                return 2000;
            }
            else if (cardBank != bank_number && target_bank != bank_number) {
                return 4000;
            }
            else if (cardBank == bank_number && target_bank != bank_number) {
                return 3000;
            }
        }
        else {
            return 1000;
        }
    }
    else {
        return (cardBank == bank_name) ? 1000 : 2000;
    }
}

bool Bank::hasAccount(string accountnumber) {
    for (int i = 0; i < num_of_accounts; i++) {
        if (accounts[i]->getAccountNumber() == accountnumber) {
            return true;
        }
    }
    return false;
}

void Bank::increase_receiver_balance(string receiver_account_number, double amount) {
    for (int i = 0; i < num_of_accounts; i++) {
        if (accounts[i]->getAccountNumber() == receiver_account_number) {
            accounts[i]->increase_account_balance(amount);
        }
    }

}

bool Bank::show_authorize(string account_num, string input_password) {
    for (int i = 0; i < num_of_accounts; i++) {
        if (accounts[i]->getAccountNumber() == account_num) {
            if (accounts[i]->getPassword() == input_password) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    return false;
}

string Account::getBankName() { return account_bank->getBankName(); }

void ATM::singleBankMode(bool SingleBankMode) {
    SingleBankMode = isSingleBankMode;
}

void ATM::addCard(const string& cardNumber, const string& password, const string& userID, bool isAdmin) {
    cards[cardNumber] = Card(cardNumber, password, isAdmin);
}

string ATM::getBankNumber() {
    return bank->getBankNumber();
}

void ATM::adminMenu() {
    while (true) {
        ui->showAdminMenu();
        string selection;
        cin >> selection;
        if (selection == "1") {
            cout << (ui->getLanguage() ? "Viewing transaction history..." : "거래 내역 조회...") << endl;
            display_history("admin", transaction_records, ui);
        }
        else if (selection == "2") {
            cout << (ui->getLanguage() ? "Exiting admin menu. Returning to main menu.\n" : "관리자 메뉴를 종료합니다. 메인 메뉴로 돌아갑니다.\n");
            break;
        }
        else {
            cout << (ui->getLanguage() ? "Invalid option. Try again.\n" : "잘못된 선택입니다. 다시 시도해 주세요.\n");
        }
    }
}

void ATM::userMenu(ATM* selectedATM) {
    if (ui == nullptr) {
        cout << "Error: UI interface is not initialized.\n";
        return;
    }

    // Bilingual 여부에 따라 언어 선택
    if (isBilingual) {
        int languageSelection = 0;
        cout << "Select Language:\n1. English\n2. 한국어\n";
        while (true) {
            cin >> languageSelection;
            if (languageSelection == 1) {
                ui->setLanguage(true); // 영어로 설정
                break;
            }
            else if (languageSelection == 2) {
                ui->setLanguage(false); // 한국어로 설정
                break;
            }
            else {
                cout << "Invalid option. Please select again: ";
            }
        }
    }

    cout << "Entering userMenu..." << endl;
    while (true) {
        ui->showUserMenu();
        cout << (ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: ");
        string selection;
        cin >> selection;

        if (selection == "1") {
            cout << (ui->getLanguage() ? "Selected Deposit menu..." : "입금 메뉴를 선택했습니다...") << endl;
            if (!deposit()) {
                cout << (ui->getLanguage() ? "Deposit failed. Returning to user menu..." : "입금 실패. 사용자 메뉴로 돌아갑니다...") << endl;
            }
        }
        else if (selection == "2") {
            cout << (ui->getLanguage() ? "Selected Withdraw menu..." : "출금 메뉴를 선택했습니다...") << endl;
            if (!withdraw()) {
                cout << (ui->getLanguage() ? "Withdraw failed. Returning to user menu..." : "출금 실패. 사용자 메뉴로 돌아갑니다...") << endl;
            }
        }
        else if (selection == "3") {
            cout << (ui->getLanguage() ? "Selected Transfer menu..." : "송금 메뉴를 선택했습니다...") << endl;
            if (!transfer()) {
                cout << (ui->getLanguage() ? "Transfer failed. Returning to user menu..." : "송금 실패. 사용자 메뉴로 돌아갑니다...") << endl;
            }
        }
        else if (selection == "4") {
            cout << (ui->getLanguage() ? "Exiting ATM. Please take your card.\n" : "ATM을 종료합니다. 카드를 가져가세요.\n");
            break;
        }
        else {
            cout << (ui->getLanguage() ? "Invalid option. Please try again.\n" : "잘못된 입력입니다. 다시 시도해 주세요.\n");
        }
    }
}

bool ATM::isInteger(const std::string& input) {
    if (input.empty()) {
        cout << (ui->getLanguage() ? "Please enter a valid numeric value." : "올바른 숫자 값을 입력해 주세요.") << endl;
        return false;
    }
    size_t start = (input[0] == '-') ? 1 : 0;
    for (size_t i = start; i < input.size(); ++i) {
        if (!isdigit(input[i])) {
            cout << (ui->getLanguage() ? "Please enter a valid numeric value." : "올바른 숫자 값을 입력해 주세요.") << endl;
            return false;
        }
    }
    return true;
}

bool ATM::isPositive(int input) {
    if (input < 0) {
        cout << (ui->getLanguage() ? "Negative numbers cannot be entered.\nPlease try again." : "음수는 입력할 수 없습니다.\n다시 시도해 주세요.") << endl;
        return false;
    }
    return true;
}

bool ATM::hasSufficientCash(int amount) {
    int totalCash = cash[0] * 1000 + cash[1] * 5000 + cash[2] * 10000 + cash[3] * 50000;
    return totalCash >= amount;
}

void ATM::dispenseCash(int amount) {
    int denominations[4] = { 50000, 10000, 5000, 1000 };
    int numNotes[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++) {
        numNotes[i] = min(amount / denominations[i], cash[3 - i]);
        amount -= numNotes[i] * denominations[i];
        cash[3 - i] -= numNotes[i];
    }
    cout << (ui->getLanguage() ? "Dispensed cash as follows:\n" : "다음과 같이 현금이 지급되었습니다:\n");
    for (int i = 0; i < 4; i++) {
        cout << denominations[i] << (ui->getLanguage() ? " won: " : "원: ") << numNotes[i] << endl;
    }
    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
    cin.ignore();
    cin.get();
}

void ATM::addCashToATM(int m1, int m2, int m3, int m4, int fee) {
    int denominations[4] = { 1000, 5000, 10000, 50000 };
    int numNotes[4] = { m1, m2, m3, m4 };
    int totalBills = 0;
    for (int i = 0; i < 4; ++i) {
        cash[i] += numNotes[i];
        totalBills += numNotes[i] * denominations[i];
    }
    if (fee > 0) {
        cash[0] += fee / 1000;
    }
    cout << (ui->getLanguage() ? "Cash added to ATM: " : "ATM에 추가된 현금: ") << totalBills << " won\n";
    cout << (ui->getLanguage() ? "Adding fee to ATM: " : "ATM에 추가된 수수료: ") << fee << " won\n";
}

int ATM::getValidInput(const string& prompt, Interface* ui) {
    int value;
    while (true) {
        cout << prompt;
        value = globalinput_int(atm_list, bank_list, ui);
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << (ui->getLanguage() ? "Wrong Input. Please enter a valid integer." : "잘못된 입력입니다. 유효한 정수를 입력하세요.") << endl;
            continue;
        }
        if (value == -1) {
            return -1;
        }
        if (value >= 0 && value <= numeric_limits<int>::max()) {
            return value;
        }
        else {
            if (ui->getLanguage()) {
                cout << "Invalid input. Please enter a number between 0 and " + to_string(numeric_limits<int>::max()) + "." << endl;
            }
            else {
                cout << "잘못된 입력입니다. 0에서 " + to_string(numeric_limits<int>::max()) + " 사이의 숫자를 입력하세요." << endl;
            }
        }
    }
}

bool ATM::depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4, int& totalBills) {
    totalBills = 0;
    int total = 0;
    cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' for all to cancel):\n"
        : "각 지폐의 개수를 입력하세요 (또는 전부 '0'을 입력해 취소):\n");
    while (total <= 50) {
        cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
        m1 = getValidInput("", ui);
        total += m1;
        if (total > 50) {
            cout << (ui->getLanguage() ? "Too many bills. Maximum total is 50.\n"
                : "현금은 총합 50장을 초과할 수 없습니다.\n");
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
        m2 = getValidInput("", ui);
        total += m2;
        if (total > 50) {
            cout << (ui->getLanguage() ? "Too many bills. Maximum total is 50.\n"
                : "현금은 총합 50장을 초과할 수 없습니다.\n");
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
        m3 = getValidInput("", ui);
        total += m3;
        if (total > 50) {
            cout << (ui->getLanguage() ? "Too many bills. Maximum total is 50.\n"
                : "현금은 총합 50장을 초과할 수 없습니다.\n");
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
        m4 = getValidInput("", ui);
        total += m4;
        if (total > 50) {
            cout << (ui->getLanguage() ? "Too many bills. Maximum total is 50.\n"
                : "현금은 총합 50장을 초과할 수 없습니다.\n");
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        break;
    }
    totalBills = (m1 * 1000) + (m2 * 5000) + (m3 * 10000) + (m4 * 50000);
    if (totalBills == 0) {
        cout << (ui->getLanguage() ? "No money entered. Cancelling...\n" : "입금 금액이 없습니다. 취소 중...\n");
        return false;
    }
    cout << (ui->getLanguage() ? "Total deposit amount: " : "총 입금 금액: ") << totalBills << " won\n";
    return true;
}

bool ATM::deposit() {
    cout << "Starting deposit process...\n";
    if (account == nullptr) {
        cout << (ui->getLanguage() ? "Error: Account is not initialized.\n" : "계좌가 개설되어 있지 않았습니다.\n");
        return false;
    }
    string cardBank = account->getBankName();
    string atmBank = getatmbank();
    while (true) {
        ui->showDepositMenu();
        int input = getValidInput((ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: "), ui);
        if (input == 1) {
            int m1, m2, m3, m4, totalBills = 0;
            if (!depositCash(ui, m1, m2, m3, m4, totalBills)) {
                return false;
            }
            int fee = (cardBank == atmBank) ? 1000 : 2000;
            cout << (ui->getLanguage() ? "Please pay the deposit fee: " : "입금 수수료를 납부해주세요: ") << fee << " won\n";
            int feePaid = 0;
            if (!payFee(fee)) {
                return false;
            }
            addCashToATM(m1, m2, m3, m4, feePaid);
            transaction_recording(transactionid(), account->getCardNumber(), "Deposit", totalBills, "(Cash Deposit)", atmBank, getatmNumber(), account->getOwnerName(), account->getBankName(), account->getAccountNumber());
            processTransaction(totalBills, cardBank);
            return true;
        }
        else if (input == 2) {
            int checkAmount;
            while (true) {
                ui->promptForCheckDeposit();
                cout << (ui->getLanguage() ? "Please input check (Minimum : 100,000 won): " : "수표를 입금해주세요 (최소 금액 : 100,000원): ") << endl;
                checkAmount = globalinput_int(atm_list, bank_list, ui);
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << (ui->getLanguage() ? "Wrong Input. Try Again." : "잘못된 입력. 다시 시도.") << endl;
                    continue;
                }
                if (checkAmount == 0) {
                    ui->transactionCancelled();
                    return false;
                }
                if (checkAmount < 100000) {
                    cout << (ui->getLanguage() ? "Check deposit requires at least 100,000. Please try again." : "수표 예금은 최소 100,000원 이상만 가능합니다. 다시 시도해주세요.") << endl;
                }
                else {
                    break;
                }
            }
            int fee = (cardBank == atmBank) ? 1000 : 2000;
            cout << (ui->getLanguage() ? "Please pay the deposit fee: " : "입금 수수료를 납부해주세요: ") << fee << " won" << endl;
            int feePaid = 0;
            if (!payFee(fee)) {
                cout << (ui->getLanguage() ? "Fee payment failed. Cancelling transaction.\n" : "수수료 납부 실패. 거래를 취소합니다.\n");
                return false;
            }
            transaction_recording(transactionid(), account->getCardNumber(), "Deposit", checkAmount, "(Check Deposit)", getatmbank(), getatmNumber(), account->getOwnerName(), account->getBankName(), account->getAccountNumber());
            processTransaction(checkAmount, cardBank);
            return true;
        }
        else if (input == 3) {
            ui->transactionCancelled();
            return false;
        }
        else {
            ui->showErrorMessage();
        }
    }
}

bool ATM::fee_cash_calculator(int fee) {
    int denominations[4] = { 50000, 10000, 5000, 1000 };
    int cashAdded[4] = { 0, 0, 0, 0 };
    int total = 0;
    cout << (ui->getLanguage() ? "Please pay the fee: " : "수수료를 납부해주세요: ") << fee << " won\n";
    for (int i = 3; i >= 0; i--) {
        while (total < fee) {
            cout << denominations[i] << (ui->getLanguage() ? " won bills: " : "원 지폐 수: ");
            int input = globalinput_int(atm_list, bank_list, ui);
            if (input < 0) {
                cout << (ui->getLanguage() ? "Invalid input. Please enter a positive number of bills." : "잘못된 입력입니다. 양수의 지폐 개수를 입력해 주세요.") << endl;
                return false;
            }
            total += input * denominations[i];
            cashAdded[i] += input;
            if (total >= fee) break;
        }
    }
    if (total < fee) {
        cout << (ui->getLanguage() ? "Failure to pay fees: Insufficient cash provided." : "수수료 납부 실패: 현금 부족.") << endl;
        return false;
    }
    if (total > fee) {
        int change = total - fee;
        cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << " won\n";

        if (!change_ATM_dec(change)) {
            cout << (ui->getLanguage() ? "Failed to dispense change due to insufficient ATM cash." : "ATM 현금 부족으로 거스름돈 지급 실패.") << endl;
            return false;
        }
    }
    for (int i = 0; i < 4; i++) {
        cash[i] += cashAdded[i];
    }
    cout << (ui->getLanguage() ? "Fee paid successfully. ATM cash updated." : "수수료 납부 성공. ATM 현금이 업데이트되었습니다.") << endl;
    return true;
}

bool ATM::change_ATM_dec(int change) {
    int num_cash[4]{ 0,0,0,0 };
    int real_cash[4]{ 1000, 5000, 10000, 50000 };
    for (int i = 3; i >= 0; i--) {
        while (change >= real_cash[i]) {
            num_cash[i] += 1;
            change -= real_cash[i];
        }
    }
    for (int j = 0; j < 4; j++) {
        if ((cash[j] - num_cash[j]) >= 0) {
        }
        else {
            cout << (ui->getLanguage() ? "Transaction failed: Insufficient cash in ATM" : "거래 실패: ATM 내 현금 부족");
            ui->transactionCancelled();
            cout << "======================================================================" << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
    }
    for (int k = 0; k < 4; k++) {
        cash[k] -= num_cash[k];
    }
    return true;
}

bool ATM::payFee(int fee) {
    int m1, m2, m3, m4, totalPaid = 0;
    cout << (ui->getLanguage() ? "Enter bills to pay the fee:\n" : "수수료를 납부할 지폐 개수를 입력하세요:\n");
    m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 지폐 개수: "), ui);
    m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 지폐 개수: "), ui);
    m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 지폐 개수: "), ui);
    m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 지폐 개수: "), ui);
    totalPaid = (m1 * 1000) + (m2 * 5000) + (m3 * 10000) + (m4 * 50000);
    if (totalPaid < fee) {
        cout << (ui->getLanguage() ? "Insufficient fee payment. Please try again.\n" : "수수료 납부 금액이 부족합니다. 다시 시도하세요.\n");
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }
    int change = totalPaid - fee;
    if (change > 0) {
        cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << " won\n";
        int previousCash[4];
        for (int i = 0; i < 4; i++) {
            previousCash[i] = cash[i];
        }
        dispenseCash(change);
        int remainingChange = change;
        for (int i = 0; i < 4; i++) {
            int dispensed = previousCash[i] - cash[i];
            remainingChange -= dispensed * (50000 / (1 << i));
        }
        if (remainingChange > 0) {
            cout << (ui->getLanguage() ? "Error: Unable to dispense change. Transaction cancelled.\n"
                : "오류: 거스름돈 지급 실패. 거래를 취소합니다.\n");
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
    }
    addCashToATM(m1, m2, m3, m4);
    cout << (ui->getLanguage() ? "Fee paid successfully.\n" : "수수료 납부가 완료되었습니다.\n");
    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
    cin.ignore();
    cin.get();
    return true;
}

bool ATM::fee_account_calculator(int fee) {
    ui->requestFeePayment(fee);
    cout << (ui->getLanguage() ? "Fee: " : "수수료: ") << fee << endl;
    cout << (ui->getLanguage() ? "Paying a fee from your account..." : "계좌에서 수수료를 지불 중...") << endl;
    double balance = account->getAvailableFund();
    if (balance >= fee) {
        account->decrease_account_balance(fee);
        cout << (ui->getLanguage() ? "Successfully Fee paid!" : "수수료 납부 성공!") << endl;
        cout << "======================================================================" << endl;
        return true;
    }
    else {
        cout << (ui->getLanguage() ? "Failed to pay fee: Insufficient account balance" : "수수료 납부 실패: 계좌 잔액 부족") << endl;
        cout << "======================================================================" << endl;

        return false;
    }
}

bool ATM::withdraw() {
    long long int withdrawAmount;
    if (ui == nullptr || account == nullptr) {
        cout << (ui->getLanguage() ? "Error: Account is not initialized." : "계좌가 개설되어 있지 않습니다.") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }
    string cardBank = account->getAccountNumber().substr(0, 4);
    int withdrawalCount = 0;
    while (withdrawalCount < 3) {
        cout << (ui->getLanguage() ? "\n=== Withdraw Menu ===\nAccount Balance: " : "\n=== 출금 메뉴 ===\n계좌 잔액: ") << account->getAvailableFund() << " won\n";
        cout << (ui->getLanguage() ? "<The Amount to withdraw>" : "<출금할 금액>") << endl;
        int m1 = 0;
        int m2 = 0;
        int m3 = 0;
        int m4 = 0;
        cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' for all to cancel):\n"
            : "각 지폐의 개수를 입력하세요 (또는 전부 '0'을 입력해 취소):\n");
        while (m1 + m2 + m3 + m4 <= 50) {
            m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 개수: "), ui);
            m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 개수: "), ui);
            m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 개수: "), ui);
            m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 개수: "), ui);
            break;
        }
        if (m1 + m2 + m3 + m4 > 50) {
            cout << (ui->getLanguage() ? "Do not insert more than number of 50 bills" : "현금을 50장 이상 넣을 수 없습니다.");
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui->getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
        }
        withdrawAmount = (m1 * 1000) + (m2 * 5000) + (m3 * 10000) + (m4 * 50000);
        if (withdrawAmount == 0) {
            cout << (ui->getLanguage() ? "Returning to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }
        if (withdrawAmount > 500000) {
            cout << (ui->getLanguage() ? "Error: Transaction limit is 500,000 won per withdrawal." : "오류: 거래 한도는 출금 당 500,000원입니다.") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            continue;
        }
        if (withdrawAmount > account->getAvailableFund()) {
            cout << (ui->getLanguage() ? "Error: Insufficient account balance." : "오류: 계좌 잔액 부족.") << endl;
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }
        if (!hasSufficientCash(withdrawAmount)) {
            cout << (ui->getLanguage() ? "Error: ATM has insufficient cash." : "오류: ATM에 충분한 현금이 없습니다.") << endl;
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }
        int fee = (account->getBankName() == bank->getBankName()) ? 1000 : 2000;
        if (account->getAvailableFund() < withdrawAmount + fee) {
            cout << (ui->getLanguage() ? "Error: Insufficient balance to cover the fee." :
                "오류: 수수료를 포함한 잔액이 부족합니다.") << endl;
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }
        cout << (ui->getLanguage() ? "Fee paid successfully. ATM cash updated." :
            "수수료 납부 성공. ATM 현금이 업데이트되었습니다.") << endl;
        account->decrease_account_balance(withdrawAmount + fee);
        dispenseCash(withdrawAmount);
        withdrawalCount++;
        transaction_recording(transactionid(), account->getCardNumber(), "Withdraw", withdrawAmount, getatmbank(), getatmNumber(), account->getOwnerName(), account->getBankName(), account->getAccountNumber());
        cout << (ui->getLanguage() ? "Withdrawal Successful!" : "출금 성공!") << endl;
        cout << (ui->getLanguage() ? "Current Balance: " : "현재 잔액: ") << account->getAvailableFund() << " won\n";
        int continueWithdrawal = globalinput_int(atm_list, bank_list, ui);
        cout << (ui->getLanguage() ? "Would you like to make another withdrawal? (1: Yes, 0: No): " : "추가 인출을 원하시면 1을, 종료하시려면 0을 입력하세요: ");
        if (continueWithdrawal == 0) {
            cout << (ui->getLanguage() ? "Returning to User Menu..." : "사용자 메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        else if (continueWithdrawal != 1) {
            cout << (ui->getLanguage() ? "Error: Invalid input. Ending withdrawal session." : "오류: 잘못된 입력입니다. 출금 세션을 종료합니다.") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        else if (continueWithdrawal == 1) {
            withdrawalCount++;
            continue;
        }
    }
    if (withdrawalCount == 3) {
        cout << (ui->getLanguage() ? "Error: Withdrawal limit exceeded. Returning to User Menu..." : "오류: 최대 인출 횟수를 초과했습니다. 사용자 메뉴로 돌아갑니다...") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }
    return true;
}

bool ATM::transfer() {
    while (true) {
        ui->showTransferOptions();
        int input = globalinput_int(atm_list, bank_list, ui);
        if (input == 1) {
            cout << (ui->getLanguage() ? "Please enter the account to transfer or '0' to cancel." : "송금할 계좌를 입력하거나 '0'을 눌러 취소하세요.") << endl;
            string account_num = globalinput_string(atm_list, bank_list, ui);
            if (account_num == "0") {
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            if (!isInteger(account_num)) {
                ui->showErrorMessage();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            bool validAccount = false;
            for (int i = 0; i < bank_list.size(); i++) {
                for (int j = 0; j < bank_list[i]->getNumOfAccounts(); j++) {
                    if (bank_list[i]->access_to_account(j)->getAccountNumber() == account_num) {
                        validAccount = true;
                        break;
                    }
                }
            }
            if (validAccount) {
                cout << (ui->getLanguage() ? "<The amount of transfer>" : "<송금할 금액>") << endl;
                int m1 = 0;
                int m2 = 0;
                int m3 = 0;
                int m4 = 0;
                long long transferAmount;
                cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' to cancel):\n"
                    : "각 지폐의 개수를 입력하세요 (또는 '0'을 입력해 취소):\n");
                while (m1 + m2 + m3 + m4 <= 50) {
                    m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: "), ui);
                    m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: "), ui);
                    m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: "), ui);
                    m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: "), ui);
                    break;
                }
                if (m1 + m2 + m3 + m4 > 50) {
                    cout << (ui->getLanguage() ? "Do not insert more than number of 50 bills" : "현금을 50장 이상 넣을 수 없습니다.");
                    for (size_t i = 0; i < atm_list.size(); ++i) {
                        cout << (ui->getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                        atm_list[i]->transactionHistory(true);
                        continue;
                    }
                    exit(0);
                }
                if (m1 + m2 + m3 + m4 == 0) {
                    cout << (ui->getLanguage() ? "Cancelling...\n" : "취소 중...\n");
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
                transferAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;
                if (fee_cash_calculator(1000)) {
                    transaction_recording(transactionid(), account->getCardNumber(), "Cash Transfer", transferAmount,
                        getatmbank(), getatmID(), bank->access_to_account_by_num(account_num)->getBankName(),
                        bank->access_to_account_by_num(account_num)->getAccountNumber(), "(Cash transfer)");
                    ui->showTransferSuccessUI(account->getAvailableFund());
                    bank->increase_receiver_balance(account_num, transferAmount);
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return true;
                }
                else {
                    ui->showErrorMessage();
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
            }
            else {
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account." : "거래 실패: 존재하지 않는 계좌.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
        }
        else if (input == 2) {
            cout << (ui->getLanguage() ? "Enter account number to transfer or '0' to cancel." : "송금할 계좌 번호를 입력하거나 '0'을 눌러 취소하세요: ") << endl;
            string account_num = globalinput_string(atm_list, bank_list, ui);
            if (account_num == "0") {
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            bool validAccount = false;
            for (int i = 0; i < bank_list.size(); i++) {
                for (int j = 0; j < bank_list[i]->getNumOfAccounts(); j++) {
                    if (bank_list[i]->access_to_account(j)->getAccountNumber() == account_num) {
                        validAccount = true;
                        break;
                    }
                }
            }
            if (!validAccount) {
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account." : "거래 실패: 존재하지 않는 계좌.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            double transferAmount;
            cout << (ui->getLanguage() ? "Enter amount to transfer or '0' to cancel." : "송금할 금액을 입력하거나 '0'을 눌러 취소하세요.") << endl;
            while (true) {
                transferAmount = globalinput_double(atm_list, bank_list, ui);
                if (transferAmount == 0) {
                    ui->transactionCancelled();
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
                if (isPositive(transferAmount)) break;
                ui->showErrorMessage();
            }
            if (account->getAvailableFund() < transferAmount) {
                cout << (ui->getLanguage() ? "Transaction failed: Insufficient balance." : "거래 실패: 잔액 부족.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            account->decrease_account_balance(transferAmount);
            int real_fee;
            real_fee = bank->calculateFee("transfer", account->getCardNumber().substr(0, 4), account_num.substr(0, 4));
            if (fee_account_calculator(real_fee)) {
                transaction_recording(transactionid(), account->getCardNumber(), "Account Transfer", transferAmount, getatmbank(), getatmID(), account->getOwnerName(), account->getBankName(), account->getAccountNumber(), "(Account transfer)");
                bank->increase_receiver_balance(account_num, transferAmount);
                ui->showTransferSuccessUI(account->getAvailableFund());
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return true;
            }
            else {
                account->increase_account_balance(transferAmount);
                ui->showErrorMessage();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
        }
        else if (input == 3 or input == 0) {
            ui->transactionCancelled();
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        else {
            ui->showErrorMessage();
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
    }
}

void ATM::processTransaction(int depositAmount, const string& cardBank) {
    int fee = bank->calculateFee(cardBank);
    account->increase_account_balance(depositAmount);
}

string ATM::transactionid() {
    return account->getAccountNumber().substr(0, 4) + to_string(++transaction_order);
}

void ATM::transaction_recording(const string& parameter1, const string& parameter2,
    const string& parameter3, const int& parameter4,
    const string& parameter5, const string& parameter6,
    const string& parameter7, const string& parameter8,
    const string& parameter9, const string& parameter10) {
    if (num_of_transaction >= 100) {
        cout << (ui->getLanguage() ? "Transaction history is full. Cannot record more transactions."
            : "거래 기록이 가득 찼습니다. 더 이상 기록할 수 없습니다.") << endl;
        return;
    }
    record* new_record = nullptr;
    if (parameter3 == "Deposit") {
        new_record = new record(parameter1, parameter2,
            parameter3, parameter4,
            parameter5, parameter6,
            parameter7, parameter8,
            parameter9, parameter10);
    }
    else if (parameter3 == "Withdraw") {
        new_record = new record(parameter1, parameter2,
            parameter3, parameter4,
            parameter5, parameter6,
            parameter7, parameter8,
            parameter9);
    }
    else if (parameter3 == "Cash Transfer") {
        new_record = new record(parameter1, parameter2,
            parameter3, parameter4,
            parameter5, parameter6,
            parameter7, parameter8,
            parameter9);
    }
    else if (parameter3 == "Account Transfer") {
        new_record = new record(parameter1, parameter2,
            parameter3, parameter4,
            parameter5, parameter6,
            parameter7, parameter8,
            parameter9, parameter10);
    }
    transaction_records.push_back(new_record);
    cout << (ui->getLanguage() ? "Transaction recorded successfully." : "거래가 성공적으로 기록되었습니다.") << endl;
}

void display_history(bool exit, const vector<record*>& transaction_records, const Interface& ui) {
    if (transaction_records.empty()) {
        ui.showErrorUI(ui.getLanguage() ? "No transaction history available." : "거래 내역이 없습니다.");
        return;
    }
    if (!exit) {
        ui.clearScreen();
        ui.showTransitionMessage(ui.getLanguage() ? "All Transaction History" : "전체 거래 내역");
    }
    
    vector<string> transaction_types = { "Deposit", "Withdraw", "Cash Transfer", "Account Transfer" };
    for (const auto& record : transaction_records) {
        for (const auto& type : transaction_types) {
            if (type == record->getTransactionType()) {
                record->display_one_transaction(const_cast<Interface*>(&ui), type);
            }
        }
    }
    if (exit) {
        cout << (ui.getLanguage() ? "Session End" : "세션 종료") << endl;
        return;
    }
    cout << (ui.getLanguage() ? "Press any key to continue..." : "계속하려면 아무 키나 누르세요...") << endl;
    cin.ignore();
    cin.get();
    ui.showTransitionMessage(ui.getLanguage() ? "Save to file?" : "파일로 저장하시겠습니까?");
    cout << (ui.getLanguage() ? "1. Yes\n2. No" : "1. 예\n2. 아니오") << endl;
    string saveOption;
    cin >> saveOption;
    if (saveOption == "1") {
        ofstream file("AllTransactionHistory.txt");
        if (!file) {
            ui.showErrorUI(ui.getLanguage() ? "Error: Unable to create file." : "오류: 파일을 생성할 수 없습니다.");
            return;
        }
        string write;
        for (const auto& record : transaction_records) {
            for (const auto& type : transaction_types) {
                if (type == record->getTransactionType()) {
                    write = record->write_one_transaction(const_cast<Interface*>(&ui), type);
                    record->display_one_transaction(const_cast<Interface*>(&ui), type);
                }
            }
        }
        file << "************Transaction History************" << endl;
        for (const auto& record : transaction_records) {
            file << write;
        }
        file.close();
        ui.showTransitionMessage(ui.getLanguage() ? "Saved to AllTransactionHistory.txt" : "AllTransactionHistory.txt에 저장되었습니다.");
    }
    else {
        ui.showTransitionMessage(ui.getLanguage() ? "Returning to menu." : "메뉴로 돌아갑니다.");
    }
}

string generateAtmID(const string& bankNumber, int atmCount) {
    ostringstream oss;
    oss << bankNumber << setfill('0') << setw(2) << atmCount + 1;
    return oss.str();
}

string globalinput_string(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui) {
    string input;
    while (true) {
        cin >> input;
        if (input == "-1") {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
        }
        else if (input == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        return input;
    }
}

double globalinput_double(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui) {
    double input;
    while (true) {
        cin >> input;
        if (input == -1) {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
        }
        else if (to_string(input) == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        return input;
    }
}

int globalinput_int(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui) {
    string raw_input;
    int input;
    while (true) {
        cin >> raw_input;
        if (raw_input == "-1") {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
        }
        else if (raw_input == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        else {
            input = stoi(raw_input);
            return input;
        }
    }
}

int getIntegerInput(const string& prompt, Interface* ui) {
    string raw_input;
    int value;
    while (true) {
        cout << prompt;
        cin >> raw_input;
        if (raw_input == "-1") {
            cout << (ui->getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui->getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
        }
        else if (raw_input == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui->getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        else if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid input. Only integers are allowed. Please try again." << endl;
        }
        else {
            int input;
            input = stoi(raw_input);
            return input;
        }
    }
}

int main() {
    Interface ui;
    int current_atm_num;
    ATM* atm;

    while (true) {
        ui.showStartScreen();
        cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;
        string startSelection;
        startSelection = globalinput_string(atm_list, bank_list, ui);
        if (startSelection == "1") {
            ui.showTransitionMessage(ui.getLanguage() ? "Moving to Bank Management Menu..." : "은행 관리 메뉴로 이동 중...");
            while (true) {
                ui.showBankManagementMenu();
                cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;
                string bankSelection;
                bankSelection = globalinput_string(atm_list, bank_list, ui);
                if (bankSelection == "1") {
                    string bankName, bankCode;
                    cout << (ui.getLanguage() ? "Enter the bank name: " : "은행 이름을 입력하세요: ");
                    bankName = globalinput_string(atm_list, bank_list, ui);
                    cout << (ui.getLanguage() ? "Bank code is 4 digits." : "은행 코드는 4자리입니다.") << endl;
                    cout << (ui.getLanguage() ? "Enter the bank code: " : "은행 코드를 입력하세요: ");
                    while (true) {
                        bankCode = globalinput_string(atm_list, bank_list, ui);
                        if (bankCode.length() == 4)
                            break;
                        cout << (ui.getLanguage() ? "Error: Bank code must be 4 digits.\nPlease Try again." : "오류: 은행 코드는 4자리여야 합니다.\n다시 시도해 주세요.") << endl;
                    }
                    bank_list.push_back(new Bank(bankName, bankCode, &ui));
                    cout << (ui.getLanguage() ? "Bank added successfully.\n" : "은행이 성공적으로 추가되었습니다.\n");
                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                }
                else if (bankSelection == "2") {
                    for (int i = 0; i < bank_list.size(); i++) {
                        cout << "======================================================================" << endl;
                        cout << i + 1 << "." << endl;
                        cout << (ui.getLanguage() ? "Bank Name: " : "은행 이름: ") << bank_list[i]->getBankName() << endl;
                        cout << (ui.getLanguage() ? "Bank Code: " : "은행 코드: ") << bank_list[i]->getBankNumber() << endl << endl;
                    }
                    string bankCode;
                    cout << (ui.getLanguage() ? "Enter the bank code to delete: " : "삭제할 은행 코드를 입력하세요: ");
                    bankCode = globalinput_string(atm_list, bank_list, ui);
                    bool found = false;
                    for (auto it = bank_list.begin(); it != bank_list.end(); ++it) {
                        if ((*it)->getBankNumber() == bankCode) {
                            cout << "Deleting bank: " << (*it)->getBankNumber() << endl;
                            delete* it;
                            bank_list.erase(it);
                            found = true;
                            cout << (ui.getLanguage() ? "Bank is deleted successfully." : "은행이 성공적으로 삭제되었습니다.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                            cin.ignore();
                            cin.get();
                            break;
                        }
                    }
                    if (!found) {
                        cout << (ui.getLanguage() ? "Error: Bank not found.\n" : "오류: 해당 은행을 찾을 수 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                    }
                }
                else if (bankSelection == "3") {
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available. Please add a bank first.\n" : "등록된 은행이 없습니다. 먼저 은행을 추가해주세요.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    cout << (ui.getLanguage() ? "Select a bank to create an account:\n" : "계좌를 생성할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }
                    int bankChoice = getIntegerInput(
                        string(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: "),
                        &ui
                    );
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    Bank* selectedBank = bank_list[bankChoice - 1];
                    string ownerName, password;
                    double initialBalance;
                    cout << (ui.getLanguage() ? "Enter the owner name: " : "계좌 소유자의 이름을 입력하세요: ");
                    ownerName = globalinput_string(atm_list, bank_list, ui);
                    initialBalance = getIntegerInput(ui.getLanguage() ? "Enter the initial balance: " : "초기 잔액을 입력하세요: ", &ui);
                    cout << (ui.getLanguage() ? "Enter the password: " : "비밀번호를 입력하세요: ");
                    password = globalinput_string(atm_list, bank_list, ui);
                    selectedBank->make_account(ownerName, selectedBank->getBankName(), initialBalance, password);
                    cout << (ui.getLanguage() ? "Account created successfully.\n" : "계좌가 성공적으로 생성되었습니다.\n") << endl;
                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                }
                else if (bankSelection == "4") {
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available.\n" : "등록된 은행이 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    cout << (ui.getLanguage() ? "Select a bank to delete an account:\n" : "계좌를 삭제할 은행을 선택하세요:\n");
                    for (int i = 0; i < bank_list.size(); i++) {
                        cout << "======================================================================" << endl;
                        cout << i + 1 << ". " << endl;
                        bank_list[i]->listAccounts();
                    }
                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ", &ui);
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    Bank* selectedBank = bank_list[bankChoice - 1];
                    if (!selectedBank) {
                        cout << (ui.getLanguage() ? "Error: Invalid bank selected.\n" : "오류: 잘못된 은행이 선택되었습니다.\n");
                        continue;
                    }
                    cout << (ui.getLanguage() ? "Enter the account number to delete: " : "삭제할 계좌 번호를 입력하세요: ");
                    string accountNumber;
                    cin >> accountNumber;
                    if (selectedBank->deleteAccount(accountNumber)) {
                        cout << (ui.getLanguage() ? "Account removed successfully.\n" : "계좌가 성공적으로 삭제되었습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                    }
                    else {
                        cout << (ui.getLanguage() ? "Error: Account number not found.\n" : "오류: 해당 계좌 번호를 찾을 수 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                    }
                }
                else if (bankSelection == "5") {
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No Accounts available.\n" : "등록된 계좌가 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    ui.clearScreen();
                    for (int i = 0; i < bank_list.size(); i++) {
                        cout << "======================================================================" << endl;
                        cout << i + 1 << ". " << endl;
                        bank_list[i]->listAccounts();
                    }
                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                }
                else if (bankSelection == "6") {
                    ui.showTransitionMessage(ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...");
                    break;
                }
                else {
                    cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }
        else if (startSelection == "2") {
            ui.showTransitionMessage(ui.getLanguage() ? "Moving to ATM Management Menu..." : "ATM 관리 메뉴로 이동 중...");
            while (true) {
                ui.showAMTManagementMenu();
                cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;
                string ATMSelection;
                ATMSelection = globalinput_string(atm_list, bank_list, ui);
                if (ATMSelection == "1") {
                    string bankname;
                    int bank_index;
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No bank to create ATM. " : "ATM을 생성할 은행이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    cout << (ui.getLanguage() ? "Please select a Main Bank" : "주 은행을 입력해 주세요.") << endl;
                    cout << (ui.getLanguage() ? "If you wish to cancle, select '0'." : "취소하고 싶으면 '0'을 눌러 주세요.") << endl;
                    cout << (ui.getLanguage() ? "List of Banks" : "은행 목록") << endl;
                    for (int i = 0; i < bank_list.size(); i++) {
                        cout << i + 1 << ". ";
                        cout << bank_list[i]->getBankName() << endl;
                    }
                    bank_index = globalinput_int(atm_list, bank_list, ui);
                    if (bank_index == 0) {
                        cout << (ui.getLanguage() ? "Creating ATM is canceled." : "ATM 생성이 취소되었습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    bank_index--;
                    if (not (bank_index >= 0) and (bank_index < bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid input value." : "잘못된 입력값입니다.") << endl;
                        cout << (ui.getLanguage() ? "Creating ATM is canceled." : "ATM 생성이 취소되었습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    while (true) {
                        cout << bank_list[bank_index]->getBankName() << (ui.getLanguage() ? " Generating ATM." : " 은행에서 ATM 생성을 시작합니다.") << endl;
                        bool issingle;
                        string singleormulti;
                        cout << (ui.getLanguage() ? "1. Single Bank ATM" : "1. 단일 은행 ATM") << endl;
                        cout << (ui.getLanguage() ? "2. Multi Bank ATM" : "2. 다중 은행 ATM") << endl;
                        singleormulti = globalinput_string(atm_list, bank_list, ui);
                        if (singleormulti == "1") {
                            cout << (ui.getLanguage() ? "Generating Single Bank ATM." : "단일 은행 ATM을 만듭니다.") << endl;
                            issingle = true;
                        }
                        else if (singleormulti == "2") {
                            cout << (ui.getLanguage() ? "Generating Multi Bank ATM." : "다중 은행 ATM을 만듭니다.") << endl;
                            issingle = false;
                        }
                        else {
                            cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                            cin.ignore();
                            cin.get();
                            continue;
                        }
                        int cashes[4]{ 0, 0, 0, 0 };
                        int cash1, cash2, cash3, cash4;
                        cout << (ui.getLanguage() ? "Enter the initial cash amount for the ATM." : "ATM의 초기 금액을 입력해 주세요.") << endl;
                        cout << (ui.getLanguage() ? "Enter the number of bills for each denomination." : "각 단위에 해당하는 지폐의 수를 입력해 주세요.") << endl;
                        cout << (ui.getLanguage() ? "1,000 bills: " : "1,000: ");
                        cash1 = globalinput_int(atm_list, bank_list, ui);
                        cout << endl << (ui.getLanguage() ? "5,000 bills: " : "5,000: ");
                        cash2 = globalinput_int(atm_list, bank_list, ui);
                        cout << endl << (ui.getLanguage() ? "10,000 bills: " : "10,000: ");
                        cash3 = globalinput_int(atm_list, bank_list, ui);
                        cout << endl << (ui.getLanguage() ? "50,000 bills: " : "50,000: ");
                        cash4 = globalinput_int(atm_list, bank_list, ui);
                        cashes[0] = cash1;
                        cashes[1] = cash2;
                        cashes[2] = cash3;
                        cashes[3] = cash4;
                        cout << (ui.getLanguage() ? "Total initial cash in the ATM: " : "ATM의 초기 금액: ")
                            << cashes[0] * 1000 + cashes[1] * 5000 + cashes[2] * 10000 + cashes[3] * 50000 << endl;
                        cout << (ui.getLanguage() ? "Creating ATM..." : "ATM 생성 중 ...") << endl;
                        string atmId = generateAtmID(bank_list[bank_index]->getBankNumber(), atm_list.size());
                        cout << (ui.getLanguage() ? "Choose ATM language mode:\n1. Unilingual\n2. Bilingual\n"
                            : "ATM 언어 모드를 선택하세요:\n1. 단일 언어\n2. 이중 언어\n");
                        int langMode = globalinput_int(atm_list, bank_list, ui);
                        bool isBilingual = (langMode == 2);
                        if (langMode != 1 && langMode != 2) {
                            cout << (ui.getLanguage() ? "Invalid selection. Defaulting to Unilingual mode.\n"
                                : "잘못된 선택입니다. 단일 언어 모드로 설정됩니다.\n");
                            isBilingual = false;
                        }
                        if (issingle) {
                            atm_list.push_back(new singleATM(bank_list[bank_index], issingle, isBilingual, cashes, atmId, &ui));
                            cout << (ui.getLanguage() ? "ATM is initialized successfully." : "ATM이 성공적으로 생성되었습니다.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                            cin.ignore();
                            cin.get();
                            break;
                        }
                        else {
                            atm_list.push_back(new multiATM(bank_list[bank_index], issingle, isBilingual, cashes, atmId, &ui));
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                            cin.ignore();
                            cin.get();
                            break;
                        }
                    }
                    current_atm_num = bank_index;
                }
                else if (ATMSelection == "2") {
                    if (atm_list.empty()) {
                        cout << (ui.getLanguage() ? "No ATM to delete." : "삭제할 ATM이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    while (true) {
                        cout << "======================================================================" << endl;
                        for (int i = 0; i < atm_list.size(); i++) {
                            cout << i + 1 << "." << endl;
                            cout << (ui.getLanguage() ? "ATM ID: " : "ATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                            cout << (ui.getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_list[i]->getatmbank() << endl;
                            cout << (ui.getLanguage() ? "ATM Mode: " : "ATM 모드: ");
                            cout << (ui.getLanguage() ? "Language Mode: " : "언어 모드: ") << atm_list[i]->getLanMode() << endl;
                            if (atm_list[i]->issinglemode()) {
                                cout << (ui.getLanguage() ? "Single" : "단일") << endl;
                            }
                            else {
                                cout << (ui.getLanguage() ? "Multi" : "다중") << endl;
                            }
                            cout << (ui.getLanguage() ? "Cash on ATM: " : "ATM 내 현금: ") << atm_list[i]->cashinatm() << endl;
                        }
                        cout << "======================================================================" << endl;
                        cout << (ui.getLanguage() ? "Enter the number of ATM to delete or press 0 to go back: "
                            : "삭제할 ATM 번호를 입력하거나, 0을 눌러 이전 화면으로 돌아가세요: ") << endl;
                        int num;
                        num = globalinput_int(atm_list, bank_list, ui);
                        if (num == 0) {
                            cout << (ui.getLanguage() ? "Returning to previous menu..." : "이전 화면으로 돌아갑니다...") << endl;
                            break;
                        }
                        if (num > 0 && num <= atm_list.size()) {
                            num--;
                            auto iterator = atm_list.begin() + num;
                            ATM* atm = *iterator;
                            atm_list.erase(iterator);
                            delete atm;
                            cout << (ui.getLanguage() ? "ATM deletion complete." : "ATM이 삭제되었습니다.") << endl;
                            break;
                        }
                        else {
                            cout << (ui.getLanguage() ? "Invalid input. Try again." : "잘못된 입력입니다. 다시 시도하세요.") << endl;
                        }
                    }
                }
                else if (ATMSelection == "3") {
                    if (atm_list.empty()) {
                        cout << (ui.getLanguage() ? "No ATM Exist" : "ATM이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    while (true) {
                        cout << "======================================================================" << endl;
                        for (int i = 0; i < atm_list.size(); i++) {
                            cout << i + 1 << "." << endl;
                            cout << (ui.getLanguage() ? "ATM ID: " : "ATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                            cout << (ui.getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_list[i]->getatmbank() << endl;
                            cout << (ui.getLanguage() ? "ATM Mode: " : "ATM 모드: ");
                            cout << (ui.getLanguage() ? "Language Mode: " : "언어 모드: ") << atm_list[i]->getLanMode() << endl;
                            if (atm_list[i]->issinglemode()) {
                                cout << (ui.getLanguage() ? "Single" : "단일") << endl;
                            }
                            else {
                                cout << (ui.getLanguage() ? "Multi" : "다중") << endl;
                            }
                            cout << (ui.getLanguage() ? "Cash on ATM: " : "ATM 내 현금: ") << atm_list[i]->cashinatm() << endl;
                        }
                        cout << "======================================================================" << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                }
                else if (ATMSelection == "4") {
                    ui.showTransitionMessage(ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...");
                    break;
                }
                else {
                    cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }

        else if (startSelection == "3") {
            while (true) {
                ui.showLanguageSettingsMenu();
                int languageSelection = getIntegerInput(ui.getLanguage() ? "Please select a language: " : "언어를 선택하세요: ", &ui);
                if (languageSelection == 1) {
                    ui.setLanguage(true);
                    ui.clearScreen();
                    cout << "Language changed to English.\n";
                    break;
                }
                else if (languageSelection == 2) {
                    ui.setLanguage(false);
                    ui.clearScreen();
                    cout << "언어가 한국어로 변경되었습니다.\n";
                    break;
                }
                else {
                    cout << (ui.getLanguage() ? "Invalid input. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }
        else if (startSelection == "4") {
            ui.clearScreen();
            if (atm_list.empty()) {
                cout << (ui.getLanguage() ? "Please create an ATM first." : "먼저 ATM을 생성해 주세요.") << endl;
                cout << (ui.getLanguage() ? "Press Enter to return to the previous menu..." : "이전 메뉴로 돌아가려면 엔터 키를 누르세요...") << endl;
                cin.ignore();
                cin.get();
                continue;
            }
            bool exitToMainMenu = false;
            while (true) {
                cout << (ui.getLanguage() ? "Select ATM for transaction." : "거래를 진행할 ATM을 선택해 주세요.") << endl;
                for (int i = 0; i < atm_list.size(); i++) {
                    cout << i + 1 << "." << endl;
                    cout << (ui.getLanguage() ? "ATM ID: " : "ATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                    cout << (ui.getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_list[i]->getatmbank() << endl;
                    cout << (ui.getLanguage() ? "ATM Mode: " : "ATM 모드: ")
                        << (atm_list[i]->issinglemode() ? (ui.getLanguage() ? "Single" : "단일") : (ui.getLanguage() ? "Multi" : "다중")) << endl;
                    cout << (ui.getLanguage() ? "Language Mode: " : "언어 모드: ") << atm_list[i]->getLanMode() << endl;
                    cout << (ui.getLanguage() ? "Cash on ATM: " : "ATM 내 현금: ") << atm_list[i]->cashinatm() << endl;
                }
                string raw_input;
                int atmChoice;
                while (true) {
                    cout << (ui.getLanguage() ? "Enter the number of ATM." : "ATM 번호를 입력하세요.") << endl;
                    cin >> raw_input;
                    if (raw_input == "-1") {
                        cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
                        for (size_t i = 0; i < atm_list.size(); ++i) {
                            cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                            atm_list[i]->transactionHistory(true);
                            continue;
                        }
                        exit(0);
                    }
                    else if (raw_input == "/") {
                        display_atm(atm_list, ui);
                        display_account(bank_list, ui);
                        cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
                        continue;
                    }
                    else if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << (ui.getLanguage() ? "Invalid input. Please enter a valid number." : "유효하지 않은 입력입니다. 올바른 숫자를 입력하세요.") << endl;
                        continue;
                    }
                    else {
                        atmChoice = stoi(raw_input);
                    }
                    if (atmChoice == -1) {
                        cout << (ui.getLanguage() ? "Returning to main menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                        exitToMainMenu = true;
                        break;
                    }
                    if (atmChoice > 0 && atmChoice <= static_cast<int>(atm_list.size())) {
                        break;
                    }
                    else {
                        cout << (ui.getLanguage() ? "Invalid choice. Please select again." : "잘못된 선택입니다. 다시 선택하세요.") << endl;
                    }
                }   if (exitToMainMenu) break;
                ATM* selectedATM = atm_list[atmChoice - 1];
                while (true) {
                    ui.clearScreen();
                    cout << (ui.getLanguage() ? "Insert your card." : "카드를 삽입하세요.") << endl;
                    string cardNumber;
                    cout << (ui.getLanguage() ? "Enter your card number (or '0' to return): " : "카드 번호를 입력하세요 (0을 입력하면 돌아갑니다): ");
                    cardNumber = globalinput_string(atm_list, bank_list, ui);
                    if (cardNumber == "0") {
                        cout << (ui.getLanguage() ? "Returning to main menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                        exitToMainMenu = true;
                        break;
                    }
                    else if (cardNumber == "admin" || cardNumber == "ADMIN" || cardNumber == "Admin") {
                        cout << (ui.getLanguage() ? "[Administrator Authentication] Please enter the password.\n"
                            : "[관리자 인증] 비밀번호를 입력해주세요.\n");
                        string adminpw;
                        cin >> adminpw;
                        if (adminpw == "admin") {
                            cout << (ui.getLanguage() ? "Authentication successful. Accessing transaction history...\n"
                                : "인증 성공. 거래 내역을 확인합니다...\n");
                            if (atm_list.empty()) {
                                cout << (ui.getLanguage() ? "No ATMs available to display transaction history."
                                    : "거래 내역을 표시할 ATM이 없습니다.") << endl;
                            }
                            else {
                                for (size_t i = 0; i < atm_list.size(); ++i) {
                                    cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                                    atm_list[i]->transactionHistory(false);
                                    continue;
                                }
                            }
                        }
                        else {
                            cout << (ui.getLanguage() ? "Authentication Failed.\n"
                                : "인증 실패\n");
                            continue;
                        }
                    }
                    if (cardNumber.empty() || cardNumber.length() != 12 || !std::all_of(cardNumber.begin(), cardNumber.end(), ::isdigit)) {
                        cout << (ui.getLanguage() ? "Invalid card number. Please try again." : "유효하지 않은 카드 번호입니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    if (selectedATM->issinglemode()) {
                        string atmBankNumber = selectedATM->getBankNumber();
                        string cardBankNumber = cardNumber.substr(0, 4);
                        if (atmBankNumber != cardBankNumber) {
                            cout << (ui.getLanguage() ? "Invalid card. This ATM only supports the bank it belongs to." : "유효하지 않은 카드입니다. 이 ATM은 해당 은행의 카드만 지원합니다.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                            cin.ignore();
                            cin.get();
                            continue;
                        }
                    }
                    Bank* targetBank = nullptr;
                    Account* targetAccount = nullptr;
                    for (auto bank : bank_list) {
                        if (bank->getBankNumber() == cardNumber.substr(0, 4)) {
                            targetBank = bank;
                            break;
                        }
                    }
                    if (!targetBank) {
                        cout << (ui.getLanguage() ? "Bank associated with card not found. Please try again." : "카드와 연결된 은행을 찾을 수 없습니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    for (int i = 0; i < targetBank->getNumOfAccounts(); ++i) {
                        Account* account = targetBank->getAccount(i);
                        if (account && account->getCardNumber() == cardNumber) {
                            targetAccount = account;
                            break;
                        }
                    }
                    if (!targetAccount) {
                        cout << (ui.getLanguage() ? "Account associated with card not found. Please try again." : "카드와 연결된 계좌를 찾을 수 없습니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }
                    selectedATM->setAccount(targetAccount);
                    int retryCount = 0;
                    const int maxRetries = 3;
                    while (retryCount < maxRetries) {
                        string password;
                        cout << (ui.getLanguage() ? "Enter your password: " : "비밀번호를 입력하세요: ");
                        password = globalinput_string(atm_list, bank_list, ui);
                        string correctPassword = targetAccount->getPassword();
                        if (correctPassword == password) {
                            ui.clearScreen();
                            cout << (ui.getLanguage() ? "Card authentication successful." : "카드 인증 성공.") << endl;
                            break;
                        }
                        retryCount++;
                        if (retryCount < maxRetries) {
                            int chancesLeft = maxRetries - retryCount;
                            cout << (ui.getLanguage() ? "Chance Left: " + std::to_string(chancesLeft)
                                : "남은 기회: " + std::to_string(chancesLeft)) << endl;
                            cout << (ui.getLanguage() ? "Incorrect password. Please try again."
                                : "비밀번호가 틀렸습니다. 다시 시도하세요.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            cin.get();
                        }
                    }
                    if (retryCount >= maxRetries) {
                        cout << (ui.getLanguage() ? "Error: Maximum retries exceeded." : "오류: 최대 시도 횟수를 초과했습니다.") << endl;
                        for (size_t i = 0; i < atm_list.size(); ++i) {
                            cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                            atm_list[i]->transactionHistory(true);
                            continue;
                        }
                        exit(0);
                    }
                    ui.clearScreen();
                    cout << (ui.getLanguage() ? "Card authentication successful." : "카드 인증 성공.") << endl;
                    if (selectedATM->getIsBilingual()) {
                        while (true) {
                            ui.showLanguageSettingsMenu();
                            int languageSelection = getIntegerInput(ui.getLanguage() ? "Please select a language: " : "언어를 선택하세요: ", &ui);
                            if (languageSelection == 1) {
                                ui.setLanguage(true);
                                ui.clearScreen();
                                cout << "Language changed to English.\n";
                                break;
                            }
                            else if (languageSelection == 2) {
                                ui.setLanguage(false);
                                ui.clearScreen();
                                cout << "언어가 한국어로 변경되었습니다.\n";
                                break;
                            }
                            else {
                                cout << (ui.getLanguage() ? "Invalid input. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                            }
                        }
                    }
                    string userOption;
                    while (true) {
                        ui.showUserMenu();
                        cout << (ui.getLanguage() ? "Select an option: " : "옵션을 선택하세요: ");
                        cin >> userOption;
                        if (userOption == "-1") {
                            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
                            for (size_t i = 0; i < atm_list.size(); ++i) {
                                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                                atm_list[i]->transactionHistory(true);
                                continue;
                            }
                            exit(0);
                        }
                        else if (userOption == "/") {
                            display_atm(atm_list, ui);
                            display_account(bank_list, ui);
                            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
                            continue;
                        }
                        else if (cin.fail()) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            ui.showErrorMessage();
                            continue;
                        }
                        if (userOption == "1") {
                            if (!selectedATM->deposit()) continue;
                        }
                        else if (userOption == "2") {
                            if (!selectedATM->withdraw()) continue;
                        }
                        else if (userOption == "3") {
                            if (!selectedATM->transfer()) continue;
                        }
                        else if (userOption == "4") {
                            cout << (ui.getLanguage() ? "Exiting to main menu." : "메인 메뉴로 돌아갑니다.") << endl;
                            exitToMainMenu = true;
                            break;
                        }
                        else {
                            cout << (ui.getLanguage() ? "Invalid option. Try again." : "잘못된 입력입니다. 다시 시도하세요.") << endl;
                        }
                        break;
                    }
                    if (exitToMainMenu) break;
                    break;
                }
                if (exitToMainMenu) break;
            }
        }
        else if (startSelection == "5") {
            while (true) {
                ui.clearScreen();
                cout << (ui.getLanguage() ? "[This menu is available only for administrators]" : "[관리자에게만 제공되는 메뉴입니다]") << endl;
                cout << (ui.getLanguage() ? "1. Authenticate as an administrator\n2. Return to Main Menu\n"
                    : "1. 관리자 인증하기\n2. 메인 메뉴로 돌아가기\n");
                string administrator;
                administrator = globalinput_string(atm_list, bank_list, ui);
                if (administrator == "1") {
                    cout << (ui.getLanguage() ? "Please authenticate as an administrator." : "관리자 인증을 해주세요.") << endl;
                    string auth;
                    auth = globalinput_string(atm_list, bank_list, ui);
                    if (auth == "admin" || auth == "Admin" || auth == "ADMIN") {
                        cout << (ui.getLanguage() ? "Authentication successful. Accessing transaction history...\n"
                            : "인증 성공. 거래 내역을 확인합니다...\n");
                        if (atm_list.empty()) {
                            cout << (ui.getLanguage() ? "No ATMs available to display transaction history."
                                : "거래 내역을 표시할 ATM이 없습니다.") << endl;
                        }
                        else {
                            for (size_t i = 0; i < atm_list.size(); ++i) {
                                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                                atm_list[i]->transactionHistory(false);
                            }
                        }
                        cout << (ui.getLanguage() ? "Press Enter to return to the menu..." : "메뉴로 돌아가려면 Enter 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break;
                    }
                    else {
                        cout << (ui.getLanguage() ? "[Authentication failed] Returning to menu...\n"
                            : "[인증 실패] 메뉴로 돌아갑니다...\n");
                        continue;
                    }
                }
                else if (administrator == "2") {
                    cout << (ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                    break;
                }
                else {
                    cout << (ui.getLanguage() ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도하세요.") << endl;
                }
            }
        }
        else if (startSelection == "6") {
            ui.showTransitionMessage(ui.getLanguage() ? "Exiting system. Goodbye!" : "시스템을 종료합니다. 안녕히 가세요!\n");
            cout << endl;
            for (size_t i = 0; i < atm_list.size(); ++i) {
                cout << (ui.getLanguage() ? "\nATM ID: " : "\nATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                atm_list[i]->transactionHistory(true);
                continue;
            }
            exit(0);
            break;
        }
        else {
            cout << (ui.getLanguage() ? "Invalid input. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
        }
    }
    return 0;
}
