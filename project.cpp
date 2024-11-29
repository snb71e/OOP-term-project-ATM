// 1
bool ATM::depositCash(Interface* ui, int& m1, int& m2, int& m3, int& m4, int& totalBills) {
    totalBills = 0;
    m1, m2, m3, m4 = 0;

    cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' to cancel):\n"
        : "각 지폐의 개수를 입력하세요 (또는 '0'을 입력해 취소):\n");
    while (m1 + m2 + m3 + m4 <= 50){
        // 지폐 입력 및 총 금액 계산
        m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 투입 개수: "), ui);
        m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 투입 개수: "), ui);
        m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 투입 개수: "), ui);
        m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 투입 개수: "), ui);
        break;
    }
    if (m1 + m2 + m3 + m4 > 50) {
        cout << (ui->getLanguage() ? "Do not insert more than number of 50 bills" : "현금을 50장 이상 넣을 수 없습니다.");
        exit(0);
    }

    totalBills = (m1 * 1000) + (m2 * 5000) + (m3 * 10000) + (m4 * 50000);

    if (totalBills == 0) {
        cout << (ui->getLanguage() ? "No money entered. Cancelling...\n" : "입금 금액이 없습니다. 취소 중...\n");
        return false;
    }

    cout << (ui->getLanguage() ? "Total deposit amount: " : "총 입금 금액: ") << totalBills << " won\n";
    return true;
}

// 2
bool ATM::withdraw() {
    long long int withdrawAmount;
    if (ui == nullptr || account == nullptr) {
        cout << (ui->getLanguage() ? "Error: Account is not initialized." : "계좌가 개설되어 있지 않습니다.") << endl;
        cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
        cin.ignore();
        cin.get();
        return false;
    }

    string cardBank = account->getAccountNumber().substr(0, 4); // 카드의 은행 코드 확인
    int withdrawalCount = 0; // 세션 당 인출 횟수 제한

    if (!card_verification(account)) return false;

    // 한 세션당 최대 3번 출금
    while (withdrawalCount < 3) {
        cout << (ui->getLanguage() ? "\n=== Withdraw Menu ===\nAccount Balance: " : "\n=== 출금 메뉴 ===\n계좌 잔액: ") << account->getAvailableFund() << " won\n";
        cout << (ui->getLanguage() ? "<The Amount to withdraw>" : "<출금할 금액>") << endl;

        int m1, m2, m3, m4 = 0;

        cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' for all to cancel):\n"
            : "각 지폐의 개수를 입력하세요 (또는 전부 '0'을 입력해 취소):\n");
        while (m1 + m2 + m3 + m4 <= 50) {
            // 지폐 입력 및 총 금액 계산
            m1 = getValidInput((ui->getLanguage() ? "Number of 1,000 bills: " : "1,000원 개수: "), ui);
            m2 = getValidInput((ui->getLanguage() ? "Number of 5,000 bills: " : "5,000원 개수: "), ui);
            m3 = getValidInput((ui->getLanguage() ? "Number of 10,000 bills: " : "10,000원 개수: "), ui);
            m4 = getValidInput((ui->getLanguage() ? "Number of 50,000 bills: " : "50,000원 개수: "), ui);
            break;
        }
        if (m1 + m2 + m3 + m4 > 50) {
            cout << (ui->getLanguage() ? "Do not insert more than number of 50 bills" : "현금을 50장 이상 넣을 수 없습니다.");
            exit(0);
        }

        withdrawAmount = (m1 * 1000) + (m2 * 5000) + (m3 * 10000) + (m4 * 50000);


        // 출금 취소 처리
        if (withdrawAmount == 0) {
            cout << (ui->getLanguage() ? "Returning to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false; // 사용자 메뉴로 돌아감
        }

        // 출금 금액 한도 및 계좌 잔액 부족 오류 처리
        if (withdrawAmount > 500000) {
            cout << (ui->getLanguage() ? "Error: Transaction limit is 500,000 won per withdrawal." : "오류: 거래 한도는 출금 당 500,000원입니다.") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            continue;
        }

        if (withdrawAmount > account->getAvailableFund()) {
            cout << (ui->getLanguage() ? "Error: Insufficient account balance." : "오류: 계좌 잔액 부족.") << endl; //655000
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }

        // ATM 현금 부족 오류 처리
        if (!hasSufficientCash(withdrawAmount)) {
            cout << (ui->getLanguage() ? "Error: ATM has insufficient cash." : "오류: ATM에 충분한 현금이 없습니다.") << endl;
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }

        int fee = (cardBank == bank->getBankName()) ? 1000 : 2000;
        if (account->getAvailableFund() < withdrawAmount + fee) {
            cout << (ui->getLanguage() ? "Error: Insufficient balance to cover the fee." : "오류: 수수료를 포함한 잔액이 부족합니다.") << endl;
            cout << (ui->getLanguage() ? "Returing to Menu..." : "메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...") << endl;
            cin.ignore();
            cin.get();
            return false;
        }

        // 출금 처리
        account->decrease_account_balance(withdrawAmount + fee);
        dispenseCash(withdrawAmount);
        withdrawalCount++;
        transaction_recording(transactionid(), account->getCardNumber(), "Withdraw", withdrawAmount, getatmbank(), getatmNumber(), account->getOwnerName(), account->getBankName(), account->getAccountNumber());

        cout << (ui->getLanguage() ? "Withdrawal Successful!" : "출금 성공!") << endl;
        cout << (ui->getLanguage() ? "Current Balance: " : "현재 잔액: ") << account->getAvailableFund() << " won\n";

        // 추가 출금 여부 확인
        int continueWithdrawal = globalinput_int(atm_list, bank_list, ui);
        cout << (ui->getLanguage() ? "Would you like to make another withdrawal? (1: Yes, 0: No): " : "추가 인출을 원하시면 1을, 종료하시려면 0을 입력하세요: ");
        if (continueWithdrawal == 0) {
            cout << (ui->getLanguage() ? "Returning to User Menu..." : "사용자 메뉴로 돌아갑니다...") << endl;
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false; // 사용자 메뉴로 돌아감
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
}
