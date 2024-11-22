#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace std;

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define WHITE "\033[97m"
#define BG_DARK "\033[48;5;235m"
#define BG_LIGHT "\033[48;5;248m"
#define ACCENT_BLUE "\033[38;5;81m"
#define ACCENT_GREEN "\033[38;5;76m"
#define ACCENT_RED "\033[38;5;160m"


class Account;
class Bank;
class ATM;
class Interface;
class card;

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
    int num_of_accounts = 0;
    Account* accounts[100];
    string history[100];
    int num_of_transactions = 0;

public:
    Bank(string bankname, string banknumber)
        : bank_name(bankname), bank_number(banknumber)
    {
        string bank_name = bankname;
        string bank_number = banknumber;
        bank_list[num_of_banks++] = this;
    }
    ~Bank() { delete this; }
    int fee_calculator_deposit(string input_cardbank); //deposit, transfer, withdraw 각각 설정
    string getBankName() { return bank_name; }
    string getBankNumber() { return bank_number; }
    Account* make_account(string new_owner_name, string bankname, double initial_balance, string password) {
        string new_bank_number;
        string new_card_number;
        for (int i = 0; i < num_of_banks; i++) {
            if (bank_list[i]->getBankName() == bankname) {
                new_bank_number = bank_list[i]->getBankNumber();
                new_card_number = bank_list[i]->getBankNumber();
                break;
            }
        }
        srand(static_cast<unsigned int>(time(0)));
        int random_number1 = rand() % 9000000 + 1000000;
        int random_number2 = rand() % 900000000 + 100000000;
        new_bank_number += to_string(random_number1);
        new_card_number += to_string(random_number2);
        Account* new_account = new Account(new_bank_number, new_owner_name, new_card_number, initial_balance, this, password);
        accounts[num_of_accounts++] = new_account;

        // Test하기 편하라고 넣어 놨습니다
        cout << "계좌가 개설되었습니다." << endl;
        cout << " - 이름: " << new_owner_name << endl;
        cout << " - 은행 명: " << bankname << endl;
        cout << " - 새 계좌 번호: " << new_account->getAccountNumber() << endl;
        cout << " - 새 카드 번호: " << new_account->getCardNumber() << endl;
        cout << " - 새 카드 비밀번호: " << new_account->getPassword() << endl << endl;

        return new_account;
    }
    /*Bank(string name) : bank_name(name) {}
    int calculateFee(const string& cardBank) const {
        return (cardBank == bank_name) ? 1000 : 2000;
    }*/

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

    // 화면 초기화
    void clearScreen() const {
        cout << "\033[H\033[J"; // 커서를 맨 위로 이동하고 화면을 지움
    }

    // 중앙 정렬된 텍스트 출력
    void printCentered(const string& text, int width = 50) const {
        int padding = (width - text.length()) / 2;
        cout << string(padding, ' ') << text << endl;
    }

    // 구분선 출력
    void printSeparator(int width = 50) const {
        cout << string(width, '-') << endl;
    }

    // 좌우 정렬된 메뉴 출력
    void printMenuOption(const string& left, const string& right, int width = 50) const {
        int padding = (width - left.length() - right.length()) / 2;
        cout << string(padding, ' ') << left << string(padding, ' ') << right << endl;
    }

