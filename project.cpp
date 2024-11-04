#include <iostream>
#include <string>
using namespace std;

class Account {
public:
    string accountNumber;
    int balance;

    Account(string accNum, int initialBalance) : accountNumber(accNum), balance(initialBalance) {}

    void deposit(int amount) {
        balance += amount;
    }

    int getBalance() const {
        return balance;
    }
};

class Bank {
public:
    string bankName;

    Bank(string name) : bankName(name) {}

    int calculateFee(const string& cardBank) const {
        return (cardBank == bankName) ? 1000 : 2000;
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
            << (isEng ? " has been charged. Please insert the fee: " : "원이 부과되었습니다. 수수료를 투입하세요: ");
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
};

class ATM {
private:
    string atmBank;
    Account* account;
    Bank* bank;
    Interface* ui;

public:
    ATM(Bank* atmBank, Account* userAccount, Interface* interface)
        : bank(atmBank), account(userAccount), atmBank(atmBank->bankName), ui(interface) {}

    bool deposit() {
        int input;
        string cardBank = account->accountNumber.substr(0, 4);

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

    void processTransaction(int depositAmount, const string& cardBank) {
        int fee = bank->calculateFee(cardBank);
        int insertedFee;

        ui->requestFeePayment(fee);
        cin >> insertedFee;

        if (insertedFee >= fee) {
            account->deposit(depositAmount);
            ui->showDepositAmount(depositAmount);
            ui->displayBalance(account->getBalance());

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
    Bank myBank("kakao");
    Account userAccount("kakao1234", 5000);
    Interface ui;
    ATM atm(&myBank, &userAccount, &ui);

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
        else {
            cout << (ui.getLanguage() ? "Option not yet implemented." : "아직 구현되지 않은 옵션입니다.") << endl;
        }
    }

    return 0;
}
