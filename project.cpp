#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream> // 추가 필요
#include <exception>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

string globalinput_string(vector<ATM*> atm_list, vector<Bank*> bank_list, Interface ui) {
    string input;
    while (true) {
        cin >> input;
        if (input == "-1") {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
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
        cin >> raw_input; //일단 정수도 문자열로 반환

        if (raw_input == "-1") {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            exit(0);
        }
        else if (raw_input == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        else {
            input = stoi(raw_input); // 문자열을 정수로 변환
            return input; // 변환 성공 시 반환
        }
    }
}

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define WHITE "\033[97m"
#define BG_DARK "\033[48;5;235m"
#define BG_LIGHT "\033[48;5;248m"
#define ACCENT_BLUE "\033[38;5;81m"
#define ACCENT_GREEN "\033[38;5;76m"
#define ACCENT_RED "\033[38;5;160m"
#define NOMINMAX // windows.h의 max 매크로 비활성화


class Account;
class Bank;
class ATM;
class Interface;
class card;
class singleATM;
class multiATM;


int num_of_ATM = 0;
int num_of_banks = 0;


int getIntegerInput(const string& prompt, Interface ui) {
    string raw_input;
    int value;
    while (true) {
        cout << prompt;
        cin >> raw_input;

        // 입력 실패 처리
        if (raw_input == "-1") {
            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
            exit(0);
        }
        else if (raw_input == "/") {
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
            continue;
        }
        else if (cin.fail()) {
            cin.clear(); // 스트림 상태 초기화
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
            cout << "Error: Invalid input. Only integers are allowed. Please try again." << endl;
        }
        else {
            int input;
            input = stoi(raw_input); // 문자열을 정수로 변환
            return input; // 변환 성공 시 반환
        }


    }
}

std::vector<Bank*> bank_list; // Bank 객체 포인터의 리스트
std::vector<ATM*> atm_list;


class Account {
private:
    string account_number;
    string password;
    string owner_name;
    string card_number;
    double available_fund = 0;
    Bank* account_bank;
public:
    Account(string account_num, string owner, string card_num, double init_balance, Bank* bank, string pwd) : account_number(account_num), owner_name(owner), card_number(card_num), available_fund(init_balance), account_bank(bank), password(pwd) {
        account_number = account_num;
        owner_name = owner;
        card_num = card_number;
        available_fund = init_balance;
        account_bank = bank;
        password = pwd;
    }
    ~Account() { delete this; }
    bool authorize_user(string input_password) { return password == input_password; }
    double increase_account_balance(double amount) { return available_fund += amount; } //atm 속 돈 양 증가 추가
    double decrease_account_balance(double amount) { return available_fund -= amount; } //atm 속 돈 양 감소 추가
    string getAccountNumber() { return account_number; }
    string getOwnerName() { return owner_name; };
    double getAvailableFund() { return available_fund; }
    string getCardNumber() { return card_number; }
    string getPassword() { return password; }
};

class Bank {
private:
    string bank_name;
    string bank_number; //계좌번호나 카드번호에 식별용으로 활용
    Bank* bank_list[10];
    int num_of_banks = 0;
    string history[100];
    int num_of_transactions = 0;
    Account* accounts[100];
    int num_of_accounts = 0;
    Interface* ui;

public:
    Bank(string bankname, string banknumber, Interface* interface)
        : bank_name(bankname), bank_number(banknumber), ui(interface)
    {
        bank_list[num_of_banks++] = this;
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
    Account* make_account(string new_owner_name, string bankname, double initial_balance, string password);
    Account* getAccount(int index) const;
    int getNumOfAccounts() const;
    Account* access_to_account(int i);
    int calculateFee(string transaction_type, string cardBank, string target_bank);
    bool hasAccount(string accountnumber);
    void increase_receiver_balance(string receiver_account_number, double amount);
    bool show_authorize(string account_num, string input_password);
};
class Interface {
private:
    bool isEng;

    // 구분선 출력
    void printSeparator(int width = 70) const {
        std::cout << std::string(width, '=') << std::endl;
    }

    // 텍스트를 중앙에 출력
    void printCentered(const std::string& text, int width = 70) const {
        int padding = (width - static_cast<int>(text.length())) / 2;
        std::cout << std::string(padding, ' ') << text << std::endl;
    }

    // 박스 생성
    std::string createBox(const std::string& text, int boxWidth = 30, int boxHeight = 5) const {
        std::ostringstream boxStream;

        // 상단 테두리
        boxStream << "=" << std::string(boxWidth - 2, '=') << "=" << std::endl;

        // 빈 줄 생성
        int emptyLines = (boxHeight - 1) / 2;
        for (int i = 0; i < emptyLines; ++i) {
            boxStream << "=" << std::string(boxWidth - 2, ' ') << "=" << std::endl;
        }

        // 텍스트 중앙 정렬
        int textPadding = (boxWidth - 2 - static_cast<int>(text.length())) / 2;
        boxStream << "=" << std::string(textPadding, ' ') << text
            << std::string(boxWidth - 2 - textPadding - text.length(), ' ') << "=" << std::endl;

        // 빈 줄 생성
        for (int i = 0; i < emptyLines; ++i) {
            boxStream << "=" << std::string(boxWidth - 2, ' ') << "=" << std::endl;
        }

        // 하단 테두리
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

    // 4개의 박스를 출력
    void printFourBoxes(const std::vector<std::string>& options, int boxWidth = 30, int totalWidth = 70) const {
        // 4개의 박스 텍스트 준비 (빈 박스로 채우기)
        std::vector<std::string> filledOptions = options;
        while (filledOptions.size() < 4) {
            filledOptions.push_back(""); // 빈 박스 추가
        }

        // 상단 두 박스 출력
        std::string leftBox = createBox(filledOptions[0], boxWidth);
        std::string rightBox = createBox(filledOptions[1], boxWidth);

        std::istringstream leftStream(leftBox), rightStream(rightBox);
        std::string leftLine, rightLine;

        while (std::getline(leftStream, leftLine) && std::getline(rightStream, rightLine)) {
            int padding = (totalWidth - (boxWidth * 2)) / 3;
            std::cout << std::string(padding, ' ') << leftLine
                << std::string(padding, ' ') << rightLine << std::endl;
        }

        // 하단 두 박스 출력
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

    // 메뉴 출력
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

    // 화면 초기화
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
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기
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

        // 첫 번째 줄
        printTwoBoxes(isEng ? "1. Bank Management" : "1. 은행 관리", isEng ? "2. ATM Management" : "2. ATM 관리");

        // 두 번째 줄
        printTwoBoxes(isEng ? "3. Language Settings" : "3. 언어 설정", isEng ? "4. Insert Card" : "4. 카드 삽입");

        // 세 번째 줄
        printTwoBoxes(isEng ? "5. Transaction History" : "5. 거래 내역", isEng ? "6. Exit" : "6. 종료");

        printSeparator();
    }

    void showBankManagementMenu() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Bank Management" : "은행 관리", 70);
        printSeparator();

        // 첫 번째 줄
        printTwoBoxes(isEng ? "1. Add Bank" : "1. 은행 추가", isEng ? "2. Remove Bank" : "2. 은행 삭제");

        // 두 번째 줄
        printTwoBoxes(isEng ? "3. Create Account" : "3. 계좌 생성", isEng ? "4. Delete Account" : "4. 계좌 삭제");

        // 세 번째 줄
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

    void handleBankManagement() {
        while (true) {
            showBankManagementMenu();
            int choice = getIntegerInput(isEng ? "Select Option" : "옵션을 선택하세요: ", this);
            int num_of_banks = bank_list.size();

            if (choice == 1) {
                string bank_name, bank_number;
                cout << "새로운 은행 이름을 입력하세요: ";
                cin >> bank_name;
                cout << "새로운 은행 번호를 입력하세요(4자리): ";
                cin >> bank_number;

                Bank* new_bank = new Bank(bank_name, bank_number, this);
                bank_list.push_back(new_bank);
                std::cout << "은행이 추가되었습니다: " << bank_name << " (" << bank_number << ")\n";

            }
            else if (choice == 2) {
                string bank_name;
                cout << "삭제할 은행 이름을 입력하세요: ";
                cin >> bank_name;

                bool found = false;
                for (auto it = bank_list.begin(); it != bank_list.end(); ++it) {
                    if ((*it)->getBankName() == bank_name) {
                        delete* it; // 메모리 해제
                        bank_list.erase(it); // 벡터에서 제거
                        std::cout << "은행이 삭제되었습니다: " << bank_name << std::endl;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cout << "해당 이름의 은행을 찾을 수 없습니다.\n";
                }

            }
            else if (choice == 3) {
                string bank_name;
                cout << "계좌를 생성할 은행 이름을 입력하세요: ";
                cin >> bank_name;

                Bank* target_bank = nullptr;
                for (auto& bank : bank_list) {
                    if (bank->getBankName() == bank_name) {
                        target_bank = bank;
                        break;
                    }
                }


                if (target_bank) {
                    string owner, password;
                    double balance;
                    cout << "소유자 이름: ";
                    cin >> owner;
                    cout << "초기 입금액: ";
                    cin >> balance;
                    cout << "비밀번호: ";
                    cin >> password;

                    target_bank->createAccount(owner, balance, password);
                }
                else {
                    cout << "해당 은행을 찾을 수 없습니다.\n";
                }
            }
            else if (choice == 4) {
                string bank_name, account_number;
                cout << "계좌를 삭제할 은행 이름을 입력하세요: ";
                cin >> bank_name;

                Bank* target_bank = nullptr;
                for (int i = 0; i < num_of_banks; ++i) {
                    if (bank_list[i]->getBankName() == bank_name) {
                        target_bank = bank_list[i];
                        break;
                    }
                }

                if (target_bank) {
                    cout << "삭제할 계좌 번호를 입력하세요: ";
                    cin >> account_number;
                    target_bank->deleteAccount(account_number);
                }
                else {
                    cout << "해당 은행을 찾을 수 없습니다.\n";
                }
            }
            else if (choice == 5) {
                string bank_name;
                cout << "조회할 은행 이름을 입력하세요: ";
                cin >> bank_name;

                Bank* target_bank = nullptr;
                for (int i = 0; i < num_of_banks; ++i) {
                    if (bank_list[i]->getBankName() == bank_name) {
                        target_bank = bank_list[i];
                        break;
                    }
                }

                if (target_bank) {
                    target_bank->listAccounts();
                }
                else {
                    cout << "해당 은행을 찾을 수 없습니다.\n";
                }
            }
            else if (choice == 6) {
                cout << "메인 메뉴로 돌아갑니다.\n";
                break;
            }
            else {
                cout << "잘못된 입력입니다. 다시 시도해주세요.\n";
            }
        }
    }

    void showLanguageSettingsMenu() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Language Settings" : "언어 설정", 70);
        printSeparator();

        // 언어 선택 박스 출력
        printTwoBoxes(isEng ? "1. English" : "1. 영어", isEng ? "2. Korean" : "2. 한국어");
        printSeparator();

        // 사용자 입력 안내
        cout << (isEng ? "Please select your preferred language: " : "원하시는 언어를 선택해 주세요: ");
    }


    void showWelcomeMessage() const {
        printMenu(isEng ? "Welcome" : "환영합니다", { isEng ? "1. Deposit" : "1. 입금", isEng ? "2. Withdraw" : "2. 출금", isEng ? "3. Transfer" : "3. 송금", isEng ? "4. Exit" : "4. 종료" });
    }

    void showDepositMenu() const {
        printMenu(isEng ? "Deposit Menu" : "입금 메뉴", { isEng ? "1. Cash Deposit" : "1. 현금 입금" ,isEng ? "2. Check Deposit" : "2. 수표 입금",isEng ? "3. Exit" : "3. 거래 취소" });
    }

    void showWithdrawMenu(int balance) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Withdraw Menu" : "출금 메뉴", 70);
        printSeparator();
        printCentered((isEng ? "Account Balance: " : "계좌 잔액: ") + std::to_string(balance) + (isEng ? "won" : "원"), 70);
        printCentered(isEng ? "Enter the amount to withdraw or '0' to cancel:" : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요:", 70);
        printSeparator();
    }

    void showWithdrawUI(int balance) const {
        clearScreen(); // 화면 초기화
        printSeparator(); // 상단 구분선
        printCentered(isEng ? "Withdraw Menu" : "출금 메뉴", 70); // 메뉴 제목 중앙 출력
        printSeparator(); // 제목 하단 구분선

        // 잔액 및 지시문 중앙 정렬 출력
        printCentered((isEng ? "Account Balance: " : "계좌 잔액: ") + std::to_string(balance) + (isEng ? " units." : "원"), 70);
        printCentered(isEng ? "Enter the amount to withdraw or '0' to cancel:" : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요:", 70);

        printSeparator(); // 하단 구분선
    }

    void showBillInputUI() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Enter Number of Bills" : "출금할 지폐의 개수를 입력하세요", 70);
        printSeparator();
        printCentered(isEng ? "Input the number of bills for each denomination." : "각 단위에 해당하는 지폐의 수를 입력하세요.", 70);
        printSeparator();
    }

    void showWithdrawSuccessUI(int balance) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Withdraw Successful!" : "출금 성공!", 70);
        printSeparator();
        printCentered((isEng ? "Current Balance: " : "현재 잔액: ") + std::to_string(balance) + (isEng ? " units." : "원"), 70);
        printSeparator();
    }
    void showTransferOptionsUI() const {
        printMenu(isEng ? "Transfer Menu" : "송금 메뉴", { isEng ? "1. Cash Transfer" : "1. 현금 송금", isEng ? "2. Account Transfer" : "2. 계좌 송금", isEng ? "3. Cancel" : "3. 취소" });
    }
    void showAccountInputUI() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Enter the account number to transfer:" : "송금할 계좌 번호를 입력하세요.", 70);
        printSeparator();
        printCentered(isEng ? "Enter the account number or '0' to cancel." : "계좌 번호를 입력하거나 '0'을 눌러 취소하세요.", 70);
        printSeparator();
    }

    void showTransferCashUI(int transferAmount) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Insert cash." : "현금을 투입하세요.", 70);
        printSeparator();
        printCentered((isEng ? "Transfer amount: " : "송금 금액: ") + std::to_string(transferAmount) + (isEng ? " units." : "원"), 70);
        printSeparator();
    }
    void showTransferSuccessUI(int accountBalance) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Transfer Successful!" : "송금 성공!", 70);
        printSeparator();
        printCentered((isEng ? "Current Balance: " : "현재 계좌 잔액: ") + std::to_string(accountBalance) + (isEng ? " units." : "원"), 70);
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

    void showTransactionCancelledUI() const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Transaction Cancelled." : "거래가 취소되었습니다.", 70);
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
        printMenu(isEng ? "Depost completed" : "입금 완료", { (isEng ? "Depost success!" : "입금 성공!"), (isEng ? "Depost amount" : "총 입금 금액: ") + std::to_string(amount) + (isEng ? "won" : "원") });
    }

    void displayBalance(int balance) const {
        printMenu(isEng ? "Balance Inquiry" : "잔액 조회", { (isEng ? "Current Balance: " : "현재 잔액: ") + std::to_string(balance) + (isEng ? " units." : "원") });
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

public:
    record(const string& id, const string& card, const string& type, int amt)
        : transactionID(id), cardNumber(card), transactionType(type), amount(amt) {}

    string gettransactionID() { return transactionID; }
    string getcardnumber() { return cardNumber; }
    string gettransaction_type() { return transactionType; }
    int getamount() { return amount; }

    void display_one_transaction(Interface* ui) {
        cout << (ui->getLanguage() ? "Transaction ID: " : "거래 ID: ") << transactionID << endl;
        cout << (ui->getLanguage() ? "Card Number: " : "카드 번호: ") << cardNumber << endl;
        cout << (ui->getLanguage() ? "Transaction Type: " : "거래 유형: ") << transactionType << endl;
        cout << (ui->getLanguage() ? "Amount: " : "금액: ") << amount << endl;
        cout << "-------------------------------------------" << endl;
    }
};


class Card {
public:
    string cardNumber;
    string password;
    bool isAdmin;
    Card() : cardNumber(""), password(""), isAdmin(false) {} // 기본 생성자 추가

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
    int transaction_order = 0; ////// 거래 번호 추가
    int num_of_transaction = 0;
    record* transaction_records[100];////// 거래 내역 추가
    map<string, string> userPasswords;    // 카드 번호와 비밀번호 매핑
    map<string, Card> cards;            // 카드 번호와 카드 객체 매핑
    vector<string> transactionHistory;   // 거래 내역
    map<string, string> userInfo;        // 사용자 정보(ID, 카드 번호)
    int loginAttempts = 0;   // 로그인 시도 횟수
    Bank* primaryBank; // 주 은행
    bool isSingleBankMode; // Single Bank 모드 여부

public:
    ATM(Bank* atmBank, bool issingle, int arr[4], const string& atmId, Interface* uiinterface)
        : bank(atmBank), isSingleBankMode(issingle), atmID(atmId), ui(uiinterface) {
        for (int i = 0; i < 4; ++i) {
            cash[i] = arr[i];
        }
        if (bank) {
            atmBankName = bank->getBankName(); // Initialize atmBank with the bank's name
        }
    }

    string getatmNumber() { return atmNumber; }
    string getremainingcash(int i) { return to_string(cash[i]); }

    string make_atmNumber(Bank* atmBank);
    void singleBankMode(bool SingleBankMode);
    void addCard(const string& cardNumber, const string& password, const string& userID, bool isAdmin);
    string trim(const string& str);
    string getBankNumber();
    string getatmID() { return atmID; }
    string getatmbank() const { return atmBankName; }
    bool issinglemode() { return isSingleBankMode; }
    int cashinatm() const { return cash[0] * 1000 + cash[1] * 5000 + cash[2] * 10000 + cash[3] * 50000; }
    bool insertCard();
    void adminMenu();
    void userMenu(ATM* selectedATM);
    bool authenticateUser(Card& card);
    void displayATMModes();
    bool validateCardForSingleBank(const string& cardNumber);
    bool isInteger(const std::string& input);
    bool isPositive(int input);
    bool hasSufficientCash(int amount);
    void dispenseCash(int amount);
    void reduceCash(int amount);
    bool card_verification(Account* card);
    int getValidInput(const string& prompt, Interface* ui);
    void depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4);
    bool deposit();
    bool fee_cash_calculator(int fee);
    bool change_ATM_dec(int change);
    bool fee_account_calculator(int fee);
    bool withdraw();
    bool transfer();
    void processTransaction(int depositAmount, const string& cardBank);
    string transactionid();
    void transaction_recording(string transaction_type, int amount);
    void display_history(const string& card_number);
    void setAccount(Account* acc) {
        account = acc;
    }
};

