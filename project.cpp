#include <iostream>
#inclue <string>
using namespace std;

class Interface
{
private:
public:
    bool isEng;
    void selectlang() {
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

class Bank
{
private:
    string bank_name;
    string bank_number; //계좌번호나 카드번호에 식별용으로 활용
    Bank* bank_list[10];
    int num_of_banks = 0;
    int num_of_accounts = 0;
    account* accounts[100];
    /* data */
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
    account* make_account(string new_owner_name, string bankname, double initial_balance, password) {
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
        account* new_account = new account(new_bank_number, new_owner_name, new_card_number, initial_balance, this, password);
        accounts[num_of_accounts++] = new_account;
        return new_account;
    }
};

class account
{
private:
    string account_number;
    string password;
    string owner_name;
    string card_number;
    double available_fund = 0;
    Bank* account_bank;
    /* data */
public:
    account(string account_num, string owner, string card_num, double init_balance, Bank* bank, string pwd) {
        account_number = account_num;
        owner_name = owner;
        card_num = card_number;
        available_fund = init_balance;
        account_bank = bank;
        password = pwd;
    }
    ~account();
    void authorize_user(string input_password) { return password == input_password; }
    void increase_account_balance(double amount) { return available_fund += amount; } //atm 속 돈 양 증가 추가
    void decrease_account_balance(double amount) { return available_fund -= amount; } //atm 속 돈 양 감소 추가
    string getAccountNumber() { return account_number; }
    string getOwnerName() { return owner_name; };
    double getAvailableFund() { return available_fund; }
};


int main() {
    return 0;
}
