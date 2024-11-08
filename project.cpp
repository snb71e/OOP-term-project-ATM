#include <iostream>
#include <string>
using namespace std;

class Bank;

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
    string getCardNumber() { return card_number; }
    string getOwnerName() { return owner_name; }
    string getPassword() { return password; }
    double getAvailableFund() { return available_fund; }
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
        Account* new_account =nullptr;
        for (int i = 0; i < num_of_banks; i++) {
            if (bank_list[i]->getBankName() == bankname) {
                new_bank_number = bank_list[i]->getBankNumber();
                new_card_number = bank_list[i]->getBankNumber();

                srand(static_cast<unsigned int>(time(0)));
                int random_number1 = rand() % 9000000 + 1000000;
                int random_number2 = rand() % 900000000 + 100000000;
                new_bank_number += to_string(random_number1);
                new_card_number += to_string(random_number2);
                new_account = new Account(new_bank_number, new_owner_name, new_card_number, initial_balance, this, password);
                accounts[num_of_accounts++] = new_account;
                cout << "Name: " << new_owner_name << endl;
                cout << "Bank Name: " << bankname << endl;
                cout << "New Account Number: " << new_bank_number << endl;
                cout << "New Card Number: " << new_card_number << endl;
                break;
            }
            else {
                cout << "There's no " << bankname << endl;
            }
        }

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

    void write_history_inventory(string transaction_detail) {
        history[num_of_transactions++] = transaction_detail;
    } // 거래 내용을 atm의 메서드 내에서 작성 후 해당 함수 적용

    void display_history() {
        for (int i = 0; i < num_of_transactions; i++) {
            cout << history[i] << endl;
        }
    }
    bool hasAccount(string accountnumber) {
        for (int i = 0; i < num_of_accounts; i++) {
            if (accounts[i]->getAccountNumber() == accountnumber) {
                return true;
            }
        }
        return false;
    }

    double increase_receiver_balance(string receiver_account_number, double amount) {
        for (int i = 0; i < num_of_accounts; i++) {
            if (accounts[i]->getAccountNumber() == receiver_account_number) {
                accounts[i]->increase_account_balance(amount);
            }
        }
    }

    void show_authorize(string user_cardnumber, string input_password) {
        for (int i = 0; i < num_of_accounts; i++) {
            if (accounts[i]->getCardNumber() == user_cardnumber) {
                if (accounts[i]->getPassword() == input_password) {
                    cout << "인증 성공" << endl;
                }
                else {
                    cout << "비밀번호가 틀렸습니다." << endl;
                }
            }
        }
    }
};

class Interface {
private:
    bool isEng;

public:
    Interface(bool lang = true) : isEng(lang) {}

    void setLanguage(bool lang) {
        isEng = lang;
    }

    bool getLanguage() const {
        return isEng;
    }

    void showDepositOptions() const {
        if (isEng) {
            cout << "=== Deposit Menu ===" << endl;
            cout << "1. Cash Deposit" << endl;
            cout << "2. Check Deposit" << endl;
            cout << "3. Cancel Transaction" << endl;
            cout << "Please select an option: ";
        }
        else {
            cout << "=== 입금 메뉴 ===" << endl;
            cout << "1. 현금 입금" << endl;
            cout << "2. 수표 입금" << endl;
            cout << "3. 거래 취소" << endl;
            cout << "옵션을 선택하세요: ";
        }
    }

    void showErrorMessage() const {
        cout << (isEng ? "Invalid input. Please try again." : "잘못된 입력입니다. 다시 시도해주세요.") << endl;
    }

    void showWelcomeMessage() const {
        if (isEng) {
            cout << "Welcome! Please select a menu option." << endl;
            cout << "1. Deposit\n2. Withdraw\n3. Transfer\n";
        }
        else {
            cout << "안녕하세요! 메뉴를 선택해주세요." << endl;
            cout << "1. 입금\n2. 출금\n3. 송금\n";
        }
    }