class singleATM : public ATM {
public:
    singleATM(Bank* bank, bool issingle, int arr[4], const string& id, Interface* ui)
        : ATM(bank, issingle, arr, id, ui) {}
};

class multiATM : public ATM {
public:
    multiATM(Bank* bank, bool issingle, int arr[4], const string& id, Interface* ui)
        : ATM(bank, issingle, arr, id, ui) {}
};



void display_atm(vector<ATM*> atm_list, Interface ui) {
    if (atm_list.empty()) {
        cout << (ui.getLanguage() ? "There are no ATMs created." : "생성된 ATM이 없습니다.") << endl;
    }
    else {
        for (int i = 0; i < num_of_ATM; i++) {
            cout << "ATM[SN:" << atm_list[i]->getatmNumber() << "] remaining cash : {KRW 50000 : " << atm_list[i]->getremainingcash(3) << ", KRW 10000 : " << atm_list[i]->getremainingcash(2) << ", KRW 5000 : " << atm_list[i]->getremainingcash(1)
                << ", KRW 1000 : " << atm_list[i]->getremainingcash(0) << "} " << endl;
        }
    }

}
void display_account(vector<Bank*> bank_list, Interface ui) {
    if (num_of_banks == 0) {
        cout << (ui.getLanguage() ? "There are no Banks created." : "생성된 은행이 없습니다.") << endl;
    }
    else {
        bool account_exist = false;
        for (int i = 0; i < num_of_banks; i++) {
            if (bank_list[i]->getNumOfAccounts() != 0) {
                account_exist = true;
            }
        }
        if (!account_exist) {
            cout << (ui.getLanguage() ? "There are no Accounts created." : "생성된 계좌가 없습니다.") << endl;
        }
        else {
            for (int i = 0; i < num_of_banks; i++) {
                for (int j = 0; j < bank_list[i]->getNumOfAccounts(); j++) {

                    cout << "Account[Bank:" << bank_list[i]->getBankName() << ", No : " << bank_list[i]->access_to_account(j)->getAccountNumber() << ", Owner : " << bank_list[i]->access_to_account(j)->getOwnerName() << "]"
                        << " balance: " << bank_list[i]->access_to_account(j)->getAvailableFund() << endl;
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


Account* Bank::createAccount(const string& owner, double balance, const string& password) {
    if (num_of_accounts >= 100) {
        cout << (ui->getLanguage() ? "Fail to generate Accout: Maximum account" : "계좌 생성 실패: 최대 계좌 수를 초과했습니다.\n");
        return nullptr;
    }

    string account_number = bank_number + to_string(rand() % 90000000 + 10000000);
    string card_number = bank_number + to_string(rand() % 9000 + 1000);

    Account* new_account = new Account(account_number, owner, card_number, balance, this, password);
    accounts[num_of_accounts++] = new_account;

    cout << (ui->getLanguage() ? "Account generated.\n" : "계좌가 생성되었습니다.\n");
    cout << (ui->getLanguage() ? "Account Number: " : "계좌 번호: ") << new_account->getAccountNumber() << endl;
    cout << (ui->getLanguage() ? "Card Number" : "카드 번호: ") << new_account->getCardNumber() << endl;
    return new_account;
}

bool Bank::deleteAccount(const string& account_number) {
    for (int i = 0; i < num_of_accounts; ++i) {
        if (accounts[i]->getAccountNumber() == account_number) {
            delete accounts[i];
            accounts[i] = accounts[--num_of_accounts]; // 마지막 계좌를 이동하여 채움
            cout << (ui->getLanguage() ? "Account deleted: " : "계좌 삭제 완료: ") << account_number << endl;
            return true;
        }
    }
    cout << (ui->getLanguage() ? "Unavailable Account.\n" : "계좌를 찾을 수 없습니다.\n");
    return false;
}

void Bank::listAccounts() const {
    if (num_of_accounts == 0) {
        cout << this->getBankName() << (ui->getLanguage() ? ": No registered accounts." : ": 등록된 계좌가 없습니다.") << endl;
        return;
    }

    cout << (ui->getLanguage() ? "Bank " : "은행 ") << bank_name
        << (ui->getLanguage() ? "'s account list:" : "의 계좌 목록:") << endl;

    for (int i = 0; i < num_of_accounts; ++i) {
        Account* account = accounts[i];
        cout << std::fixed << std::setprecision(0); // 잔액을 정수형으로 출력
        cout << (ui->getLanguage() ? "- Owner: " : " - 소유자: ") << account->getOwnerName()
            << (ui->getLanguage() ? ", Bank Name: " : ", 은행 이름: ") << bank_name
            << (ui->getLanguage() ? ", Account Number: " : ", 계좌 번호: ") << account->getAccountNumber()
            << (ui->getLanguage() ? ", Card Number: " : ", 카드 번호: ") << account->getCardNumber()
            << (ui->getLanguage() ? ", Balance: " : ", 잔액: ") << account->getAvailableFund() << (ui->getLanguage() ? " units\n" : "원\n") << endl;
    }
}

Account* Bank::make_account(string new_owner_name, string bankname, double initial_balance, string password) {
    if (num_of_accounts >= 100) {
        cout << "계좌 생성 실패: 최대 계좌 수를 초과했습니다.\n";
        return nullptr;
    }

    // 계좌 번호 생성 (은행 고유 번호 + 현재 계좌 수 + 1)
    string account_number = bank_number + to_string(num_of_accounts + 1);

    // 카드 번호 생성 (12자리: 앞 4자리 은행 고유 번호 + 뒤 8자리 무작위 숫자)
    string card_number = bank_number; // 앞 4자리: 은행 고유 번호

    for (int i = 0; i < 8; ++i) {
        card_number += to_string(rand() % 10); // 뒤 8자리 무작위 숫자 추가
    }

    // 계좌 객체 생성
    Account* new_account = new Account(account_number, new_owner_name, card_number, initial_balance, this, password);
    accounts[num_of_accounts++] = new_account;

    // 생성된 계좌 정보 출력
    cout << (ui->getLanguage() ? "Account is initialized.\n" : "계좌가 생성되었습니다.\n");
    cout << "======================================================================" << endl;
    cout << (ui->getLanguage() ? "<Account Information>\n" : "<계좌 정보>\n");
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
    return nullptr; // 유효하지 않은 인덱스일 경우
}

int Bank::getNumOfAccounts() const {
    return num_of_accounts;
}
Account* Bank::access_to_account(int i) { return accounts[i]; }


int Bank::calculateFee(string transaction_type, string cardBank = "", string target_bank = "") {
    if (transaction_type == "transfer") {
        if (!cardBank.empty() && !target_bank.empty()) {
            if (cardBank == bank_name && target_bank == bank_name) {
                return 2000;
            }
            else if (cardBank != bank_name && target_bank != bank_name) {
                return 4000;
            }
            else {
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
void Bank::increase_receiver_balance(string receiver_account_number, double amount) { // 계좌 이체용
    for (int i = 0; i < num_of_accounts; i++) {
        if (accounts[i]->getAccountNumber() == receiver_account_number) {
            accounts[i]->increase_account_balance(amount);
            return;
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



string ATM::make_atmNumber(Bank* atmBank) {
    srand(time(0));
    int random_number3 = rand() % 900 + 100;
    string atmnum = account->getAccountNumber().substr(0, 4) + to_string(random_number3);
    return atmnum;
}
void ATM::singleBankMode(bool SingleBankMode) {
    SingleBankMode = isSingleBankMode;
}
void ATM::addCard(const string& cardNumber, const string& password, const string& userID, bool isAdmin) {
    cards[cardNumber] = Card(cardNumber, password, isAdmin); // 카드 등록
}
string ATM::trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
string ATM::getBankNumber() {
    return bank->getBankNumber(); // 기존 코드에 trim 적용
}

bool ATM::insertCard() {
    cout << (ui->getLanguage() ? "Enter your card number: " : "카드 번호를 입력하세요: ");
    string cardNumber;
    cin >> cardNumber;

    // 카드 번호가 비어있거나 12자리가 아닐 경우
    if (cardNumber.empty() || cardNumber.length() != 12 || !std::all_of(cardNumber.begin(), cardNumber.end(), ::isdigit)) {
        cout << (ui->getLanguage() ? "Error: Invalid card number. Please enter a 12-digit number." : "오류: 유효하지 않은 카드 번호입니다. 12자리 숫자를 입력하세요.") << endl;
        return false;
    }

    string bankCode = cardNumber.substr(0, 4); // 카드 번호 앞 4자리 은행 코드 추출

    // 카드 번호가 등록된 카드인지 확인
    if (cards.find(cardNumber) == cards.end()) {
        cout << (ui->getLanguage() ? "Error: Card not found." : "에러: 카드가 존재하지 않습니다.") << endl;
        return false;
    }

    Card& card = cards[cardNumber];

    // 비밀번호 인증
    string inputPassword;
    cout << (ui->getLanguage() ? "Enter your password: " : "비밀번호를 입력하세요: ");
    cin >> inputPassword;

    if (card.password != inputPassword) {
        cout << (ui->getLanguage() ? "Incorrect password. Please try again." : "비밀번호가 틀렸습니다. 다시 시도하세요.") << endl;
        return false;
    }
    cout << (ui->getLanguage() ? "Card authentication successful!" : "카드 인증 성공!") << endl;
    return true;
}
void ATM::adminMenu() {
    while (true) {
        ui->showAdminMenu(); // 관리자 메뉴 출력
        string selection;
        cin >> selection;

        //if (selection == "/") {
        //    // 관리자일 경우 모든 거래 내역 출력
        //    display_history("admin");
        //    continue;  // 다시 메뉴로 돌아감
        //}
        if (selection == "1") {
            // 관리자 기능: 거래 내역 조회
            cout << (ui->getLanguage() ? "Viewing transaction history..." : "거래 내역 조회...") << endl;
            display_history("admin");
        }
        else if (selection == "2") {
            cout << (ui->getLanguage() ? "Exiting admin menu. Returning to main menu.\n" : "관리자 메뉴를 종료합니다. 메인 메뉴로 돌아갑니다.\n");
            break; // 관리자 메뉴 종료
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

    cout << "Entering userMenu..." << endl;

    while (true) {
        ui->showUserMenu();
        cout << (ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: ");

        // 스트림 상태 초기화
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string selection;
        cin >> selection;

        if (cin.fail()) {
            cin.clear(); // 입력 스트림 상태 초기화
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남은 입력 제거
            cout << (ui->getLanguage() ? "Invalid input. Please try again.\n" : "잘못된 입력입니다. 다시 시도해 주세요.\n");
            continue;
        }

        if (selection.empty()) {
            cout << (ui->getLanguage() ? "Invalid input. Please try again.\n" : "잘못된 입력입니다. 다시 시도해 주세요.\n");
            continue;
        }
        cout << "User selected: " << selection << endl;


        // 숫자 입력 처리
        if (selection == "1") {
            cout << "Selected Deposit menu..." << endl;
            bool success = deposit();
            if (!success) {
                cout << "Deposit failed. Returning to user menu..." << endl;
            }
        }
        else if (selection == "2") {
            cout << "Selected Withdraw menu..." << endl;
            bool success = withdraw();
            if (!success) {
                cout << "Withdraw failed. Returning to user menu..." << endl;
            }
        }
        else if (selection == "3") {
            cout << "Selected Transfer menu..." << endl;
            bool success = transfer();
            if (!success) {
                cout << "Transfer failed. Returning to user menu..." << endl;
            }
        }
        else if (selection == "4") {
            cout << (ui->getLanguage() ? "Exiting ATM. Please take your card.\n" : "ATM을 종료합니다. 카드를 가져가세요.\n");
            break;
        }
        else if (selection == "/") {
            cout << "Displaying ATM and account details..." << endl;
            display_atm(atm_list, ui);
            display_account(bank_list, ui);
            continue;
        }
        else if (selection == "admin") {
            cout << "Accessing admin menu..." << endl;
            adminMenu();
            continue;
        }
        else {
            cout << (ui->getLanguage() ? "Invalid option. Please try again.\n" : "잘못된 선택입니다. 다시 시도해 주세요.\n");
        }
    }

    cout << "Exiting userMenu..." << endl;
}

bool ATM::authenticateUser(Card& card) {
    string inputPassword;
    int attempts = 0;

    while (attempts < 3) {
        cout << (ui->getLanguage() ? "Enter your password: " : "비밀번호를 입력하세요: ");
        cin >> inputPassword;

        if (card.password == inputPassword) {
            cout << (ui->getLanguage() ? "Authentication successful! Welcome!\n" : "인증 성공! 환영합니다!\n");
            return true;
        }

        attempts++;
        cout << (ui->getLanguage()
            ? "Incorrect password. " + to_string(3 - attempts) + " attempts remaining.\n"
            : "비밀번호가 틀렸습니다. 남은 시도 횟수: " + to_string(3 - attempts) + "번\n");
    }

    cout << (ui->getLanguage() ? "Too many incorrect attempts. Card has been returned.\n"
        : "잘못된 시도가 너무 많습니다. 카드가 반환되었습니다.\n");
    return false;
}
void ATM::displayATMModes() {
    cout << (ui->getLanguage()
        ? "Current ATM Mode: " + string(isSingleBankMode ? "Single Bank ATM" : "Multi Bank ATM") + "\n"
        : "현재 ATM 모드: " + string(isSingleBankMode ? "단일 은행 ATM" : "다중 은행 ATM") + "\n");
}
bool ATM::validateCardForSingleBank(const string& cardNumber) {
    string bankCode = cardNumber.substr(0, 4); // 카드 번호 앞 4자리
    return bankCode == bank->getBankNumber();
}
bool ATM::isInteger(const std::string& input) { //string type이 integer인지 확인
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

    for (int i = 0; i < 4; ++i) {
        numNotes[i] = min(amount / denominations[i], cash[i]);
        amount -= numNotes[i] * denominations[i];
        cash[i] -= numNotes[i];
    }

    cout << (ui->getLanguage() ? "Dispensed cash as follows:\n" : "다음과 같이 현금이 지급되었습니다:\n");
    for (int i = 0; i < 4; ++i) {
        cout << denominations[i] << (ui->getLanguage() ? " won: " : "원: ") << numNotes[i] << endl;
    }
}
void ATM::reduceCash(int amount) {
    dispenseCash(amount);
}
bool ATM::card_verification(Account* card) { // card 비밀번호 인증
    for (int i = 0; i < 3; i++) {
        string password;
        cout << (ui->getLanguage() ? "Please enter your card password." : "카드 비밀번호를 입력해 주세요.") << endl;
        cin >> password;
        if (not isInteger(password)) return false;
        if (bank->show_authorize(card->getAccountNumber(), password)) {
            cout << (ui->getLanguage() ? "Password verificated successfully" : "비밀번호 인증 성공") << endl;
            return true;
        }
        else {
            cout << (ui->getLanguage() ? "Password does not match." : "비밀번호가 일치하지 않습니다.") << endl;
            cout << (ui->getLanguage() ? "Number of attempts to enter password: " : "남은 비밀 번호 입력 가능 횟수: ") << 2 - i << endl;
        }
    }
    cout << (ui->getLanguage() ? "Transaction failed: Password authentication failed" : "거래 실패: 비밀번호 인증 실패") << endl;
    return false;

}
int ATM::getValidInput(const string& prompt, Interface* ui) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        // 입력 스트림에 에러가 발생했거나 정수가 아닌 값이 들어온 경우
        if (cin.fail()) {
            cin.clear(); // 스트림 상태 초기화
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
            cout << (ui->getLanguage() ? "Wrong Input. Please enter a valid integer." : "잘못된 입력입니다. 유효한 정수를 입력하세요.") << endl;
            continue;
        }

        // -1을 입력한 경우 종료 신호로 처리
        if (value == -1) {
            return -1;
        }

        // 0 이상 최대 정수 값 이내의 범위 검증
        if (value >= 0 && value <= numeric_limits<int>::max()) {
            return value; // 유효한 값 반환
        }
        else {
            cout << (ui->getLanguage() ? "Invalid input. Please enter a number between 0 and " + to_string(numeric_limits<int>::max()) + "."
                : "잘못된 입력입니다. 0에서 " + to_string(numeric_limits<int>::max()) + " 사이의 숫자를 입력하세요.") << endl;
        }
    }
}

void ATM::depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4) {
    int totalBills = 0;

    while (true) {
        totalBills = 0; // 각 루프에서 초기화
        cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '-1' to exit):\n"
            : "각 지폐의 개수를 입력하세요 (또는 '-1'을 입력해 종료):\n");

        // 1,000원 지폐 입력
        m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: "), ui);
        if (m1 == -1) {
            cout << (ui->getLanguage() ? "Exiting deposit process.\n" : "입금 프로세스를 종료합니다.\n");
            throw runtime_error(ui->getLanguage() ? "User exited deposit process." : "사용자가 입금 프로세스를 종료했습니다.");
        }
        totalBills += m1;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Error: Maximum number of bills (50) exceeded. Please re-enter.\n"
                : "오류: 지폐 개수 제한(50장)을 초과했습니다. 다시 입력해주세요.\n");
            continue;
        }

        // 5,000원 지폐 입력
        m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: "), ui);
        if (m2 == -1) {
            cout << (ui->getLanguage() ? "Exiting deposit process.\n" : "입금 프로세스를 종료합니다.\n");
            throw runtime_error(ui->getLanguage() ? "User exited deposit process." : "사용자가 입금 프로세스를 종료했습니다.");
        }
        totalBills += m2;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Error: Maximum number of bills (50) exceeded. Please re-enter.\n"
                : "오류: 지폐 개수 제한(50장)을 초과했습니다. 다시 입력해주세요.\n");
            continue;
        }

        // 10,000원 지폐 입력
        m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: "), ui);
        if (m3 == -1) {
            cout << (ui->getLanguage() ? "Exiting deposit process.\n" : "입금 프로세스를 종료합니다.\n");
            throw runtime_error(ui->getLanguage() ? "User exited deposit process." : "사용자가 입금 프로세스를 종료했습니다.");
        }
        totalBills += m3;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Error: Maximum number of bills (50) exceeded. Please re-enter.\n"
                : "오류: 지폐 개수 제한(50장)을 초과했습니다. 다시 입력해주세요.\n");
            continue;
        }

        // 50,000원 지폐 입력
        m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: "), ui);
        if (m4 == -1) {
            cout << (ui->getLanguage() ? "Exiting deposit process.\n" : "입금 프로세스를 종료합니다.\n");
            throw runtime_error(ui->getLanguage() ? "User exited deposit process." : "사용자가 입금 프로세스를 종료했습니다.");
        }
        totalBills += m4;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Error: Maximum number of bills (50) exceeded. Please re-enter.\n"
                : "오류: 지폐 개수 제한(50장)을 초과했습니다. 다시 입력해주세요.\n");
            continue;
        }

        break; // 총합이 50 이하일 때 루프 종료
    }
}

bool ATM::deposit() {
    cout << "Starting deposit process..." << endl;

    if (account == nullptr) {
        cout << (ui->getLanguage() ? "Error: Account is not initialized." : "계좌가 개설되어 있지 않았습니다.") << endl;
        return false;
    }
    string accountNumber = account->getAccountNumber();
    if (accountNumber.length() < 4) {
        cout << (ui->getLanguage() ? "Error: Account number too short. Length: " : "에러: 계좌번호가 너무 짧습니다. ") << accountNumber.length() << endl;
        throw std::length_error(ui->getLanguage() ? "Account number length is less than 4." : "계좌번호가 4자리보다 짧음.");
    }
    string cardBank = accountNumber.substr(0, 4);

    while (true) {
        ui->showDepositMenu();

        int input = getValidInput((ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: "), ui);

        if (input == 1) { // 현금 예금
            int m1, m2, m3, m4, depositAmount;

            try {
                depositCash(ui, m1, m2, m3, m4); // 예금 금액 계산
            }
            catch (const runtime_error& e) {
                cout << (ui->getLanguage() ? "Deposit cancelled. Returning to the previous menu...\n"
                    : "입금이 취소되었습니다. 이전 메뉴로 돌아갑니다...\n");
                return false; // 예금 종료
            }

            depositAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;

            // 거래 기록 저장
            transaction_recording("Deposit", depositAmount);
            ui->showDepositAmount(depositAmount);
            processTransaction(depositAmount, cardBank);
            return true;
        }
        else if (input == 2) { // 수표 예금
            int checkAmount;
            while (true) {
                ui->promptForCheckDeposit();
                cin >> checkAmount;

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
                if (checkAmount < 0) {
                    cout << (ui->getLanguage() ? "Negative input is not allowed. Try Again." : "음수 입력은 허용되지 않습니다. 다시 시도하세요.") << endl;
                    continue;
                }
                if (checkAmount < 100000) {
                    cout << (ui->getLanguage() ? "Check deposit requires at least 100,000. Please try again." : "수표 예금은 최소 100,000원 이상만 가능합니다. 다시 시도해주세요.") << endl;
                }
                else {
                    break;
                }
            }

            // 거래 기록 저장
            transaction_recording("Check Deposit", checkAmount);
            ui->showDepositAmount(checkAmount);
            processTransaction(checkAmount, cardBank);
            return true;
        }
        else if (input == 3) { // 거래 취소
            ui->transactionCancelled();
            return false;
        }
        else {
            ui->showErrorMessage();
        }
    }
}



bool ATM::fee_cash_calculator(int fee) {
    ui->requestFeePayment(fee);

    int m1, m2, m3, m4;
    int total = 0; // 총 투입된 금액
    int depositAmount = 0; // 총 투입된 금액 계산

    // 1,000원 지폐 수 입력
    cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
    cin >> m1;
    if (m1 < 0) {
        cout << (ui->getLanguage() ? "Invalid input. Please enter a positive number of bills." : "잘못된 입력입니다. 양수의 지폐 개수를 입력해 주세요.") << endl;
        return false;
    }
    total += m1 * 1000; // 1,000원 지폐 금액 계산
    depositAmount += m1 * 1000;

    if (total >= fee) {
        cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
        cin.ignore(); // 이전 입력의 개행 문자를 제거
        cin.get();
        if (total > fee) {
            int change = total - fee;
            cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore(); // 이전 입력의 개행 문자를 제거
            cin.get();
            return true;
            if (!change_ATM_dec(change)) {
                return false;  // ATM에 충분한 현금이 없을 경우
            }
        }
        return true;
    }

    // 5,000원 지폐 수 입력
    cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
    cin >> m2;
    if (m2 < 0) {
        cout << (ui->getLanguage() ? "Invalid input. Please enter a positive number of bills." : "잘못된 입력입니다. 양수의 지폐 개수를 입력해 주세요.") << endl;
        return false;
    }
    total += m2 * 5000;
    depositAmount += m2 * 5000;

    if (total >= fee) {
        cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
        cin.ignore(); // 이전 입력의 개행 문자를 제거
        cin.get();
        if (total > fee) {
            int change = total - fee;
            cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore(); // 이전 입력의 개행 문자를 제거
            cin.get();
            return true;
            if (!change_ATM_dec(change)) {
                return false;  // ATM에 충분한 현금이 없을 경우
            }
        }
        return true;
    }

    // 10,000원 지폐 수 입력
    cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
    cin >> m3;
    if (m3 < 0) {
        cout << (ui->getLanguage() ? "Invalid input. Please enter a positive number of bills." : "잘못된 입력입니다. 양수의 지폐 개수를 입력해 주세요.") << endl;
        return false;
    }
    total += m3 * 10000;
    depositAmount += m3 * 10000;

    if (total >= fee) {
        cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
        if (total > fee) {
            int change = total - fee;
            cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore(); // 이전 입력의 개행 문자를 제거
            cin.get();
            return true;
            if (!change_ATM_dec(change)) {
                return false;  // ATM에 충분한 현금이 없을 경우
            }
        }
        return true;
    }

    // 50,000원 지폐 수 입력
    cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
    cin >> m4;
    if (m4 < 0) {
        cout << (ui->getLanguage() ? "Invalid input. Please enter a positive number of bills." : "잘못된 입력입니다. 양수의 지폐 개수를 입력해 주세요.") << endl;
        return false;
    }
    total += m4 * 50000;
    depositAmount += m4 * 50000;

    if (total >= fee) {
        cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
        cin.ignore(); // 이전 입력의 개행 문자를 제거
        cin.get();
        if (total > fee) {
            int change = total - fee;
            cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
            cin.ignore(); // 이전 입력의 개행 문자를 제거
            cin.get();
            return true;
            if (!change_ATM_dec(change)) {
                return false;  // ATM에 충분한 현금이 없을 경우
            }
        }
        return true;
    }

    cout << (ui->getLanguage() ? "Failure to pay fees: Insufficient cash" : "수수료 납부 실패: 현금 부족") << endl;
    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
    cin.ignore(); // 이전 입력의 개행 문자를 제거
    cin.get();
    return false;
}
bool ATM::change_ATM_dec(int change) { // 거스름돈 줄 때 ATM 가용 현금 감소
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
bool ATM::fee_account_calculator(int fee) { // 계좌에서 수수료 계산
    ui->requestFeePayment(fee);
    cout << (ui->getLanguage() ? "Fee: " : "수수료: ") << fee << endl;
    cout << (ui->getLanguage() ? "Paying a fee from your account..." : "계좌에서 수수료를 지불 중...") << endl;
    double balance = account->getAvailableFund();
    if (balance >= fee) {
        account->decrease_account_balance(fee);
        cout << (ui->getLanguage() ? "Successfully Fee paid!" : "수수료 납부 성공!") << endl;
        cout << "======================================================================" << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return true;
    }
    else {
        cout << (ui->getLanguage() ? "Failed to pay fee: Insufficient account balance" : "수수료 납부 실패: 계좌 잔액 부족") << endl;
        cout << "======================================================================" << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }
}

bool ATM::withdraw() {
    int withdrawAmount;
    if (ui == nullptr || account == nullptr) {
        cout << (ui->getLanguage() ? "Error: Account is not initialized." : "계좌가 개설되어 있지 않습니다.") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }
    string cardBank = account->getAccountNumber().substr(0, 4); // 카드의 은행 코드 확인
    int withdrawalCount = 0; // 세션 당 인출 횟수 제한
    if (not card_verification(account)) return false;

    // 한 세션당 최대 3번 출금
    while (withdrawalCount <= 3) {
        cout << (ui->getLanguage() ? "\n=== Withdraw Menu ===\nAccount Balance: " : "\n=== 출금 메뉴 ===\n계좌 잔액: ") << account->getAvailableFund() << "won\n";
        cout << (ui->getLanguage() ? "Enter amount to withdraw or '0' to cancel: " : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요: ");
        while (true) {
            cin >> withdrawAmount;
            if (cin.fail() || withdrawAmount < 0) {
                cin.clear(); // 입력 스트림 초기화
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 입력 버퍼 비우기
                cout << (ui->getLanguage() ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도해 주세요.") << endl;
                continue;
            }
            if (isPositive(withdrawAmount)) break;
        }

        // 출금 취소 처리
        if (withdrawAmount == 0) {
            ui->transactionCancelled();
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }

        // 출금 금액 한도 및 계좌 잔액 부족 오류 처리
        if (withdrawAmount > 500000) {
            cout << (ui->getLanguage() ? "Error: Transaction limit is 500,000 won per withdrawal." : "오류: 거래 한도는 출금 당 500,000원입니다.") << endl;
            exit(0);
        }
        if (withdrawAmount > account->getAvailableFund()) {
            throw runtime_error(ui->getLanguage() ? "Insufficient account balance." : "계좌 잔액 부족");
            exit(0);
        }

        // ATM 현금 부족 오류 처리
        if (not hasSufficientCash(withdrawAmount)) {
            cout << (ui->getLanguage() ? "Error: ATM has insufficient cash." : "오류: ATM에 충분한 현금이 없습니다.") << endl;
            exit(0); // ATM 현금 부족 시 종료
        }

        int fee = (cardBank == bank->getBankName()) ? 1000 : 2000;
        if (account->getAvailableFund() < withdrawAmount + fee) {
            cout << (ui->getLanguage() ? "Error: Insufficient balance to cover the fee." : "오류: 수수료를 포함한 잔액이 부족합니다.") << endl;
            exit(0);
        }

        // 지폐 단위별 입력 요청
        int m1, m2, m3, m4, totalBills;
        cout << (ui->getLanguage() ? "Enter the number of bills for withdrawal:\n" : "출금할 지폐의 개수를 입력하세요:\n");
        cout << (ui->getLanguage() ? "1,000 won bills: " : "1,000원 지폐 개수: ");
        cin >> m1;
        cout << (ui->getLanguage() ? "5,000 won bills: " : "5,000원 지폐 개수: ");
        cin >> m2;
        cout << (ui->getLanguage() ? "10,000 won bills: " : "10,000원 지폐 개수: ");
        cin >> m3;
        cout << (ui->getLanguage() ? "50,000 won bills: " : "50,000원 지폐 개수: ");
        cin >> m4;

        totalBills = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;
        if (totalBills != withdrawAmount) {
            cout << (ui->getLanguage() ? "Error: The entered bills do not match the withdrawal amount." : "오류: 입력한 지폐 금액이 출금 금액과 일치하지 않습니다.") << endl;
            continue;
        }

        // 출금 및 수수료 차감
        account->decrease_account_balance(withdrawAmount + fee);
        dispenseCash(withdrawAmount);
        reduceCash(withdrawAmount);
        withdrawalCount++;

        cout << (ui->getLanguage() ? "Withdrawal Successful!" : "출금 성공!") << endl;
        cout << (ui->getLanguage() ? "Current Balance: " : "현재 잔액: ") << account->getAvailableFund() << "won\n";

        // 추가 출금 여부 확인
        int continueWithdrawal;
        cout << (ui->getLanguage() ? "Would you like to make another withdrawal? (1: Yes, 0: No): " : "추가 인출을 원하시면 1을, 종료하시려면 0을 입력하세요: ");
        cin >> continueWithdrawal;

        if (continueWithdrawal == 0) {

            transaction_recording("Withdraw", withdrawAmount);
            display_history(account->getCardNumber());

            return true; // 종료
        }
        else if (continueWithdrawal != 1) {
            cout << (ui->getLanguage() ? "Error: Invalid input. Ending withdrawal session." : "오류: 잘못된 입력입니다. 출금 세션을 종료합니다.") << endl;
            return false;
        }
    }
    cout << (ui->getLanguage() ? "Error: Withdrawal limit exceeded. Please restart." : "오류: 최대 인출 시도를 초과했습니다. 다시 시작하세요.") << endl;
    return false; // 3회 초과 시 종료
}

bool ATM::transfer() {
    int input;
    while (true) {
        ui->showTransferOptions();
        cin >> input;
        if (input == 1) { //현금 송금
            cout << (ui->getLanguage() ? "Please enter the account to transfer or '0' to cancel." : "송금할 계좌를 입력하거나 '0'을 눌러 취소하세요.") << endl;
            string account_num;
            cin >> account_num;
            if (not isInteger(account_num)) return false;
            if (account_num == "0") {
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            if (bank->hasAccount(account_num)) { // 존재하는 계좌인 경우
                int transferAmount;
                cout << (ui->getLanguage() ? "How much money do you want to transfer" : "얼만큼의 돈을 송금하시겠습니까?") << endl;
                cout << (ui->getLanguage() ? "Enter amount to transfer or '0' to cancel" : "송금할 금액을 입력하거나 '0'을 눌러 취소하세요.") << endl;
                while (true) {
                    cin >> transferAmount;
                    if (isPositive(transferAmount)) break;
                }
                if (transferAmount == 0) {
                    ui->transactionCancelled();
                    return false;
                }

                int m1, m2, m3, m4, deposited;
                cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
                while (true) {
                    cin >> m1;
                    if (isPositive(m1)) break;
                }
                cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
                while (true) {
                    cin >> m2;
                    if (isPositive(m2)) break;
                }
                cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
                while (true) {
                    cin >> m3;
                    if (isPositive(m3)) break;
                }
                cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
                while (true) {
                    cin >> m4;
                    if (isPositive(m4)) break;
                }


                deposited = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;
                cout << "======================================================================" << endl;

                if (deposited < transferAmount) { // 부족한 돈을 넣은 경우
                    cout << (ui->getLanguage() ? "Transaction failed: Insufficient cash" : "거래 실패: 현금 부족") << endl;
                    cout << "======================================================================" << endl;
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    break; //TransferOption으로
                }
                else if (deposited > transferAmount) { // 초과된 돈을 넣은 경우
                    cout << (ui->getLanguage() ? "Transfer amount: " : "송금 금액: ") << transferAmount << endl;
                    cout << (ui->getLanguage() ? "Amount entered: " : "투입된 금액: ") << deposited << endl << endl;
                    cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << deposited - transferAmount << endl;
                    cout << "======================================================================" << endl;
                }

                // 수수료 부과
                if (fee_cash_calculator(1000)) {
                    transaction_recording("Cash Transfer", transferAmount);
                    display_history(account->getCardNumber());

                    cout << (ui->getLanguage() ? "Transaction successful" : "거래 성공") << endl;

                }
                else { // 수수료 실패: 다시 돈 빼기
                    cash[0] -= m1;
                    cash[1] -= m2;
                    cash[2] -= m3;
                    cash[3] -= m4;
                    cout << (ui->getLanguage() ? "Transaction failed: fee payment issue." : "거래 실패: 수수료 납부 실패") << endl;
                }
            }
            else { // 잘못된 계좌 번호 입력
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account" : "거래 실패: 존재하지 않는 계좌") << endl;
                cout << "======================================================================" << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                // welcome으로
                return false;
            }
        }
        else if (input == 2) { //계좌 송금
            string cardBank = account->getAccountNumber().substr(0, 4);
            string ATMBank = bank->getBankNumber();


            cout << (ui->getLanguage() ? "Please enter the account number to transfer or '0' to cancel." : "송금할 계좌 번호를 입력하거나 '0'을 눌러 취소하세요.") << endl;
            string account_num;
            cin >> account_num;
            if (not isInteger(account_num)) return false;
            if (account_num == "0") {
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
            if (bank->hasAccount(account_num)) { // 존재하는 계좌인 경우
                string transferBank = account_num.substr(0, 4);
                double transferAmount;
                cout << (ui->getLanguage() ? "How much money do you want to transfer" : "얼만큼의 돈을 송금하시겠습니까?") << endl;
                cout << account->getAvailableFund() << (ui->getLanguage() ? "KRW You can transfer." : "원 송금 가능합니다.") << endl;
                cout << endl << (ui->getLanguage() ? "Enter amount to transfer or '0' to cancel" : "송금할 금액을 입력하거나 '0'을 눌러 취소하세요.") << endl;
                while (true) {
                    cin >> transferAmount;
                    if (isPositive(transferAmount)) break;
                }
                cout << "======================================================================" << endl;
                if (transferAmount == 0) {
                    ui->transactionCancelled();
                    cout << "======================================================================" << endl;
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
                if (account->getAvailableFund() < transferAmount) { // 계좌에 돈이 부족한 경우
                    cout << (ui->getLanguage() ? "Transaction failed: Insufficient account balance" : "거래 실패: 계좌 잔액 부족") << endl;
                    cout << "======================================================================" << endl;
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }

                // 이체 과정
                account->decrease_account_balance(transferAmount);
                bank->increase_receiver_balance(account_num, transferAmount);

                // 수수료 부과
                if ((ATMBank == cardBank) and (cardBank == transferBank)) { // 같은 은행 : 같은 계좌
                    if (fee_account_calculator(2000)) {
                        cout << (ui->getLanguage() ? "Transfer completed successfully" : "송금 성공") << endl;
                        cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        cout << "======================================================================" << endl;
                        transaction_recording("Transfer", transferAmount);
                        display_history(account->getCardNumber());
                    }
                    else { // 수수료 실패 다시 이체금 넣어 주기
                        account->increase_account_balance(transferAmount);
                        return false;
                    }

                }
                else if ((ATMBank == cardBank) and (cardBank != transferBank)) { // 같은 은행 : 다른 계좌
                    if (fee_account_calculator(3000)) {
                        cout << (ui->getLanguage() ? "Transfer completed successfully" : "송금 성공") << endl;
                        cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        cout << "======================================================================" << endl;
                        transaction_recording("Transfer", transferAmount);
                        display_history(account->getCardNumber());

                    }
                    else {
                        account->increase_account_balance(transferAmount);
                        return false;
                    }
                }
                else { // 다른 은행 : 다른 계좌
                    if (fee_account_calculator(4000)) {
                        cout << (ui->getLanguage() ? "Transfer completed successfully" : "송금 성공") << endl;
                        cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        cout << "======================================================================" << endl;
                        transaction_recording("Transfer", transferAmount);
                        display_history(account->getCardNumber());
                    }
                    else {
                        account->increase_account_balance(transferAmount);
                        return false;
                    }
                }
            }
            else { // 잘못된 계좌 번호 입력
                cout << "======================================================================" << endl;
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account" : "거래 실패: 존재하지 않는 계좌") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                // welcome으로
                return false;
            }
        }
        else if (input == 3) { // 거래 취소
            ui->transactionCancelled();
            return false;
        }
        else {
            ui->showErrorMessage();
            return false;
        }
    }
    return false;
}

void ATM::processTransaction(int depositAmount, const string& cardBank) {
    int fee = bank->calculateFee(cardBank);

    fee_cash_calculator(fee);
    account->increase_account_balance(depositAmount);
    //ui->displayBalance(account->getAvailableFund());
    //ui->showDepositAmount(depositAmount);

    /*
            if (fee_cash_calculator(fee) >= fee) {
                account->increase_account_balance(depositAmount);
                ui->showDepositAmount(depositAmount);
                ui->displayBalance(account->getAvailableFund());

                if (fee_cash_calculator(fee) > fee) {
                    cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << fee_cash_calculator(fee) - fee << (ui->getLanguage() ? " units." : "원.") << endl;
                }
            }
            else {
                cout << (ui->getLanguage() ? "Insufficient fee payment. Returning to the main menu." : "수수료가 부족하여 거래를 진행할 수 없습니다. 메인 메뉴로 돌아갑니다.") << endl;
            }*/
}
string ATM::transactionid() {
    return account->getAccountNumber().substr(0, 4) + to_string(++transaction_order);
}

///////거래 내역 기록
void ATM::transaction_recording(string transaction_type, int amount) {
    if (num_of_transaction >= 100) {
        cout << (ui->getLanguage() ? "Transaction history is full. Cannot record more transactions."
            : "거래 기록이 가득 찼습니다. 더 이상 기록할 수 없습니다.") << endl;
        return;
    }

    transaction_records[num_of_transaction] = new record(transactionid(), account->getCardNumber(), transaction_type, amount);
    num_of_transaction++;

    cout << (ui->getLanguage() ? "Transaction recorded successfully." : "거래가 성공적으로 기록되었습니다.") << endl;
}

void ATM::display_history(const string& card_number) {
    if (card_number == "admin") {
        cout << (ui->getLanguage() ? "Transaction History Inquiry: Would you like to make an inquiry?\n"
            : "거래 기록 조회: 조회하시겠습니까?") << endl;
        cout << (ui->getLanguage() ? "1. Yes\n2. No" : "1. 예\n2. 아니오") << endl;

        string select;
        cin >> select;

        if (select == "1") {
            if (num_of_transaction == 0) {
                cout << (ui->getLanguage() ? "No transaction history available." : "거래 내역이 없습니다.") << endl;
                return;
            }

            cout << "************Transaction History************" << endl;
            for (int i = 0; i < num_of_transaction; ++i) {
                transaction_records[i]->display_one_transaction(ui);
            }

            cout << (ui->getLanguage() ? "Would you like to save it to a file?\n" : "파일로 저장하시겠습니까?") << endl;
            cout << (ui->getLanguage() ? "1. Yes\n2. No" : "1. 예\n2. 아니오") << endl;

            string print;
            cin >> print;

            if (print == "1") {
                ofstream file("TransactionHistory.txt");

                if (!file) {
                    cerr << (ui->getLanguage() ? "Error: Unable to create file." : "오류: 파일을 생성할 수 없습니다.") << endl;
                    return;
                }

                file << "************Transaction History************" << endl;
                for (int i = 0; i < num_of_transaction; ++i) {
                    file << "-------------------------------------------" << endl;
                    file << "Transaction ID: " << transaction_records[i]->gettransactionID() << endl;
                    file << "Card Number: " << transaction_records[i]->getcardnumber() << endl;
                    file << "Transaction Type: " << transaction_records[i]->gettransaction_type() << endl;
                    file << "Amount: " << transaction_records[i]->getamount() << endl;
                    file << "-------------------------------------------" << endl;
                }
                file.close();
                cout << (ui->getLanguage() ? "Transaction history saved to TransactionHistory.txt"
                    : "거래 내역이 TransactionHistory.txt에 저장되었습니다.") << endl;
            }
            else {
                cout << (ui->getLanguage() ? "Returning to main menu." : "메인 메뉴로 돌아갑니다.") << endl;
            }
        }
        else if (select == "2") {
            cout << (ui->getLanguage() ? "Returning to main menu." : "메인 메뉴로 돌아갑니다.") << endl;
        }
        else {
            cout << (ui->getLanguage() ? "Invalid input. Returning to main menu." : "잘못된 입력입니다. 메인 메뉴로 돌아갑니다.") << endl;
        }
    }
    else if (card_number == account->getCardNumber()) {
        if (num_of_transaction > 0) {
            transaction_records[num_of_transaction - 1]->display_one_transaction(ui);
        }
        else {
            cout << (ui->getLanguage() ? "No recent transactions available." : "최근 거래 내역이 없습니다.") << endl;
        }
    }
    else {
        cout << (ui->getLanguage() ? "No matching transaction history found." : "일치하는 거래 내역을 찾을 수 없습니다.") << endl;
    }
}



string generateAtmID(const string& bankNumber, int atmCount) {
    ostringstream oss;
    oss << bankNumber << setfill('0') << setw(2) << atmCount + 1; // 은행 번호 + ATM 순번
    return oss.str();
}



int main() {
    // 은행 및 계정 초기화
    Interface ui;
    vector<ATM*> atm_list;
    vector<Bank*> bank_list; // 여러 은행을 저장할 벡터
    int current_atm_num;
    ATM* atm;




    // 프로그램 루프
    while (true) {
        // 초기 화면 출력 및 옵션 선택
        ui.showStartScreen();
        cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;
        string startSelection;
        startSelection = globalinput_string(atm_list, bank_list, ui);

        if (startSelection == "1") {  // 은행 관리
            ui.showTransitionMessage(ui.getLanguage() ? "Moving to Bank Management Menu..." : "은행 관리 메뉴로 이동 중...");
            while (true) {
                ui.showBankManagementMenu();
                cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;

                string bankSelection;
                bankSelection = globalinput_string(atm_list, bank_list, ui);
                if (bankSelection == "1") {
                    // 은행 추가
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
                    // 은행 삭제
                    for (int i = 0; i < bank_list.size(); i++) {
                        // 은행 목록
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
                else if (bankSelection == "3") { // 계좌 생성
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available. Please add a bank first.\n" : "등록된 은행이 없습니다. 먼저 은행을 추가해주세요.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
                        cin.get();
                        continue;
                    }

                    cout << (ui.getLanguage() ? "Select a bank to create an account:\n" : "계좌를 생성할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }
                    //----------------------------------------
                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ", ui);
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
                    initialBalance = getIntegerInput(ui.getLanguage() ? "Enter the initial balance: " : "초기 잔액을 입력하세요: ", ui);
                    cout << (ui.getLanguage() ? "Enter the password: " : "비밀번호를 입력하세요: ");
                    password = globalinput_string(atm_list, bank_list, ui);

                    selectedBank->make_account(ownerName, selectedBank->getBankName(), initialBalance, password);
                    cout << (ui.getLanguage() ? "Account created successfully.\n" : "계좌가 성공적으로 생성되었습니다.\n") << endl;
                }
                else if (bankSelection == "4") { // 계좌 삭제
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available.\n" : "등록된 은행이 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }

                    cout << (ui.getLanguage() ? "Select a bank to delete an account:\n" : "계좌를 삭제할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }

                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ", ui);
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue;
                    }

                    Bank* selectedBank = bank_list[bankChoice - 1];
                    string accountNumber;

                    cout << (ui.getLanguage() ? "Enter the account number to delete: " : "삭제할 계좌 번호를 입력하세요: ");
                    accountNumber = globalinput_string(atm_list, bank_list, ui);

                    if (selectedBank->deleteAccount(accountNumber)) {
                        cout << (ui.getLanguage() ? "Account removed successfully.\n" : "계좌가 성공적으로 삭제되었습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                    }
                    else {
                        cout << (ui.getLanguage() ? "Error: Account number not found.\n" : "오류: 해당 계좌 번호를 찾을 수 없습니다.\n");
                    }
                }
                else if (bankSelection == "5") { // 계좌 목록 조회
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No Accounts available.\n" : "등록된 계좌가 없습니다.\n");
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
                        cin.get();
                        continue;
                    }
                    ui.clearScreen();

                    for (int i = 0; i < bank_list.size(); i++) {
                        cout << "======================================================================" << endl;
                        cout << i + 1 << ". " << endl;
                        bank_list[i]->listAccounts();
                    }
                    cout << "======================================================================" << endl;

                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                }
                else if (bankSelection == "6") {
                    // 메인 메뉴로 돌아가기
                    ui.showTransitionMessage(ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...");
                    break;
                }
                //else if (bankSelection == "/" || bankSelection == "0") {
                //    globalinput_int(bankSelection, atm_list, bank_list, ui);
                //    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                //    cin.ignore(); // 이전 입력의 개행 문자를 제거
                //    cin.get();
                //    continue;}


                else {
                    // 잘못된 입력 처리
                    cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }
        else if (startSelection == "2") { // ATM 설정
            ui.showTransitionMessage(ui.getLanguage() ? "Moving to ATM Management Menu..." : "ATM 관리 메뉴로 이동 중...");

            while (true) {
                // UI ATM management 불러 오기
                ui.showAMTManagementMenu();
                cout << (ui.getLanguage() ? "Please select an option: " : "옵션 선택: ") << endl;

                string ATMSelection;
                ATMSelection = globalinput_string(atm_list, bank_list, ui);

                if (ATMSelection == "1") {
                    // ATM 추가
                    // ATM 추가
                    string bankname;
                    int bank_index;
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No bank to create ATM. " : "ATM을 생성할 은행이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
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
                    bank_index--; // bank indexing

                    if (not (bank_index >= 0) and (bank_index < bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid input value." : "잘못된 입력값입니다.") << endl;
                        cout << (ui.getLanguage() ? "Creating ATM is canceled." : "ATM 생성이 취소되었습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
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
                            cin.ignore(); // 이전 입력의 개행 문자를 제거
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

                        if (issingle) {
                            atm_list.push_back(new singleATM(bank_list[bank_index], issingle, cashes, atmId, &ui));
                            cout << (ui.getLanguage() ? "ATM is initialized successfully." : "ATM이 성공적으로 생성되었습니다.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                            cin.ignore();
                            cin.get();
                            break;
                        }
                        else {
                            atm_list.push_back(new multiATM(bank_list[bank_index], issingle, cashes, atmId, &ui));
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                            cin.ignore();
                            cin.get();
                            break;
                        }

                    }


                    current_atm_num = bank_index;
                }
                else if (ATMSelection == "2") {
                    // ATM 삭제
                    if (atm_list.empty()) {
                        cout << (ui.getLanguage() ? "No ATM to delete." : "삭제할 ATM이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
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

                        if (num == 0) { // 0 입력 시 이전 화면으로 돌아가기
                            cout << (ui.getLanguage() ? "Returning to previous menu..." : "이전 화면으로 돌아갑니다...") << endl;
                            break;
                        }

                        if (num > 0 && num <= atm_list.size()) {
                            num--; // 1-based index를 0-based로 변환
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
                else if (ATMSelection == "3") { // ATM 목록 확인
                    // ATM 삭제
                    if (atm_list.empty()) {
                        cout << (ui.getLanguage() ? "No ATM Exist" : "ATM이 존재하지 않습니다.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
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
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
                        cin.get();
                        break;
                        /*
                        cout << (ui.getLanguage() ? "Press '0' to exit." : "0을 누르면 돌아갑니다.");
                        int num;
                        cin >> num;

                        if (num == 0) { // 0 입력 시 이전 화면으로 돌아가기
                            cout << (ui.getLanguage() ? "Returning to previous menu..." : "이전 화면으로 돌아갑니다...") << endl;
                            break;
                        }*/


                    }
                }
                else if (ATMSelection == "4") { // 메인 메뉴 돌아가기
                    ui.showTransitionMessage(ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...");
                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    break;
                }

                else {
                    // 잘못된 입력 처리
                    cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }

        else if (startSelection == "3") {  // 언어 설정
            while (true) {
                ui.showLanguageSettingsMenu();
                int languageSelection = getIntegerInput(ui.getLanguage() ? "Please select a language: " : "언어를 선택하세요: ", ui);
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
        else if (startSelection == "4") { // 카드 삽입
            ui.clearScreen(); // 이전 화면 제거
            if (atm_list.empty()) { // ATM이 존재하지 않는 경우
                cout << (ui.getLanguage() ? "Please create an ATM first." : "먼저 ATM을 생성해 주세요.") << endl;
                cout << (ui.getLanguage() ? "Press Enter to return to the previous menu..." : "이전 메뉴로 돌아가려면 엔터 키를 누르세요...") << endl;
                cin.ignore();
                cin.get();
                continue; // 루프의 처음으로 돌아감            }
            }
            bool exitToMainMenu = false; // 메인 메뉴로 돌아가기 위한 플래그 변수

            while (true) {
                // ATM 목록 출력
                cout << (ui.getLanguage() ? "Select ATM for transaction." : "거래를 진행할 ATM을 선택해 주세요.") << endl;
                for (int i = 0; i < atm_list.size(); i++) {
                    cout << i + 1 << "." << endl;
                    cout << (ui.getLanguage() ? "ATM ID: " : "ATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                    cout << (ui.getLanguage() ? "ATM Bank: " : "ATM 은행: ") << atm_list[i]->getatmbank() << endl;
                    cout << (ui.getLanguage() ? "ATM Mode: " : "ATM 모드: ")
                        << (atm_list[i]->issinglemode() ? (ui.getLanguage() ? "Single" : "단일") : (ui.getLanguage() ? "Multi" : "다중")) << endl;
                    cout << (ui.getLanguage() ? "Cash on ATM: " : "ATM 내 현금: ") << atm_list[i]->cashinatm() << endl;
                }

                // ATM 선택
                string raw_input;
                int atmChoice;
                while (true) {
                    cout << (ui.getLanguage() ? "Enter the number of ATM (or 0 to return): " : "ATM 번호를 입력하세요 (0을 입력하면 돌아갑니다): ");
                    cin >> raw_input;


                    if (raw_input == "-1") {
                        cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
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
                        atmChoice = stoi(raw_input); // 문자열을 정수로 변환
                    }

                    if (atmChoice == 0) {
                        cout << (ui.getLanguage() ? "Returning to main menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                        exitToMainMenu = true;
                        break; // 초기 메뉴로 돌아감
                    }

                    if (atmChoice > 0 && atmChoice <= static_cast<int>(atm_list.size())) {
                        break; // 올바른 ATM 선택
                    }
                    else {
                        cout << (ui.getLanguage() ? "Invalid choice. Please select again." : "잘못된 선택입니다. 다시 선택하세요.") << endl;
                    }
                }   if (exitToMainMenu) break; // 카드 인증 루프 탈출

                ATM* selectedATM = atm_list[atmChoice - 1];

                // 카드 삽입 및 인증
                while (true) {
                    ui.clearScreen();
                    ui.showTransitionMessage(ui.getLanguage() ? "Insert your card." : "카드를 삽입하세요.");
                    string cardNumber;

                    cout << (ui.getLanguage() ? "Enter your card number (or '0' to return): " : "카드 번호를 입력하세요 (0을 입력하면 돌아갑니다): ");
                    cardNumber = globalinput_int(atm_list, bank_list, ui);

                    if (cardNumber == "0") {
                        cout << (ui.getLanguage() ? "Returning to main menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                        exitToMainMenu = true;
                        break; // 초기 메뉴로 돌아감
                    }

                    // 카드 번호 유효성 검사
                    if (cardNumber.empty() || cardNumber.length() != 12 || !std::all_of(cardNumber.begin(), cardNumber.end(), ::isdigit)) {
                        cout << (ui.getLanguage() ? "Invalid card number. Please try again." : "유효하지 않은 카드 번호입니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
                        cin.get();

                        continue; // 루프 재시작
                    }

                    // 단일 은행 ATM 모드에서 카드 은행 번호 확인
                    if (selectedATM->issinglemode()) {
                        string atmBankNumber = selectedATM->getBankNumber();
                        string cardBankNumber = cardNumber.substr(0, 4);

                        if (atmBankNumber != cardBankNumber) {
                            cout << (ui.getLanguage() ? "Invalid card. This ATM only supports the bank it belongs to." : "유효하지 않은 카드입니다. 이 ATM은 해당 은행의 카드만 지원합니다.") << endl;
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                            cin.ignore(); // 이전 입력의 개행 문자를 제거
                            cin.get();
                            continue; // 루프 재시작
                        }
                    }

                    // 은행 및 계좌 탐색
                    Bank* targetBank = nullptr;
                    Account* targetAccount = nullptr;

                    for (auto bank : bank_list) {
                        if (bank->getBankNumber() == cardNumber.substr(0, 4)) {
                            targetBank = bank;
                            break; // 은행 찾음
                        }
                    }

                    if (!targetBank) {
                        cout << (ui.getLanguage() ? "Bank associated with card not found. Please try again." : "카드와 연결된 은행을 찾을 수 없습니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                        cin.ignore();
                        cin.get();
                        continue; // 루프 재시작
                    }

                    for (int i = 0; i < targetBank->getNumOfAccounts(); ++i) {
                        Account* account = targetBank->getAccount(i);
                        if (account && account->getCardNumber() == cardNumber) {
                            targetAccount = account;
                            break; // 계좌 찾음
                        }
                    }

                    if (!targetAccount) {
                        cout << (ui.getLanguage() ? "Account associated with card not found. Please try again." : "카드와 연결된 계좌를 찾을 수 없습니다. 다시 시도하세요.") << endl;
                        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                        cin.ignore(); // 이전 입력의 개행 문자를 제거
                        cin.get();
                        continue; // 루프 재시작
                    }

                    selectedATM->setAccount(targetAccount);

                    int retryCount = 0;
                    const int maxRetries = 3; // 최대 비밀번호 입력 시도 횟수

                    // 비밀번호 입력 및 확인
                    string password;
                    while (retryCount < maxRetries){
                        cout << (ui.getLanguage() ? "Enter your password: " : "비밀번호를 입력하세요: ");
                        password = globalinput_int(atm_list, bank_list, ui);

                        if (targetAccount->getPassword() != password) {
                            // 남은 기회를 계산하고 출력
                            int chancesLeft = 2 - retryCount;
                            cout << (ui.getLanguage() ? "Chance Left: " + std::to_string(chancesLeft) : "남은 기회: " + std::to_string(chancesLeft)) << endl;
                            cout << (ui.getLanguage() ? "Incorrect password. Please try again." : "비밀번호가 틀렸습니다. 다시 시도하세요.") << endl;

                            // 사용자에게 Enter 키 입력을 기다림
                            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
                            cin.ignore(); // 이전 입력의 개행 문자를 제거
                            cin.get();    // Enter 키 입력 대기

                            retryCount++;
                            continue; // 루프 재시작
                        }
                        break;
                    }
                    if (retryCount >= maxRetries) {
                        cout << (ui.getLanguage() ? "Error: Maximum retries exceeded. " : "오류: 최대 시도 횟수를 초과했습니다.") << endl;
                        exit(0); // 터미널 종료
                    }

                    // 인증 성공 시 사용자 메뉴 호출
                    ui.clearScreen();
                    cout << (ui.getLanguage() ? "Card authentication successful." : "카드 인증 성공.") << endl;

                    // 사용자 메뉴 출력
                    string userOption;
                    while (true) {
                        ui.showUserMenu();
                        cout << (ui.getLanguage() ? "Select an option: " : "옵션을 선택하세요: ");
                        cin >> userOption;

                        if (userOption == "-1") {
                            cout << (ui.getLanguage() ? "Exiting the program..." : "프로그램을 종료합니다...") << endl;
                            exit(0);
                        }
                        else if (userOption == "/") {
                            display_atm(atm_list, ui);
                            display_account(bank_list, ui);
                            cout << (ui.getLanguage() ? "Please enter another input: " : "다른 입력을 다시 해주세요: ");
                            continue;
                        }
                        //----------
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
                    }
                    if (exitToMainMenu) break; // 카드 인증 루프 탈출
                    break; // 사용자 메뉴 종료 후 초기 메뉴로 복귀
                }
                if (exitToMainMenu) break; // 카드 인증 루프 탈출
            }
        }

        else if (startSelection == "5") {  // 거래 내역 보기
            while (true) { // 반복 처리
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
                                cout << (ui.getLanguage() ? "ATM ID: " : "ATM 고유 번호: ") << atm_list[i]->getatmID() << endl;
                                atm_list[i]->display_history("admin"); // 거래 내역 출력
                            }
                        }

                        cout << (ui.getLanguage() ? "Press Enter to return to the menu..." : "메뉴로 돌아가려면 Enter 키를 누르세요...");
                        cin.ignore();
                        cin.get();
                        break; // 거래 내역 확인 후 반복 종료
                    }
                    else {
                        cout << (ui.getLanguage() ? "[Authentication failed] Returning to menu...\n"
                            : "[인증 실패] 메뉴로 돌아갑니다...\n");
                        continue;
                    }
                }
                else if (administrator == "2") { // 메인 메뉴로 돌아가기
                    cout << (ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                    break;
                }
                else {
                    cout << (ui.getLanguage() ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도하세요.") << endl;
                }
            }
        }

        else if (startSelection == "6") {  // 종료
            ui.showTransitionMessage(ui.getLanguage() ? "Exiting system. Goodbye!" : "시스템을 종료합니다. 안녕히 가세요!");
            exit(0);
        }
        //else if (startSelection == "/") {
        //    // ATM 및 은행 목록 출력
        //    if (atm_list.empty()) {
        //        cout << (ui.getLanguage() ? "There are no ATMs created." : "생성된 ATM이 없습니다.") << endl;
        //    }
        //    else {
        //        display_atm(atm_list, ui);
        //    }

        //    if (bank_list.empty()) {
        //        cout << (ui.getLanguage() ? "There are no Banks created." : "생성된 은행이 없습니다.") << endl;
        //    }
        //    else {
        //        display_account(bank_list, ui);
        //    }

        //    // 초기화면으로 돌아가기
        //    cout << (ui.getLanguage() ? "Press Enter to return to the main menu..." : "메인 메뉴로 돌아가려면 엔터를 누르세요...") << endl;
        //    cin.ignore();  // 이전 입력 버퍼 제거
        //    cin.get();     // 엔터 입력 대기
        //    continue;
        //}

        else {
            cout << (ui.getLanguage() ? "Invalid input. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
        }

    }
    return 0;
}
