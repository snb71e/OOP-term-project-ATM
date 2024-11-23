
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



using namespace std;

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

int getIntegerInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        // 입력 실패 처리
        if (cin.fail()) {
            cin.clear(); // 스트림 상태 초기화
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
            cout << "Error: Invalid input. Only integers are allowed. Please try again." << endl;
        }
        else {
            return value; // 유효한 입력 반환
        }
    }
}

std::vector<Bank*> bank_list; // Bank 객체 포인터의 리스트


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

ATM* atm_list[10];
int num_of_ATM = 0;
int num_of_banks = 0;


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

public:
    Bank(string bankname, string banknumber)
        : bank_name(bankname), bank_number(banknumber)
    {
        bank_list[num_of_banks++] = this;
    }
    ~Bank() {
        for (int i = 0; i < num_of_accounts; ++i) {
            delete accounts[i];
        }
    }
    //int fee_calculator_deposit(string input_cardbank); //deposit, transfer, withdraw 각각 설정
    string getBankName() const { return bank_name; }
    string getBankNumber() const {
        return bank_number;
    }

    Account* createAccount(const string& owner, double balance, const string& password) {
        if (num_of_accounts >= 100) {
            cout << "계좌 생성 실패: 최대 계좌 수를 초과했습니다.\n";
            return nullptr;
        }

        string account_number = bank_number + to_string(num_of_accounts + 1);
        string card_number = bank_number + to_string(rand() % 9000 + 1000);

        Account* new_account = new Account(account_number, owner, card_number, balance, this, password);
        accounts[num_of_accounts++] = new_account;

        cout << "계좌가 생성되었습니다.\n";
        cout << "계좌 번호: " << new_account->getAccountNumber() << endl;
        cout << "카드 번호: " << new_account->getCardNumber() << endl;
        return new_account;
    }

    bool deleteAccount(const string& account_number) {
        for (int i = 0; i < num_of_accounts; ++i) {
            if (accounts[i]->getAccountNumber() == account_number) {
                delete accounts[i];
                accounts[i] = accounts[--num_of_accounts]; // 마지막 계좌를 이동하여 채움
                cout << "계좌 삭제 완료: " << account_number << endl;
                return true;
            }
        }
        cout << "계좌를 찾을 수 없습니다.\n";
        return false;
    }

    void listAccounts() const {
        if (num_of_accounts == 0) {
            cout << "등록된 계좌가 없습니다.\n";
            return;
        }
        cout << "은행 " << bank_name << "의 계좌 목록:\n";
        for (int i = 0; i < num_of_accounts; ++i) {
            Account* account = accounts[i];
            cout << std::fixed << std::setprecision(0); // 잔액을 정수형으로 출력
            cout << " - 소유자: " << account->getOwnerName()
                << ", 은행 이름: " << bank_name
                << ", 계좌 번호: " << account->getAccountNumber()
                << ", 카드 번호: " << account->getCardNumber()
                << ", 잔액: " << account->getAvailableFund() << "원\n";
        }
    }



    Account* make_account(string new_owner_name, string bankname, double initial_balance, string password) {
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
        cout << "계좌가 생성되었습니다.\n";
        cout << " - 소유자: " << new_owner_name << "\n"
            << " - 은행 명: " << bankname << "\n"
            << " - 계좌 번호: " << account_number << "\n"
            << " - 카드 번호: " << card_number << "\n"
            << " - 초기 잔액: " << initial_balance << "원\n";

        return new_account;
    }


    Account* getAccount(int index) const {
        if (index >= 0 && index < num_of_accounts) {
            return accounts[index];
        }
        return nullptr; // 유효하지 않은 인덱스일 경우
    }

    int getNumOfAccounts() const {
        return num_of_accounts;
    }

    /*Bank(string name) : bank_name(name) {}
    int calculateFee(const string& cardBank) const {
        return (cardBank == bank_name) ? 1000 : 2000;
    }*/
    Account* access_to_account(int i) { return accounts[i]; }

    int calculateFee(string transaction_type, string cardBank = "", string target_bank = "") {
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

    //void write_history_inventory(string transaction_detail) {
    //    history[num_of_transactions++] = transaction_detail;
    //} // 거래 내용을 atm의 메서드 내에서 작성 후 해당 함수 적용

    //void display_history() {
    //    for (int i = 0; i < num_of_transactions; i++) {
    //        cout << history[i] << endl;
    //    }
    //}
    bool hasAccount(string accountnumber) {
        for (int i = 0; i < num_of_accounts; i++) {
            if (accounts[i]->getAccountNumber() == accountnumber) {
                return true;
            }
        }
        return false;
    }
    void increase_receiver_balance(string receiver_account_number, double amount) { // 계좌 이체용
        for (int i = 0; i < num_of_accounts; i++) {
            if (accounts[i]->getAccountNumber() == receiver_account_number) {
                accounts[i]->increase_account_balance(amount);
                return;
            }
        }
    }

    bool show_authorize(string account_num, string input_password) {
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
        std::cout << "\033[H\033[J";
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
        /*
        // 옵션 선택
        std::string choice;
        while (true) {
            std::cout << (isEng ? "Please select an option: " : "옵션을 선택하세요: ");
            std::cin >> choice;

            if (choice == "1") {
                std::cout << (isEng ? "Bank Management selected." : "은행 관리가 선택되었습니다.") << std::endl;
                // 은행 관리 로직
                break;
            }
            else if (choice == "2") {
                std::cout << (isEng ? "ATM Management selected." : "ATM 관리가 선택되었습니다.") << std::endl;
                // ATM 관리 로직
                break;
            }
            else if (choice == "3") {
                std::cout << (isEng ? "Language settings selected." : "언어 설정이 선택되었습니다.") << std::endl;
                // 언어 설정 로직
                break;
            }
            else if (choice == "4") {
                std::cout << (isEng ? "Card insertion selected." : "카드 삽입이 선택되었습니다.") << std::endl;
                // 카드 삽입 로직
                break;
            }
            else if (choice == "5") {
                std::cout << (isEng ? "Transaction history selected." : "거래 내역이 선택되었습니다.") << std::endl;
                // 거래 내역 로직
                break;
            }
            else if (choice == "6") {
                std::cout << (isEng ? "Exiting the system." : "시스템을 종료합니다.") << std::endl;
                exit(0);
            }
            else {
                std::cout << (isEng ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도하세요.") << std::endl;
            }
        }*/
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
        printTwoBoxes(isEng ? "5. View Accounts" : "5. 계좌 목록 조회", isEng ? "6. Return to Main Menu" : "6. 메인 메뉴로 돌아가기");

        printSeparator();
    }


    void handleBankManagement() {
        while (true) {
            showBankManagementMenu();
            int choice = getIntegerInput("옵션을 선택하세요: ");
            int num_of_banks = bank_list.size();

            if (choice == 1) {
                string bank_name, bank_number;
                cout << "새로운 은행 이름을 입력하세요: ";
                cin >> bank_name;
                cout << "새로운 은행 번호를 입력하세요(4자리): ";
                cin >> bank_number;

                Bank* new_bank = new Bank(bank_name, bank_number);
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
        printMenu(isEng ? "Welcome" : "환영합니다", { isEng ? "[1]. Deposit" : "[1]. 입금", isEng ? "[2]. Withdraw" : "[2]. 출금", isEng ? "[3]. Transfer" : "[3]. 송금", isEng ? "[4]. Exit" : "[4]. 종료" });
    }

    void showDepositMenu() const {
        printMenu(isEng ? "Deposit Menu" : "입금 메뉴", { isEng ? "1. Cash Deposit" : "1. 현금 입금" ,isEng ? "2. Check Deposit" : "2. 수표 입금",isEng ? "3. Exit" : "3. 거래 취소" });
    }

    void showWithdrawMenu(int balance) const {
        clearScreen();
        printSeparator();
        printCentered(isEng ? "Withdraw Menu" : "출금 메뉴", 70);
        printSeparator();
        printCentered((isEng ? "Account Balance: " : "계좌 잔액: ") + std::to_string(balance) + (isEng ? " units." : "원"), 70);
        printCentered(isEng ? "Enter the amount to withdraw or '0' to cancel:" : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요:", 70);
        printSeparator();
    }

    void showWithdrawUI(int balance) const {
        clearScreen();
        cout << string(70, '#') << endl;
        cout << "                              출금 메뉴                              " << endl;
        cout << string(70, '#') << endl;
        cout << "계좌 잔액: " << balance << "원" << endl;
        cout << "출금할 금액을 입력하거나 '0'을 눌러 취소하세요:" << endl;
        cout << string(70, '#') << endl;
    }

    void showBillInputUI() const {
        clearScreen();
        cout << string(70, '#') << endl;
        cout << "                    출금할 지폐의 개수를 입력하세요                    " << endl;
        cout << string(70, '#') << endl;
    }

    void showWithdrawSuccessUI(int balance) const {
        clearScreen();
        cout << string(70, '#') << endl;
        cout << "                              출금 성공!                              " << endl;
        cout << string(70, '#') << endl;
        cout << "현재 잔액: " << balance << "원" << endl;
        cout << string(70, '#') << endl;
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
        printMenu(isEng ? "Error" : "오류", { isEng ? "Invalid input. Try again." : "잘못된 입력입니다. 다시 시도해주세요." });
    }

    void transactionCancelled() const {
        printMenu(isEng ? "Cancelled" : "취소됨", { isEng ? "Transaction has been cancelled." : "거래가 취소되었습니다." });
    }

    void showDepositAmount(int amount) const {
        printMenu("입금 완료", { "입금 성공!", "총 입금 금액: " + std::to_string(amount) + "원" });
    }

    void displayBalance(int balance) const {
        printMenu(isEng ? "Balance Inquiry" : "잔액 조회", { (isEng ? "Current Balance: " : "현재 잔액: ") + std::to_string(balance) + (isEng ? " units." : "원") });
    }

    void promptForCheckDeposit() const {
        //printMenu(isEng ? "Check Deposit" :"수표 입금", { isEng ? "Insert check or press '0' to cancel." : "수표를 넣거나 '0'을 눌러 취소하세요." });
        cout << "수표입금" << endl;
    }

    void requestFeePayment(int fee) const {
        printMenu("수수료 안내", { "수수료: " + std::to_string(fee) + "원", isEng ? "Pay to proceed." : "진행하려면 납부하세요." });
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
    string cardnumber;
    string transaction_type;
    int amount;
    //int num_of_transaction = 0;
    //record* transaction_records[100];

public:
    record(string id, string card, string type, int amt)
        : transactionID(id), cardnumber(card), transaction_type(type), amount(amt) {
        transactionID = id;
        cardnumber = card;
        transaction_type = type;
        amount = amt;
        //transaction_records[num_of_transaction++] = this;
    }
    string gettransactionID() { return transactionID; }
    string getcardnumber() { return cardnumber; }
    string gettransaction_type() { return transaction_type; }
    int getamount() { return amount; }
    void display_one_transaction() const {
        cout << "-------------------------------------------" << endl;
        cout << "Transaction ID: " << transactionID << endl;
        cout << "Card Number: " << cardnumber << endl;
        cout << "Transaction Type: " << transaction_type << endl;
        cout << "Amount: " << amount << endl;
        cout << "-------------------------------------------" << endl;
    }

    /*void display_all_transactions() {
        cout << "------------Transaction History------------" << endl;
        for (int i = 0; i < num_of_transaction; i++) {
            transaction_records[i]->display_one_transaction();
            cout << "-------------------------------------------" << endl;
        }
    }*/
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
    Account* account;
    Bank* bank;
    string atmNumber;
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
    ATM(Bank* atmBank, Account* userAccount, Interface* interface, bool singleBankMode = true)
        : bank(atmBank), account(userAccount), ui(interface), isSingleBankMode(singleBankMode) {
        // primaryBank 초기화
        cash[0] += 100;
        cash[1] += 50;
        cash[2] += 25;
        cash[3] += 10;
        if (atmBank == nullptr) {
            cout << "Error: atmBank is NULL. ATM cannot be initialized correctly." << endl;
            primaryBank = nullptr;
        }
        else {
            primaryBank = atmBank;
        }

        cash[0] += 100;
        cash[1] += 50;
        cash[2] += 25;
        cash[3] += 10;
    }
    string getatmNumber() { return atmNumber; }
    string getremainingcash(int i) { return to_string(cash[i]); }

    string make_atmNumber(Bank* atmBank) {
        int random_number3 = rand() % 900 + 100;
        string atmnum = account->getAccountNumber().substr(0, 4) + to_string(random_number3);
        return atmnum;
    }

    void singleBankMode(bool SingleBankMode) {
        SingleBankMode = isSingleBankMode;
    }
    void addCard(const string& cardNumber, const string& password, const string& userID, bool isAdmin) {
        cards[cardNumber] = Card(cardNumber, password, isAdmin); // 카드 등록
    }


    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    string getBankNumber() {
        return trim(bank->getBankNumber()); // 기존 코드에 trim 적용
    }
    bool insertCard() {
        cout << (ui->getLanguage() ? "Enter your card number: " : "카드 번호를 입력하세요: ");
        string cardNumber;
        cin >> cardNumber;

        if (cardNumber.empty()) {
            cout << (ui->getLanguage() ? "Error: Card number cannot be empty." : "오류: 카드 번호는 비어 있을 수 없습니다.") << endl;
            return false;
        }

        if (cardNumber.length() < 4) {
            cout << (ui->getLanguage() ? "Error: Card number too short." : "오류: 카드 번호가 너무 짧습니다.") << endl;
            return false;
        }

        string bankCode = cardNumber.substr(0, 4);
        cout << "Debug: Extracted bank code = " << bankCode << endl;

        if (cards.find(cardNumber) == cards.end()) {
            cout << "Error: Card not found." << endl;
            return false;
        }

        Card& card = cards[cardNumber];
        return authenticateUser(card);
    }


    void adminMenu() {
        while (true) {
            ui->showAdminMenu();
            int choice;
            cin >> choice;

            if (choice == 1) {
                //viewTransactionHistory(); // 거래 내역 보기
                cout << "아직 구현되지 않은 기능입니다" << endl;
            }
            else if (choice == 2) {
                cout << (ui->getLanguage() ? "Exiting admin menu. Returning to language selection.\n"
                    : "관리자 메뉴를 종료합니다. 언어 선택 화면으로 돌아갑니다.\n");
                break; // 관리자 메뉴 종료
            }
            else {
                cout << (ui->getLanguage() ? "Invalid choice. Try again.\n" : "잘못된 선택입니다. 다시 시도해 주세요.\n");
            }
        }
    }

    void userMenu() {
        while (true) {
            ui->showUserMenu();
            int selection;
            cin >> selection;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (ui->getLanguage() ? "Invalid input. Please try again.\n" : "잘못된 입력입니다. 다시 시도해 주세요.\n");
                continue;
            }

            if (selection == 1) {
                bool success = deposit();
                if (!success) continue; // 실패 시 메뉴로 돌아가기
            }
            else if (selection == 2) {
                bool success = withdraw();
                if (!success) continue; // 실패 시 메뉴로 돌아가기
            }
            else if (selection == 3) {
                bool success = transfer();
                if (!success) continue; // 실패 시 메뉴로 돌아가기
            }
            else if (selection == 4) {
                cout << (ui->getLanguage() ? "Exiting ATM. Please take your card.\n" : "ATM을 종료합니다. 카드를 가져가세요.\n");
                break; // 사용자 메뉴 종료
            }
            else {
                cout << (ui->getLanguage() ? "Invalid option. Please try again.\n" : "잘못된 선택입니다. 다시 시도해 주세요.\n");
            }
        }
    }

    // 사용자 인증
    bool authenticateUser(Card& card) {
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
    // 현재 ATM 모드 표시
    void displayATMModes() {
        cout << (ui->getLanguage()
            ? "Current ATM Mode: " + string(isSingleBankMode ? "Single Bank ATM" : "Multi Bank ATM") + "\n"
            : "현재 ATM 모드: " + string(isSingleBankMode ? "단일 은행 ATM" : "다중 은행 ATM") + "\n");
    }
    bool validateCardForSingleBank(const string& cardNumber) {
        string bankCode = cardNumber.substr(0, 4); // 카드 번호 앞 4자리
        return bankCode == bank->getBankNumber();
    }
    bool isInteger(const std::string& input) { //string type이 integer인지 확인
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

    bool isPositive(int input) {
        if (input < 0) {
            cout << (ui->getLanguage() ? "Negative numbers cannot be entered.\nPlease try again." : "음수는 입력할 수 없습니다.\n다시 시도해 주세요.") << endl;
            return false;
        }
        return true;
    }

    bool hasSufficientCash(int amount) {
        int totalCash = cash[0] * 1000 + cash[1] * 5000 + cash[2] * 10000 + cash[3] * 50000;
        return totalCash >= amount;
    }

    void dispenseCash(int amount) {
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

    void reduceCash(int amount) {
        dispenseCash(amount);
    }

    bool card_verification(Account* card) { // card 비밀번호 인증
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



    int getValidInput(const string& prompt, Interface* ui) {
        int value;
        while (true) {
            cout << prompt;
            cin >> value;

            if (cin.fail() || value < 0 || value > numeric_limits<int>::max()) {
                cin.clear(); // 스트림 상태 초기화
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
                cout << (ui->getLanguage() ? "Wrong Input. Try Again." : "잘못된 입력. 다시 시도.") << endl;
            }
            else {
                return value; // 유효한 입력값 반환
            }
        }
    }

    void depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4) {
        int totalBills = 0;

        // 각 지폐 입력 및 검증
        m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: "), ui);
        totalBills += m1;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Exceeded maximum number of bills. Transaction cancelled." : "지폐 개수 제한(50장)을 초과했습니다. 거래가 취소됩니다.") << endl;
            throw runtime_error("Exceeded maximum number of bills"); // 거래 취소
        }

        m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: "), ui);
        totalBills += m2;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Exceeded maximum number of bills. Transaction cancelled." : "지폐 개수 제한(50장)을 초과했습니다. 거래가 취소됩니다.") << endl;
            throw runtime_error("Exceeded maximum number of bills"); // 거래 취소
        }

        m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: "), ui);
        totalBills += m3;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Exceeded maximum number of bills. Transaction cancelled." : "지폐 개수 제한(50장)을 초과했습니다. 거래가 취소됩니다.") << endl;
            throw runtime_error("Exceeded maximum number of bills"); // 거래 취소
        }

        m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: "), ui);
        totalBills += m4;
        if (totalBills > 50) {
            cout << (ui->getLanguage() ? "Exceeded maximum number of bills. Transaction cancelled." : "지폐 개수 제한(50장)을 초과했습니다. 거래가 취소됩니다.") << endl;
            throw runtime_error("Exceeded maximum number of bills"); // 거래 취소
        }
    }

    bool deposit() {
        int input;
        cout << "Debug: Extracting substring for card bank..." << endl;
        string accountNumber = account->getAccountNumber();
        if (accountNumber.length() < 4) {
            cout << "Error: Account number too short. Length: " << accountNumber.length() << endl;
            throw std::length_error("Account number length is less than 4.");
        }
        string cardBank = accountNumber.substr(0, 4);
        cout << "Debug: Extracted card bank: " << cardBank << endl;

        while (true) {
            ui->showDepositMenu();
            input = getValidInput((ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: "), ui);

            if (input == 1) { // Cash Deposit
                int m1, m2, m3, m4, depositAmount;

                try {
                    depositCash(ui, m1, m2, m3, m4); // Handle exception if max bills exceeded
                }
                catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl; // Log exception details
                    ui->transactionCancelled();
                    return false;
                }

                depositAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;

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
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
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
                ui->showDepositAmount(checkAmount);
                processTransaction(checkAmount, cardBank);
                return true;
            }
            else if (input == 3) { // Cancel Transaction
                ui->transactionCancelled();
                return false;
            }
            else {
                ui->showErrorMessage();
            }
        }
    }
    bool fee_cash_calculator(int fee) {
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
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
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
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
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
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
                if (!change_ATM_dec(change)) {
                    return false;  // ATM에 충분한 현금이 없을 경우
                }
            }
            return true;
        }

        cout << (ui->getLanguage() ? "Failure to pay fees: Insufficient cash" : "수수료 납부 실패: 현금 부족") << endl;
        return false;
    }


    bool change_ATM_dec(int change) { // 거스름돈 줄 때 ATM 가용 현금 감소
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
                cout << "=======================================" << endl;
                return false;
            }
        }
        for (int k = 0; k < 4; k++) {
            cash[k] -= num_cash[k];
        }
        return true;
    }

    bool fee_account_calculator(int fee) { // 계좌에서 수수료 계산
        ui->requestFeePayment(fee);
        cout << (ui->getLanguage() ? "Fee: " : "수수료: ") << fee << endl;
        cout << (ui->getLanguage() ? "Paying a fee from your account..." : "계좌에서 수수료를 지불 중...") << endl;
        double balance = account->getAvailableFund();
        if (balance >= fee) {
            account->decrease_account_balance(fee);
            cout << (ui->getLanguage() ? "Successfully Fee paid!" : "수수료 납부 성공!") << endl;
            cout << "=======================================" << endl;
            return true;
            //이 수수료는 어디로 가는지??
        }
        else {
            cout << (ui->getLanguage() ? "Failed to pay fee: Insufficient account balance" : "수수료 납부 실패: 계좌 잔액 부족") << endl;
            cout << "=======================================" << endl;
            return false;
        }
    }

    bool withdraw() {
        int withdrawAmount;
        string cardBank = account->getAccountNumber().substr(0, 4); // 카드의 은행 코드 확인
        int withdrawalCount = 0; // 세션 당 인출 횟수 제한
        if (not card_verification(account)) return false;

        // 한 세션당 최대 3번 출금
        while (withdrawalCount < 3) {
            cout << (ui->getLanguage() ? "\n=== Withdraw Menu ===\nAccount Balance: " : "\n=== 출금 메뉴 ===\n계좌 잔액: ") << account->getAvailableFund() << "won\n";
            cout << (ui->getLanguage() ? "Enter amount to withdraw or '0' to cancel: " : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요: ");
            while (true) {
                cin >> withdrawAmount;
                if (isPositive(withdrawAmount)) break;
            }

            // 출금 취소 처리
            if (withdrawAmount == 0) {
                ui->transactionCancelled();
                return false;
            }

            // 출금 금액 한도 및 계좌 잔액 부족 오류 처리
            if (withdrawAmount > 500000) {
                cout << (ui->getLanguage() ? "Error: Transaction limit is 500,000 won per withdrawal." : "오류: 거래 한도는 출금 당 500,000원입니다.") << endl;
                continue;
            }
            if (withdrawAmount > account->getAvailableFund()) {
                cout << (ui->getLanguage() ? "Error: Insufficient account balance." : "오류: 계좌 잔액이 부족합니다.") << endl;
                return false; // 계좌 잔액 부족 시 종료
            }

            // ATM 현금 부족 오류 처리
            if (not hasSufficientCash(withdrawAmount)) {
                cout << (ui->getLanguage() ? "Error: ATM has insufficient cash." : "오류: ATM에 충분한 현금이 없습니다.") << endl;
                return false; // ATM 현금 부족 시 종료
            }

            int fee = (cardBank == bank->getBankName()) ? 1000 : 2000;
            if (account->getAvailableFund() < withdrawAmount + fee) {
                cout << (ui->getLanguage() ? "Error: Insufficient balance to cover the fee." : "오류: 수수료를 포함한 잔액이 부족합니다.") << endl;
                return false;
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


    bool transfer() {
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
                    cout << "=======================================" << endl;

                    if (deposited < transferAmount) { // 부족한 돈을 넣은 경우
                        cout << (ui->getLanguage() ? "Transaction failed: Insufficient cash" : "거래 실패: 현금 부족") << endl;
                        cout << "=======================================" << endl;
                        break; //TransferOption으로
                    }
                    else if (deposited > transferAmount) { // 초과된 돈을 넣은 경우
                        cout << (ui->getLanguage() ? "Transfer amount: " : "송금 금액: ") << transferAmount << endl;
                        cout << (ui->getLanguage() ? "Amount entered: " : "투입된 금액: ") << deposited << endl << endl;
                        cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << deposited - transferAmount << endl;
                        cout << "=======================================" << endl;
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
                    cout << "=======================================" << endl;
                    // welcome으로
                    return false;
                }
            }
            else if (input == 2) { //계좌 송금
                string cardBank = account->getAccountNumber().substr(0, 4);
                string ATMBank = bank->getBankNumber();

                // card 검증 단계
                if (not card_verification(account)) return false;
                cout << "=======================================" << endl;

                cout << (ui->getLanguage() ? "Please enter the account to transfer or '0' to cancel." : "송금할 계좌를 입력하거나 '0'을 눌러 취소하세요.") << endl;
                string account_num;
                cin >> account_num;
                if (not isInteger(account_num)) return false;
                if (account_num == "0") {
                    ui->transactionCancelled();
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
                    cout << "=======================================" << endl;
                    if (transferAmount == 0) {
                        ui->transactionCancelled();
                        return false;
                    }
                    if (account->getAvailableFund() < transferAmount) { // 계좌에 돈이 부족한 경우
                        cout << (ui->getLanguage() ? "Transaction failed: Insufficient account balance" : "거래 실패: 계좌 잔액 부족") << endl;
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
                            cout << "=======================================" << endl;
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
                            cout << "=======================================" << endl;
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
                            cout << "=======================================" << endl;
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
                    cout << "=======================================" << endl;
                    cout << (ui->getLanguage() ? "Transaction failed: Non-existent account" : "거래 실패: 존재하지 않는 계좌") << endl;
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
    }


    void processTransaction(int depositAmount, const string& cardBank) {
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
    ///////거래 아이디 생성 추가
    string transactionid() {
        return account->getAccountNumber().substr(0, 4) + to_string(++transaction_order);
    }

    ///////거래 내역 기록
    void transaction_recording(string transaction_type, int amount) {
        record* recording = new record(this->transactionid(), account->getCardNumber(), transaction_type, amount);
        transaction_records[num_of_transaction++] = recording;
    }


    ///////////transaction 보여주는 함수 추가
    void display_history(string card_number) {
        if (card_number == "admin") {
            cout << "Transaction History Inquiry : Would you like to make an inquiry?\n거래 기록 조회 : 조회하시겠습니까?" << endl;
            cout << "1. Yes\n2. no" << endl;
            string select;
            cin >> select;
            if (select == "1") {
                cout << "************Transaction History************" << endl;
                for (int i = 0; i < num_of_transaction; i++) {
                    transaction_records[i]->display_one_transaction();
                }

                cout << "Would you like to print it?" << endl;
                cout << "1. Yes\n2. no" << endl;
                string print;
                cin >> print;
                if (print == "1") {

                    ofstream file("TransactionHistory.txt");

                    if (!file) {
                        cerr << "Error: Unable to create file." << endl;
                        return;
                    }

                    file << "************Transaction History************" << endl;

                    for (int i = 0; i < num_of_transaction; i++) {

                        file << "-------------------------------------------" << endl;
                        file << "Transaction ID: " << transaction_records[i]->gettransactionID() << endl;
                        file << "Card Number: " << transaction_records[i]->getcardnumber() << endl;
                        file << "Transaction Type: " << transaction_records[i]->gettransaction_type() << endl;
                        file << "Amount: " << transaction_records[i]->getamount() << endl;
                        file << "-------------------------------------------" << endl;
                    }
                    file.close();
                }
                else if (print == "2") {
                    cout << "Return to Main" << endl;
                }
            }
            else if (select == "2") {
                cout << "Return to Main" << endl;
            }

        }
        else if (card_number == account->getCardNumber()) {
            transaction_records[num_of_transaction - 1]->display_one_transaction();
        }
    }

};

void display_atm() {
    for (int i = 0; i < num_of_ATM; i++) {
        cout << "ATM[SN:" << atm_list[i]->getatmNumber() << "] remaining cash : {KRW 50000 : " << atm_list[i]->getremainingcash(3) << ", KRW 10000 : " << atm_list[i]->getremainingcash(2) << ", KRW 5000 : " << atm_list[i]->getremainingcash(1)
            << ", KRW 1000 : " << atm_list[i]->getremainingcash(0) << "} " << endl;
    }
}


void display_account() {
    for (int i = 0; i < num_of_banks; i++) {
        for (int j = 0; j < bank_list[i]->getNumOfAccounts(); j++) {

            cout << "Account[Bank:" << bank_list[i]->getBankName() << ", No : " << bank_list[i]->access_to_account(j)->getAccountNumber() << ", Owner : " << bank_list[i]->access_to_account(j)->getOwnerName() << "]"
                << " balance: " << bank_list[i]->access_to_account(j)->getAvailableFund() << endl;
        }
    }
}

int main() {
    // 은행 및 계정 초기화
    Interface ui;
    Bank myBank("kakao", "3333");
    Account* myAccount = myBank.make_account("K", "kakao", 10000, "0000");
    ATM atm(&myBank, myAccount, &ui);
    vector<Bank*> bank_list; // 여러 은행을 저장할 벡터

    // 프로그램 루프
    while (true) {
        // 초기 화면 출력 및 옵션 선택
        ui.showStartScreen();
        int startSelection = getIntegerInput(ui.getLanguage() ? "Please select an option: " : "옵션 선택: ");

        if (startSelection == 1) {  // 은행 관리
            ui.showTransitionMessage(ui.getLanguage() ? "Moving to Bank Management Menu..." : "은행 관리 메뉴로 이동 중...");
            while (true) {
                ui.showBankManagementMenu();
                int bankSelection = getIntegerInput(ui.getLanguage() ? "Please select an option: " : "옵션 선택: ");

                if (bankSelection == 1) {
                    // 은행 추가
                    string bankName, bankCode;
                    cout << (ui.getLanguage() ? "Enter the bank name: " : "은행 이름을 입력하세요: ");
                    cin >> bankName;
                    cout << (ui.getLanguage() ? "Enter the bank code: " : "은행 코드를 입력하세요: ");
                    cin >> bankCode;
                    if (bankCode.length() != 4) {
                        cout << "Error: Bank code must be 4 digits." << endl;
                        continue;
                    }
                    bank_list.push_back(new Bank(bankName, bankCode));
                    cout << (ui.getLanguage() ? "Bank added successfully.\n" : "은행이 성공적으로 추가되었습니다.\n");
                }
                else if (bankSelection == 2) {
                    // 은행 삭제
                    string bankCode;
                    cout << (ui.getLanguage() ? "Enter the bank code to delete: " : "삭제할 은행 코드를 입력하세요: ");
                    cin >> bankCode;

                    bool found = false;
                    for (auto it = bank_list.begin(); it != bank_list.end(); ++it) {
                        if ((*it)->getBankNumber() == bankCode) {
                            cout << "Deleting bank: " << (*it)->getBankNumber() << endl;
                            delete* it;
                            bank_list.erase(it);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        cout << (ui.getLanguage() ? "Error: Bank not found.\n" : "오류: 해당 은행을 찾을 수 없습니다.\n");
                    }
                }
                else if (bankSelection == 3) { // 계좌 생성
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available. Please add a bank first.\n" : "등록된 은행이 없습니다. 먼저 은행을 추가해주세요.\n");
                        continue;
                    }

                    cout << (ui.getLanguage() ? "Select a bank to create an account:\n" : "계좌를 생성할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }

                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ");
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        continue;
                    }

                    Bank* selectedBank = bank_list[bankChoice - 1];
                    string ownerName, password;
                    double initialBalance;

                    cout << (ui.getLanguage() ? "Enter the owner name: " : "계좌 소유자의 이름을 입력하세요: ");
                    cin >> ownerName;
                    initialBalance = getIntegerInput(ui.getLanguage() ? "Enter the initial balance: " : "초기 잔액을 입력하세요: ");
                    cout << (ui.getLanguage() ? "Enter the password: " : "비밀번호를 입력하세요: ");
                    cin >> password;

                    selectedBank->make_account(ownerName, selectedBank->getBankName(), initialBalance, password);
                    cout << (ui.getLanguage() ? "Account created successfully.\n" : "계좌가 성공적으로 생성되었습니다.\n");
                }
                else if (bankSelection == 4) { // 계좌 삭제
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available.\n" : "등록된 은행이 없습니다.\n");
                        continue;
                    }

                    cout << (ui.getLanguage() ? "Select a bank to delete an account:\n" : "계좌를 삭제할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }

                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ");
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        continue;
                    }

                    Bank* selectedBank = bank_list[bankChoice - 1];
                    string accountNumber;

                    cout << (ui.getLanguage() ? "Enter the account number to delete: " : "삭제할 계좌 번호를 입력하세요: ");
                    cin >> accountNumber;

                    if (selectedBank->deleteAccount(accountNumber)) {
                        cout << (ui.getLanguage() ? "Account removed successfully.\n" : "계좌가 성공적으로 삭제되었습니다.\n");
                    }
                    else {
                        cout << (ui.getLanguage() ? "Error: Account number not found.\n" : "오류: 해당 계좌 번호를 찾을 수 없습니다.\n");
                    }
                }
                else if (bankSelection == 5) { // 계좌 목록 조회
                    if (bank_list.empty()) {
                        cout << (ui.getLanguage() ? "No banks available.\n" : "등록된 은행이 없습니다.\n");
                        continue;
                    }

                    cout << (ui.getLanguage() ? "Select a bank to view accounts:\n" : "계좌 목록을 조회할 은행을 선택하세요:\n");
                    for (size_t i = 0; i < bank_list.size(); ++i) {
                        cout << i + 1 << ". " << bank_list[i]->getBankName() << " (" << bank_list[i]->getBankNumber() << ")\n";
                    }

                    int bankChoice = getIntegerInput(ui.getLanguage() ? "Enter the bank number: " : "은행 번호를 입력하세요: ");
                    if (bankChoice < 1 || bankChoice > static_cast<int>(bank_list.size())) {
                        cout << (ui.getLanguage() ? "Invalid bank selection.\n" : "잘못된 은행 선택입니다.\n");
                        continue;
                    }

                    Bank* selectedBank = bank_list[bankChoice - 1];
                    ui.clearScreen();
                    selectedBank->listAccounts();
                    cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                }
                else if (bankSelection == 6) {
                    // 메인 메뉴로 돌아가기
                    ui.showTransitionMessage(ui.getLanguage() ? "Returning to Main Menu..." : "메인 메뉴로 돌아갑니다...");
                    break;
                }
                else {
                    // 잘못된 입력 처리
                    cout << (ui.getLanguage() ? "Invalid option. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
                }
            }
        }
        else if (startSelection == 3) {  // 언어 설정
            while (true) {
                ui.showLanguageSettingsMenu();
                int languageSelection = getIntegerInput(ui.getLanguage() ? "Please select a language: " : "언어를 선택하세요: ");
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
        else if (startSelection == 4) { // 카드 삽입
            ui.clearScreen(); // 이전 화면 제거
            ui.showTransitionMessage(ui.getLanguage() ? "Insert your card." : "카드를 삽입하세요.");

            while (true) {
                string cardNumber;
                cout << (ui.getLanguage() ? "Enter your card number (or '0' to return): " : "카드 번호를 입력하세요 (0을 입력하면 메인 메뉴로 돌아갑니다): ");
                cin >> cardNumber;

                // 초기화면으로 돌아가기
                if (cardNumber == "0") {
                    cout << (ui.getLanguage() ? "Returning to main menu..." : "메인 메뉴로 돌아갑니다...") << endl;
                    break; // 초기화면으로 돌아감
                }

                // 카드 번호 유효성 검사
                if (cardNumber.empty() || cardNumber.length() != 12 || !std::all_of(cardNumber.begin(), cardNumber.end(), ::isdigit)) {
                    cout << (ui.getLanguage() ? "Invalid card number. Please try again.\n" : "유효하지 않은 카드 번호입니다. 다시 시도하세요.\n");
                    continue; // 입력을 다시 받음
                }

                // 은행 및 계좌 탐색
                Bank* targetBank = nullptr;
                Account* targetAccount = nullptr;

                // 1. 은행 검색
                for (auto bank : bank_list) {
                    if (bank->getBankNumber() == cardNumber.substr(0, 4)) { // 앞 4자리 은행 번호 확인
                        targetBank = bank;
                        break;
                    }
                }

                if (!targetBank) {
                    cout << (ui.getLanguage() ? "Bank associated with card not found. Please try again.\n" : "카드와 연결된 은행을 찾을 수 없습니다. 다시 시도하세요.\n");
                    continue; // 입력을 다시 받음
                }

                // 2. 계좌 검색
                for (int i = 0; i < targetBank->getNumOfAccounts(); ++i) {
                    Account* account = targetBank->getAccount(i);
                    if (account && account->getCardNumber() == cardNumber) {
                        targetAccount = account;
                        break;
                    }
                }

                if (!targetAccount) {
                    cout << (ui.getLanguage() ? "Account associated with card not found. Please try again.\n" : "카드와 연결된 계좌를 찾을 수 없습니다. 다시 시도하세요.\n");
                    continue; // 입력을 다시 받음
                }

                string password;
                cout << (ui.getLanguage() ? "Enter your password: " : "비밀번호를 입력하세요: ");
                cin >> password;

                // 비밀번호 확인
                if (targetAccount->getPassword() != password) {
                    cout << (ui.getLanguage() ? "Incorrect password. Please try again.\n" : "비밀번호가 틀렸습니다. 다시 시도하세요.\n");
                    continue; // 입력을 다시 받음
                }

                // 인증 성공 시 사용자 메뉴 호출
                ui.clearScreen(); // 이전 화면 제거
                cout << (ui.getLanguage() ? "Card authentication successful." : "카드 인증 성공.") << endl;

                // 사용자 메뉴 출력
                while (true) {
                    ui.showWelcomeMessage();
                    string userOption;
                    cin >> userOption;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        ui.showErrorMessage();
                        continue;
                    }

                    if (userOption == "1") {
                        // 입금 로직
                        bool success = atm.deposit();
                        if (!success) continue; // Welcome 화면으로 돌아가기
                    }
                    else if (userOption == "2") {
                        // 출금 로직
                        bool success = atm.withdraw();
                        if (!success) continue;
                    }
                    else if (userOption == "3") {
                        // 송금 로직
                        bool success = atm.transfer();
                        if (!success) continue;
                    }
                    else if (userOption == "4") {
                        cout << (ui.getLanguage() ? "Exiting to main menu." : "메인 메뉴로 돌아갑니다.") << endl;
                        break; // 사용자 메뉴 종료
                    }
                    else if (userOption == "/") {
                        // 트랜잭션 히스토리 출력
                        atm.display_history("admin");
                        continue;
                    }
                    else if (userOption == "admin") {
                        // 관리자 메뉴로 이동
                        cout << (ui.getLanguage() ? "Moving to admin menu..." : "관리자 메뉴로 이동합니다.") << endl;
                        continue;
                    }
                    else {
                        cout << (ui.getLanguage() ? "Invalid option. Try again." : "잘못된 입력입니다. 다시 시도하세요.") << endl;
                    }
                }
                break; // 카드 삽입 루프 종료
            }
        }
        else if (startSelection == 5) {  // 거래 내역 보기
            ui.clearScreen();
            cout << (ui.getLanguage() ? "Displaying transaction history..." : "거래 내역을 표시합니다...") << endl;
            atm.display_history(myAccount->getCardNumber());  // 해당 계좌의 거래 내역 출력
            cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
        }
        else if (startSelection == 6) {  // 종료
            ui.showTransitionMessage(ui.getLanguage() ? "Exiting system. Goodbye!" : "시스템을 종료합니다. 안녕히 가세요!");
            break;
        }
        else {
            cout << (ui.getLanguage() ? "Invalid input. Try again.\n" : "잘못된 입력입니다. 다시 시도하세요.\n");
        }
    }

    return 0;
}
