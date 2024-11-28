// 1
// main 함수에서 고친 거
else if (userOption == "3") {
    if (!selectedATM->transfer()) continue;
    else {
        cout << (ui.getLanguage() ? "Transfer is completed successfully." : "송금이 성공적으로 완료되었습니다.") << endl;
        cout << (ui.getLanguage() ? "Press Enter to continue..." : "계속하려면 엔터 키를 누르세요...");
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 스트림 정리
        cin.get();
    }
}

// 2 
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

    if (!card_verification(account)) return false;

    // 한 세션당 최대 3번 출금
    while (withdrawalCount <= 3) {
        cout << (ui->getLanguage() ? "\n=== Withdraw Menu ===\nAccount Balance: " : "\n=== 출금 메뉴 ===\n계좌 잔액: ") << account->getAvailableFund() << " won\n";
        cout << (ui->getLanguage() ? "Enter amount to withdraw or '0' to cancel: " : "출금할 금액을 입력하거나 '0'을 눌러 취소하세요: ");

        withdrawAmount = globalinput_int(atm_list, bank_list, ui);

        // 출금 취소 처리
        if (withdrawAmount == 0) {
            cout << (ui->getLanguage() ? "Returning to User Menu..." : "사용자 메뉴로 돌아갑니다...") << endl;
            return false; // 사용자 메뉴로 돌아감
        }

        // 출금 금액 한도 및 계좌 잔액 부족 오류 처리
        if (withdrawAmount > 500000) {
            cout << (ui->getLanguage() ? "Error: Transaction limit is 500,000 won per withdrawal." : "오류: 거래 한도는 출금 당 500,000원입니다.") << endl;
            exit(0);
        }

        if (withdrawAmount > account->getAvailableFund()) {
            cout << (ui->getLanguage() ? "Error: Insufficient account balance." : "오류: 계좌 잔액 부족.") << endl;
            exit(0);
        }

        // ATM 현금 부족 오류 처리
        if (!hasSufficientCash(withdrawAmount)) {
            cout << (ui->getLanguage() ? "Error: ATM has insufficient cash." : "오류: ATM에 충분한 현금이 없습니다.") << endl;
            exit(0);
        }

        int fee = (cardBank == bank->getBankName()) ? 1000 : 2000;
        if (account->getAvailableFund() < withdrawAmount + fee) {
            cout << (ui->getLanguage() ? "Error: Insufficient balance to cover the fee." : "오류: 수수료를 포함한 잔액이 부족합니다.") << endl;
            exit(0);
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
            return false; // 사용자 메뉴로 돌아감
        }
        else if (continueWithdrawal != 1) {
            cout << (ui->getLanguage() ? "Error: Invalid input. Ending withdrawal session." : "오류: 잘못된 입력입니다. 출금 세션을 종료합니다.") << endl;
            return false;
        }
        if (continueWithdrawal == 1) {
            withdrawalCount++;
            continue;
        }
    }
    cout << (ui->getLanguage() ? "Error: Withdrawal limit exceeded. Returning to User Menu..." : "오류: 최대 인출 횟수를 초과했습니다. 사용자 메뉴로 돌아갑니다...") << endl;
    return false;
}
