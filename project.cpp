#include<iostream>
using namespace std;

class Interface
{
private:
public:
    bool isEng;
    void selectlang(){
        cout << "Select Language.\n 언어를 선택하세요.\n";
        cout << "1. English\n2. 한국어";
        int num;
        cin >> num;
        if (num == 1)
            isEng == true;
        else if (num == 2)
            isEng == false;
    }
    void makeaccount() {

    }
    void selectatm() {

    }
    void welcome() {
        if (isEng) {
            cout << "Welcome!\nSelect Menu.\n1. Deposit\n2. Withdraw\n 3. Transfer\n";
            int num;
            cin >> num;
            if (num == 1) {
                // card 검증 단계로
            }
            else if (num == 2) {
                // card 검증 단계로
            }
            else if (num == 3) {
                cout << "Select Method.\n1. Cash Transfer\n2. Account Transfer\n";
                // Transfer 단계로
            }
        }
        else {
            cout << "안녕하세요!\n무엇을 도와 드릴까요?\n 1. 입금\n2. 출금\n3. 이체\n";
            int num;
            cin >> num;
            if (num == 1) {
                // card 검증 단계로
            }
            else if (num == 2) {
                // card 검증 단계로
            }
            else if (num == 3) {
                cout << "어느 방식으로 송금하시겠습니까?\n1. 현금 이체\n2. 계좌 이체\n";
                // Transfer 단계로
            }
        }
    }
};

class Bank
{
private:
    /* data */
public:
    Bank(/* args */);
    ~Bank();

};

Bank::Bank(/* args */)
{
}

Bank::~Bank()
{
}

class ATM
{
private:
    /* data */
    int atm_cash[4] = { 0, 0, 0, 0 };
public:
    ATM(/* args */);
    ~ATM();
};

ATM::ATM(/* args */)
{
}

ATM::~ATM()
{
}

class account
{
private:
    /* data */
public:
    account(/* args */);
    ~account();
};

account::account(/* args */)
{
}

account::~account()
{
}

int main() {
    return 0;
}