    void requestFeePayment(int fee) const {
        cout << (isEng ? "A fee of " : "수수료: ") << fee
            << (isEng ? " has been charged. Please insert the fee: " : "원이 부과되었습니다. 수수료를 투입하세요.") << endl;
    }

    void transactionCancelled() const {
        cout << (isEng ? "Transaction cancelled. Returning to the main menu." : "거래가 취소되었습니다. 메인 메뉴로 돌아갑니다.") << endl;
    }

    void showDepositAmount(int amount) const {
        cout << (isEng ? "Total deposit amount is " : "총 입금 금액은 ") << amount << (isEng ? " units." : "원입니다.") << endl;
    }

    void displayBalance(int balance) const {
        cout << (isEng ? "Current balance: " : "현재 잔액: ") << balance << (isEng ? " units." : "원.") << endl;
    }

    void promptForCheckDeposit() const {
        cout << (isEng ? "Please insert the check (minimum 100,000) or enter '0' to cancel: " : "수표를 넣어주세요 (최소 100,000원) 또는 '0'을 입력하여 취소하세요: ");
    }

    void showTransferOptions() const {
        cout << (isEng ? "===Transfer Menu===" : "===이체 메뉴===") << endl;
        cout << (isEng ? "1. Cash Transfer" : "1. 현금 이체") << endl;
        cout << (isEng ? "2. Account Transfer" : "2. 계좌 이체") << endl;
        cout << (isEng ? "Please select an option: " : "옵션을 선택하세요: ") << endl;
    }
};

class ATM {
private:
    string atmBank;
    Account* account;
    Bank* bank;
    Interface* ui;
    int cash[4]{ 0, 0, 0, 0 };

public:
    ATM(Bank* atmBank, Account* userAccount, Interface* interface)
        : bank(atmBank), account(userAccount), atmBank(atmBank->getBankName()), ui(interface) {
        cash[0] += 100;
        cash[1] += 50;
        cash[2] += 25;
        cash[3] += 10;
    }

