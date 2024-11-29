bool ATM::transfer() {
    while (true) {
        ui->showTransferOptions();
        int input = globalinput_int(atm_list, bank_list, ui);

        if (input == 1) { // 현금 송금
            cout << (ui->getLanguage() ? "Please enter the account to transfer or '0' to cancel." : "송금할 계좌를 입력하거나 '0'을 눌러 취소하세요.") << endl;
            string account_num = globalinput_string(atm_list, bank_list, ui);

            if (account_num == "0") { // 송금 취소
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }

            if (!isInteger(account_num)) { // 유효하지 않은 계좌 번호
                ui->showErrorMessage();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }

            if (bank->hasAccount(account_num)) { // 유효한 계좌인 경우
                cout << (ui->getLanguage() ? "<The amount of transfer>" : "<송금할 금액>") << endl;

                int m1, m2, m3, m4 = 0;
                long long transferAmount;

                cout << (ui->getLanguage() ? "Enter the number of bills for each denomination (or enter '0' to cancel):\n"
                    : "각 지폐의 개수를 입력하세요 (또는 '0'을 입력해 취소):\n");
                while (m1 + m2 + m3 + m4 <= 50) {
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
                if (m1 + m2 + m3 + m4 == 0) {
                    cout << (ui->getLanguage() ? "Cancelling...\n" : "취소 중...\n");
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }

                transferAmount = m1 * 1000 + m2 * 5000 + m3 * 10000 + m4 * 50000;


                // 거래 기록 저장 및 수수료 부과
                if (fee_cash_calculator(1000)) {
                    transaction_recording(transactionid(), account->getCardNumber(), "Cash Transfer", transferAmount, getatmbank(), getatmID(), bank->access_to_account_by_num(account_num)->getBankName(), bank->access_to_account_by_num(account_num)->getAccountNumber(), "(Cash transfer)");
                    ui->showTransferSuccessUI(account->getAvailableFund());
                    bank->increase_receiver_balance(account_num, transferAmount);
                    return true;
                }
                else {
                    ui->showErrorMessage();
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
            }
            else { // 잘못된 계좌 번호
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account." : "거래 실패: 존재하지 않는 계좌.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
        }
        else if (input == 2) { // 계좌 송금
            cout << (ui->getLanguage() ? "Enter account number to transfer or '0' to cancel." : "송금할 계좌 번호를 입력하거나 '0'을 눌러 취소하세요: ") << endl;
            string account_num = globalinput_string(atm_list, bank_list, ui);

            if (account_num == "0") { // 송금 취소
                ui->transactionCancelled();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }

            if (!bank->hasAccount(account_num)) { // 잘못된 계좌 번호
                cout << (ui->getLanguage() ? "Transaction failed: Non-existent account." : "거래 실패: 존재하지 않는 계좌.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }

            double transferAmount;
            cout << (ui->getLanguage() ? "Enter amount to transfer or '0' to cancel." : "송금할 금액을 입력하거나 '0'을 눌러 취소하세요.") << endl;

            while (true) {
                transferAmount = globalinput_double(atm_list, bank_list, ui);
                if (transferAmount == 0) { // 송금 취소
                    ui->transactionCancelled();
                    cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                    cin.ignore();
                    cin.get();
                    return false;
                }
                if (isPositive(transferAmount)) break;

                ui->showErrorMessage(); // 유효하지 않은 금액 입력
            }

            if (account->getAvailableFund() < transferAmount) { // 잔액 부족
                cout << (ui->getLanguage() ? "Transaction failed: Insufficient balance." : "거래 실패: 잔액 부족.") << endl;
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }

            // 거래 기록 저장 및 수수료 계산
            account->decrease_account_balance(transferAmount);

            if (fee_account_calculator(2000)) {
                transaction_recording(transactionid(), account->getCardNumber(), "Account Transfer", transferAmount, getatmbank(), getatmID(), bank->getBankName(), bank->access_to_account_by_num(account_num)->getOwnerName(), bank->access_to_account_by_num(account_num)->getAccountNumber(), "(Account transfer)");

                bank->increase_receiver_balance(account_num, transferAmount);
                ui->showTransferSuccessUI(account->getAvailableFund());
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return true;
            }
            else {
                account->increase_account_balance(transferAmount);
                ui->showErrorMessage();
                cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
                cin.ignore();
                cin.get();
                return false;
            }
        }
        else if (input == 3 or input == 0) { // 거래 취소
            ui->transactionCancelled();
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
        else {
            ui->showErrorMessage();
            cout << (ui->getLanguage() ? "Press Enter to continue..." : "계속하려면 Enter를 누르세요...");
            cin.ignore();
            cin.get();
            return false;
        }
    }
}