public:
    Interface(bool lang = true) : isEng(lang) {}

    void setLanguage(bool lang) {
        isEng = lang;
    }

    bool getLanguage() const {
        return isEng;
    }

    // 메인 메뉴
    void showWelcomeMessage() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Welcome to Your ATM" : "환영합니다");
        printSeparator();
        printMenuOption(isEng ? "1. Deposit" : "1. 입금", isEng ? "2. Withdraw" : "2. 출금");
        printSeparator();
        printMenuOption(isEng ? "3. Transfer" : "3. 송금", isEng ? "4. Exit" : "4. 종료");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
    }

    // 입금 메뉴
    void showDepositOptions() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "=== Deposit Menu ===" : "=== 입금 메뉴 ===");
        printSeparator();
        printCentered(isEng ? "1. Cash Deposit" : "1. 현금 입금");
        printCentered(isEng ? "2. Check Deposit" : "2. 수표 입금");
        printCentered(isEng ? "3. Cancel Transaction" : "3. 거래 취소");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
    }

    // 출금 메뉴
    void showWithdrawOptions() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "=== Withdraw Menu ===" : "=== 출금 메뉴 ===");
        printSeparator();
        printCentered(isEng ? "1. cash withdraw" : "1. 현금 출금");
        printCentered(isEng ? "2. check withdraw" : "2. 수표 출금");
        printCentered(isEng ? "3. Cancel" : "3. 취소");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
    }

    // 송금 메뉴
    void showTransferOptions() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "=== Transfer Menu ===" : "=== 송금 메뉴 ===");
        printSeparator();
        printCentered(isEng ? "1. Cash Transfer" : "1. 현금 송금");
        printCentered(isEng ? "2. Account Transfer" : "2. 계좌 송금");
        printCentered(isEng ? "3. Cancel" : "3. 취소");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
    }

    // 에러 메시지
    void showErrorMessage() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도해주세요.");
        printSeparator();
    }

    // 거래 취소 메시지
    void transactionCancelled() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Transaction cancelled." : "거래가 취소되었습니다.");
        printSeparator();
    }

    // 입금 금액 표시
    void showDepositAmount(int amount) const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Deposit successful!" : "입금 성공!");
        printCentered(isEng ? "Total deposit amount is " + to_string(amount) + " units." :
                              "총 입금 금액은 " + to_string(amount) + "원입니다.");
        printSeparator();
    }

    // 잔액 표시
    void displayBalance(int balance) const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Your current balance is " + to_string(balance) + " units." :
                              "현재 잔액은 " + to_string(balance) + "원입니다.");
        printSeparator();
    }

    // 수표 입금 메시지
    void promptForCheckDeposit() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Please insert your check or enter '0' to cancel." :
                              "수표를 넣어주세요 또는 '0'을 입력하여 취소하세요.");
        printSeparator();
    }

    // 수수료 요청
    void requestFeePayment(int fee) const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "A fee of " + to_string(fee) + " units has been charged." :
                              "수수료 " + to_string(fee) + "원이 부과되었습니다.");
        printCentered(isEng ? "Please pay the fee to proceed." : "진행하려면 수수료를 납부하세요.");
        printSeparator();
    }
        // 사용자 메뉴
    void showUserMenu() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "Welcome to Your ATM" : "환영합니다");
        printSeparator();
        printMenuOption(isEng ? "1. Deposit" : "1. 입금", isEng ? "2. Withdraw" : "2. 출금");
        printSeparator();
        printMenuOption(isEng ? "3. Transfer" : "3. 송금", isEng ? "4. Exit" : "4. 종료");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
    }

    // 관리자 메뉴
    void showAdminMenu() const {
        clearScreen(); // 화면 초기화
        printSeparator();
        printCentered(isEng ? "=== Admin Menu ===" : "=== 관리자 메뉴 ===");
        printSeparator();
        printCentered(isEng ? "1. View Transaction History" : "1. 거래 내역 보기");
        printCentered(isEng ? "2. Exit" : "2. 종료");
        printSeparator();
        printCentered(isEng ? "Please select an option:" : "옵션을 선택하세요:");
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
        cout << (ui->getLanguage() ? "Please insert your card (enter card number): " : "카드를 삽입하세요 (카드 번호 입력): ");
        string cardNumber;
        cin >> cardNumber;
        cout << "Debug: Entered card number = " << cardNumber << endl;

        if (cards.find(cardNumber) == cards.end()) {
            cout << (ui->getLanguage() ? "Invalid card. Please try again.\n" : "잘못된 카드입니다. 다시 시도해 주세요.\n");
            return false;
        }
        cout << "Debug: Card found in the system." << endl;

        string bankCode = cardNumber.substr(0, 4);
        cout << "Debug: Extracted bank code = " << bankCode << endl;

        if (isSingleBankMode) {
            if (primaryBank == nullptr) {
                cout << "Debug: primaryBank is NULL. Please check initialization." << endl;
                return false;
            }

            string primaryBankNumber = primaryBank->getBankNumber();
            cout << "Debug: primaryBank number = " << primaryBankNumber << endl;

            if (bankCode != primaryBankNumber) {
                cout << (ui->getLanguage() ? "Card not issued by primary bank.\n" : "주 은행에서 발행된 카드가 아닙니다.\n");
                return false;
            }
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
        string cardBank = account->getAccountNumber().substr(0, 4);

        while (true) {
            ui->showDepositOptions();
            input = getValidInput((ui->getLanguage() ? "Select an option: " : "옵션을 선택하세요: "), ui);

            if (input == 1) { // 현금 예금
                int m1, m2, m3, m4, depositAmount;

                try {
                    depositCash(ui, m1, m2, m3, m4); // 예외 처리
                }
                catch (const runtime_error& e) {
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
            else if (input == 3) { // 거래 취소
                ui->transactionCancelled();
                return false;
            }
            else {
                ui->showErrorMessage();
            }
        }
    }

    bool fee_cash_calculator(int fee) { // 현금으로 수수료 계산 + ATM 현금 감소
        ui->requestFeePayment(fee);

        int m1, m2, m3, m4;
        int total = 0; // 총 투입된 금액
        int depositAmount = 0; // 총 투입된 금액 계산

        // 1,000원 지폐 수 입력
        cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
        cin >> m1;
        total += m1 * 1000; // 1,000원 지폐 금액 계산
        depositAmount += m1 * 1000;

        if (total >= fee) {
            cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
                if (!change_ATM_dec(change)) {
                    return false;
                }
            }
            return true; // 수수료 납부 성공 후 메인 메뉴로 돌아감
        }

        // 5,000원 지폐 수 입력
        cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
        cin >> m2;
        total += m2 * 5000;
        depositAmount += m2 * 5000;

        if (total >= fee) {
            cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
                if (!change_ATM_dec(change)) {
                    return false;
                }
            }
            return true; // 수수료 납부 성공 후 메인 메뉴로 돌아감
        }

        // 10,000원 지폐 수 입력
        cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
        cin >> m3;
        total += m3 * 10000;
        depositAmount += m3 * 10000;

        if (total >= fee) {
            cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
                if (!change_ATM_dec(change)) {
                    return false;
                }
            }
            return true; // 수수료 납부 성공 후 메인 메뉴로 돌아감
        }

        // 50,000원 지폐 수 입력
        cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
        cin >> m4;
        total += m4 * 50000;
        depositAmount += m4 * 50000;

        if (total >= fee) {
            cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료 납부 성공") << endl;
            if (total > fee) {
                int change = total - fee;
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << change << "원" << endl;
                if (!change_ATM_dec(change)) {
                    return false;
                }
            }
            return true; // 수수료 납부 성공 후 메인 메뉴로 돌아감
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
                    }
                    else { // 수수료 실패: 다시 돈 빼기
                        cash[0] -= m1;
                        cash[1] -= m2;
                        cash[2] -= m3;
                        cash[3] -= m4;
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
        } return false;
    }

    void processTransaction(int depositAmount, const string& cardBank) {
        int fee = bank->calculateFee(cardBank);

        fee_cash_calculator(fee);
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
        if (card_number == "/") {
            cout << "------------Transaction History------------" << endl;
            for (int i = 0; i < num_of_transaction; i++) {
                transaction_records[i]->display_one_transaction();
                cout << "-------------------------------------------" << endl;
            }
        }
        else if (card_number == account->getCardNumber()) {
            transaction_records[num_of_transaction]->display_one_transaction();
        }
    }
};