    bool deposit() {
        int input;
        string cardBank = account->getAccountNumber().substr(0, 4);

        while (true) {
            ui->showDepositOptions();
            cin >> input;

            if (input == 1) { // 현금 예금
                int m1, m2, m3, m4, depositAmount;
                cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
                cin >> m1;
                cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
                cin >> m2;
                cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
                cin >> m3;
                cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
                cin >> m4;

                depositAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;

                if ((m1 + m2 + m3 + m4) > 50) {
                    cout << (ui->getLanguage() ? "Exceeded maximum number of bills. Returning cash." : "현금 투입 최대 장수를 초과했습니다. 투입된 현금을 반환합니다.") << endl;
                    ui->showDepositAmount(depositAmount);
                    continue;
                }

                if ((m1 + m2 + m3 + m4) <= 0) {
                    ui->transactionCancelled();
                    return false;
                }

                ui->showDepositAmount(depositAmount);
                processTransaction(depositAmount, cardBank);
                return true;
            }
            else if (input == 2) { // 수표 예금
                int checkAmount;
                while (true) {
                    ui->promptForCheckDeposit();
                    cin >> checkAmount;
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
        int m1, m2, m3, m4, depositAmount;
        cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
        cin >> m1;
        cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
        cin >> m2;
        cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
        cin >> m3;
        cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
        cin >> m4;
        depositAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;
        cout << depositAmount << (ui->getLanguage() ? "KRW entered. " : "원 들어왔습니다.") << endl;
        cash[0] += m1;
        cash[1] += m2;
        cash[2] += m3;
        cash[3] += m4;
        if (depositAmount >= fee) {
            if (depositAmount > fee) {
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << depositAmount - fee << endl;
                if (not change_ATM_dec(depositAmount - fee)) {
                    return false;
                }
            }
            cout << (ui->getLanguage() ? "Fee paid successfully. " : "수수료가 납부되었습니다.") << endl;
            return true;
        }
        else {
            cout << (ui->getLanguage() ? "Insufficient fee payment.\nReturning to the main menu." : "수수료가 부족하여 거래를 진행할 수 없습니다.\n메인 메뉴로 돌아갑니다.") << endl;
            return false;
        }
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
                cout << (ui->getLanguage() ? "Error: No Cash in ATM" : "오류: ATM 내에 현금이 부족합니다.");
                ui->transactionCancelled();
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
        double balance = account->getAvailableFund();
        if (balance >= fee) {
            account->decrease_account_balance(fee);
            cout << (ui->getLanguage() ? "Successfully Fee paid!" : "수수료 납부 성공!") << endl;
            return true;
            //이 수수료는 어디로 가는지??
        }
        else {
            cout << (ui->getLanguage() ? "Insufficient funds in your account." : "계좌에 잔액이 부족합니다.") << endl;
            return false;
        }
    }

    bool transfer() {
        int input;
        while (true) {
            ui->showTransferOptions();
            cin >> input;
            if (input == 1) { //현금 이체
                cout << (ui->getLanguage() ? "Please enter the account to transfer" : "송금할 계좌를 입력해 주세요.") << endl;
                string account_num;
                cin >> account_num;
                if (bank->hasAccount(account_num)) { // 존재하는 계좌인 경우
                    int transferAmount;
                    cout << (ui->getLanguage() ? "How much money do you want to transfer" : "얼만큼의 돈을 이체하시겠습니까?") << endl;
                    cout << account->getAvailableFund() << (ui->getLanguage() ? "KRW You can transfer." : "원 이체 가능합니다.") << endl;
                    cin >> transferAmount;
                    if (account->getAvailableFund() < transferAmount) {
                        cout << (ui->getLanguage() ? "Not enough money to transfer in your account. " : "계좌에 돈이 부족합니다.") << endl;
                    }
                    else {
                        int m1, m2, m3, m4, deposited;
                        cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
                        cin >> m1;
                        cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
                        cin >> m2;
                        cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
                        cin >> m3;
                        cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
                        cin >> m4;

                        deposited = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;

                        if (deposited < transferAmount) { // 부족한 돈을 넣은 경우
                            cout << (ui->getLanguage() ? "It is not enough money." : "돈이 부족합니다.") << endl;
                            break; //TransferOption으로
                        }
                        else if (deposited > transferAmount) { // 초과된 돈을 넣은 경우
                            cout << (ui->getLanguage() ? "Transfer amount: " : "이체 금액: ") << transferAmount << endl;
                            cout << (ui->getLanguage() ? "Amount entered: " : "투입된 금액: ") << deposited << endl;
                            cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << deposited - transferAmount << endl;
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
                }
                else { // 잘못된 계좌 번호 입력
                    cout << (ui->getLanguage() ? "Wrong Account number. Please start over from scratch." : "존재하지 않는 계좌입니다. 다시 시작하십시오.") << endl;
                    // welcome으로
                    return false;
                }
            }
            else if (input == 2) { //계좌 이체
                string cardBank = account->getAccountNumber().substr(0, 4);
                string ATMBank = bank->getBankNumber();

                // card 검증 단계 (언젠간 채워질 예정)

                if (ui->getLanguage()) {
                    cout << "You can transfer KRW " << account->getAvailableFund() << ".\n";
                }
                else {
                    cout << account->getAvailableFund() << "원 계좌 이체 가능합니다." << endl;
                }
                cout << (ui->getLanguage() ? "Please enter the account to transfer" : "송금할 계좌를 입력해 주세요.") << endl;
                string account_num;
                cin >> account_num;
                if (bank->hasAccount(account_num)) { // 존재하는 계좌인 경우
                    string transferBank = account_num.substr(0, 4);

                    int transferAmount, m1, m2, m3, m4, deposited;
                    cout << (ui->getLanguage() ? "How much money do you want to transfer" : "얼만큼의 돈을 이체하시겠습니까?") << endl;
                    cin >> transferAmount;
                    cout << (ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: ");
                    cin >> m1;
                    cout << (ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: ");
                    cin >> m2;
                    cout << (ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: ");
                    cin >> m3;
                    cout << (ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: ");
                    cin >> m4;

                    deposited = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;


                    if (deposited < transferAmount) { // 부족한 돈을 넣은 경우
                        cout << (ui->getLanguage() ? "It is not enough money." : "돈이 부족합니다.") << endl;
                        break; //TransferOption으로
                    }
                    else if (deposited > transferAmount) { // 초과된 돈을 넣은 경우
                        cout << (ui->getLanguage() ? "Transfer amount: " : "이체 금액: ") << transferAmount << endl;
                        cout << (ui->getLanguage() ? "Amount entered: " : "투입된 금액: ") << deposited << endl;
                        cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << deposited - transferAmount << endl;
                    }
                    // 수수료 부과
                    if ((ATMBank == cardBank) and (cardBank == transferBank)) { // 같은 은행 : 같은 계좌
                        if (fee_account_calculator(2000)) {
                            cout << (ui->getLanguage() ? "Transfer completed successfully." : "이체를 성공하였습니다!") << endl;
                            cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        }
                        else {
                            return false;
                        }

                    }
                    else if ((ATMBank == cardBank) and (cardBank != transferBank)) { // 같은 은행 : 다른 계좌
                        if (fee_account_calculator(3000)) {
                            cout << (ui->getLanguage() ? "Transfer completed successfully." : "이체를 성공하였습니다!") << endl;
                            cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        }
                        else {
                            return false;
                        }
                    }
                    else { // 다른 은행 : 다른 계좌
                        if (fee_account_calculator(4000)) {
                            cout << (ui->getLanguage() ? "Transfer completed successfully." : "이체를 성공하였습니다!") << endl;
                            cout << (ui->getLanguage() ? "Account Balance: " : "계좌 잔액: ") << account->getAvailableFund() << endl;
                        }
                        else {
                            return false;
                        }
                    }
                }
                else { // 잘못된 계좌 번호 입력
                    cout << (ui->getLanguage() ? "Wrong Account number. Please start over from scratch." : "존재하지 않는 계좌입니다. 다시 시작하십시오.") << endl;
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
        int insertedFee;

        ui->requestFeePayment(fee);
        cin >> insertedFee;

        if (insertedFee >= fee) {
            account->increase_account_balance(depositAmount);
            ui->showDepositAmount(depositAmount);
            ui->displayBalance(account->getAvailableFund());

            if (insertedFee > fee) {
                cout << (ui->getLanguage() ? "Change: " : "거스름돈: ") << insertedFee - fee << (ui->getLanguage() ? " units." : "원.") << endl;
            }
        }
        else {
            cout << (ui->getLanguage() ? "Insufficient fee payment. Returning to the main menu." : "수수료가 부족하여 거래를 진행할 수 없습니다. 메인 메뉴로 돌아갑니다.") << endl;
        }
    }
};

int main() {

    Bank myBank("toss", "111");
    Account* myAccount = myBank.make_account("Seonae", "toss", 30000, "1234");
    //Account* myAccount = myBank.make_account("K", "kakao", 10000, "2345");*/
    Interface ui;
    ATM atm(&myBank, myAccount, &ui);

    cout << "1. English" << endl << "2. 한국어" << endl;
    int n;
    cin >> n;
    if (n == 1) {
        ui.setLanguage(true);
    }
    else {
        ui.setLanguage(false);
    }

    while (true) {
        ui.showWelcomeMessage();

        int selection;
        cin >> selection;

        if (selection == 1) {
            bool success = atm.deposit();
            if (!success) continue; // Welcome 화면으로 돌아가기
        }
        else if (selection == 2) {
            bool success = atm.transfer();
            if (!success) continue;
        }
        else {
            cout << (ui.getLanguage() ? "Option not yet implemented." : "아직 구현되지 않은 옵션입니다.") << endl;
        }
    }

    /*  return 0;*/
}
