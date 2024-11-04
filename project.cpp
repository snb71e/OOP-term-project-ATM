#include <iostream>
#include <string>
#include <limits> // for numeric_limits
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
public:
    void showDepositOptions() const {
        cout << "=== Deposit Menu ===" << endl;
        cout << "1. 현금 예금" << endl;
        cout << "2. 수표 예금" << endl;
        cout << "3. 거래 취소" << endl;
        cout << "Please select an option: ";
    }

    void showErrorMessage() const {
        cout << "잘못된 입력입니다. 다시 선택해주세요." << endl;
    }
};

class ATM {
private:
    string atmBank; // ATM 소속 은행
    Account* account; // 사용자의 계좌
    Bank* bank; // ATM이 속한 은행
    Interface ui;

public:
    ATM(Bank* atmBank, Account* userAccount) : bank(atmBank), account(userAccount), atmBank(atmBank->bankName) {}

    void deposit() {
        int input;
        string cardBank = account->accountNumber.substr(0, 4); // 카드 은행 구분을 위한 예시

        while (true) {  // 초기화면처럼 반복하여 메뉴를 보여줌
            ui.showDepositOptions();
            cin >> input;

            if (cin.fail()) { // 숫자가 아닌 입력 처리
                cin.clear(); // cin 플래그 초기화
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 버리기
                ui.showErrorMessage();
                continue;
            }

            if (input == 1) { // 현금 예금
                int m1, m2, m3, m4, depositAmount;
                cout << "1,000원 투입 개수: ";
                cin >> m1;
                cout << "5,000원 투입 개수: ";
                cin >> m2;
                cout << "10,000원 투입 개수: ";
                cin >> m3;
                cout << "50,000원 투입 개수: ";
                cin >> m4;

                depositAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;

                if ((m1 + m2 + m3 + m4) > 50) {
                    cout << "현금 투입 최대 장수를 초과했습니다. 투입된 현금이 반환됩니다." << endl;
                    cout << "반환된 현금은 " << depositAmount << "원 입니다." << endl;
                    continue;
                }
                if ((m1 + m2 + m3 + m4) <= 0) {
                    cout << "오류. 초기화면으로 돌아갑니다." << endl;
                    continue;
                }
                cout << "총 금액은 " << depositAmount << "원입니다." << endl;

                int input1;
                cout << "1. 금액 입력 완료" << endl << "2. 수표 예금" << endl << "3. 거래 취소" << endl;
                cin >> input1;

                if (input1 == 1) {
                    processTransaction(depositAmount, cardBank);
                }
                else if (input1 == 2) {
                    continue; // 수표 예금 메뉴로 돌아가기
                }
                else if (input1 == 3) {
                    cout << "거래 취소. 초기 화면으로 돌아갑니다." << endl;
                    break;
                }
                else {
                    ui.showErrorMessage();
                }
            }
            else if (input == 2) { // 수표 예금
                int checkAmount;
                do {
                    cout << "수표를 투입하십쇼 (100,000원 이상): ";
                    cin >> checkAmount;
                    if (checkAmount < 100000) {
                        cout << "수표 예금은 100,000원 이상만 가능합니다. 다시 입력해주세요." << endl;
                    }
                } while (checkAmount < 100000);

                cout << "수표 입금이 완료되었습니다. 최종 입금 금액은 " << checkAmount << "원입니다." << endl;

                int input2;
                cout << "1. 금액 입력 완료" << endl << "2. 현금 예금" << endl << "3. 거래 취소" << endl;
                cin >> input2;

                if (input2 == 1) {
                    processTransaction(checkAmount, cardBank);
                }
                else if (input2 == 2) {
                    continue; // 현금 예금 메뉴로 돌아가기
                }
                else if (input2 == 3) {
                    cout << "거래 취소. 초기 화면으로 돌아갑니다." << endl;
                    break;
                }
                else {
                    ui.showErrorMessage();
                }
            }
            else if (input == 3) { // 거래 취소
                cout << "거래 취소. 초기 화면으로 돌아갑니다." << endl;
                break;
            }
            else {
                ui.showErrorMessage();
            }
        }
    }

    void processTransaction(int depositAmount, const string& cardBank) {
        int fee = bank->calculateFee(cardBank);
        int insertedFee;

        cout << "수수료 " << fee << "원이 부과됩니다. 수수료를 투입해주세요: ";
        cin >> insertedFee;

        if (cin.fail()) { // 수수료에 숫자가 아닌 값 입력 시 처리
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "잘못된 수수료 입력입니다. 거래가 취소됩니다." << endl;
            return;
        }

        if (insertedFee >= fee) {
            account->deposit(depositAmount);
            cout << "입금이 완료되었습니다. 최종 입금 금액은 " << depositAmount << "원입니다." << endl;
            cout << "현재 잔액: " << account->getBalance() << "원" << endl;

            if (insertedFee > fee) {
                cout << "거스름돈 " << insertedFee - fee << "원을 받아주세요." << endl;
            }
        }
        else {
            cout << "수수료가 부족하여 거래를 진행할 수 없습니다. 초기화면으로 돌아갑니다." << endl;
        }
    }
};

int main() {
    Bank myBank("shinhan");                  // 은행 객체 생성
    Account userAccount("kakao1234", 5000); // 계좌 객체 생성 (초기 잔액 5000원)
    ATM atm(&myBank, &userAccount);         // ATM 객체 생성

    atm.deposit(); // 입금 기능 테스트
    return 0;
}