int main() {

    Bank myBank("kakao", "1111");
    Account* Acc1 = myBank.make_account("Ben", "kakao", 30000, "1234");
    cout << "[나의 계좌]" << endl;
    Account* myAccount = myBank.make_account("K", "kakao", 10000, "2345");
    Interface ui;
    bool singleBankMode = true;  // 단일 은행 모드로 설정

    ATM atm(&myBank, myAccount, &ui, singleBankMode);

    atm.addCard(Acc1->getCardNumber(), "1234", "User1", true);  // 일반 사용자 카드
    atm.addCard("111122223333", "admin", "Admin", true); // 관리자 카드

    while (true) {
        cout << "Please select your preferred language.\n원하시는 언어를 선택해 주세요.\n" << endl;
        cout << "1. English" << endl << "2. 한국어" << endl;
        int n;
        cin >> n;
        if (n == 1) {
            ui.setLanguage(true);
            break;
        }
        else if (n == 2) {
            ui.setLanguage(false);
            break;
        }
        cout << "Invalid input. Please try again.\n잘못된 입력입니다. 다시 시도해 주세요.\n" << endl;
    }

    // 카드 삽입 및 인증
    while (true) {
        if (!atm.insertCard()) {
            cout << (ui.getLanguage() ? "Failed to insert card. Restarting.\n" : "카드 삽입에 실패했습니다. 다시 시도해주세요.\n");
            continue;
        }

        while (true) {
            ui.showWelcomeMessage();
            cout << "> ";

            int selection;
            cin >> selection;
            if (cin.fail() || selection < 1 || selection > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice. Try again." << endl;
                this_thread::sleep_for(chrono::seconds(1)); // 1초 대기
                continue;
            }

            bool success = false;
            if (selection == 1) {
                success = atm.deposit();
            }
            else if (selection == 2) {
                success = atm.withdraw();
            }
            else if (selection == 3) {
                success = atm.transfer();
            }
            else if (selection == 4) {  // Exit option
                cout << (ui.getLanguage() ? "Exiting ATM. Please take your card.\n" : "ATM을 종료합니다. 카드를 가져가세요.\n");
                break;
            }
            cout << "You selected option " << selection << "." << endl;
            this_thread::sleep_for(chrono::seconds(2)); // 2초 대기 후 메뉴로 돌아감
            /*
            else {
                ui.showErrorMessage();
                continue;
            }
*/
            if (!success) {
                cout << (ui.getLanguage() ? "Transaction failed. Returning to main menu.\n" : "거래 실패. 메인 메뉴로 돌아갑니다.\n");
            }
        }
    }
}
