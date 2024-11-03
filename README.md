# OOP
#객체지향프로그래밍 term project
#atm 만들기
객프 텀프


역할
보고서 작성
팀원 정보: 이름 및 학번
최종 클래스 다이어그램: 클래스 관계를 설명하는 다이어그램
요구사항 체크 시트: 구현된 요구사항은 ‘O’, 구현되지 않은 요구사항은 ‘X’로 표시
콘솔 스크린샷: 각 요구사항을 테스트한 스크린샷과 간단한 설명 (요구사항 ID 포함)
각자 맡은 사람이 작성
객체지향 개념 사용 설명: 예) 상속, 다형성 등의 객체지향 개념 설명 및 적용 방식
각자 맡은 사람이 작성
소스 코드 실행 방법
최종 소스 코드: cpp 파일에서 복사하여 보고서에 붙여 넣기
멤버 기여도 표: 각 팀원의 기여도 정보


코드 짜기
구상하기
시연하기- 질문받기 (4명)
틀
class
-atm
-bank
-account
-interface

INTERFACE
Welcome
Cash/Check Insert and Dispenser
 Card Insert
 Receipt Print | Keypad

ATM

ATM 별 고유 ID(6자리)
거래 (입금, 출금, 이체 등) 별 고유 ID
다중 은행 ATM
이중 언어(이중언어 사용시 사용자가 선택 가능)
- atm이 언어 옵션 제시 ( ENG or KOR ) -> 선택 후 모든 메뉴는 그 언어로 되도록 설정
ATM이 갖고 있는 현금
cash (KRW 1,000, 5,000, 10,000, 50,000) 이렇게만 처리 가능
check (KRW 100,000~) 이렇게만 처리 가능
Welcome | Cash/Check Insert and Dispenser | Card Insert | Receipt Print | Keypad 출력

Card Insert

콘솔에 card 번호 입력 = card 넣는 거
단일 ATM인 경우 card 유형 맞는지
admin card일때 “Transaction history”에 access 가능해야함
-> 관리자 메뉴: admin Menu (transaction History)
관리자 카드 들어오면 ATM은 저 히스토리 창만 메뉴로 가짐
그 메뉴 선택되면 system 시작 이후 all users(ID,카드넘버  등 info)
txt  파일로 아웃풋 나오도록 
사용자 인증
비밀번호 맞는지
틀리면 error 메시지지
3회 이상 틀리면 종료, 카드 반환

Cash/Check Insert and Dispense

cancel / 현금 부족 시 종료
각 atm은 여러 종류의 transaction fees 가짐 ( Deposit fee, Withdrawal fee,
Account transfer fee ,Cash transfer fee - non/primary 각각 값이 다름)
Deposit fee for non-primary banks: KRW 2,000; the fee is paid by inserting additional cash. 
Deposit fee for primary banks: KRW 1,000; the fee is paid by inserting additional cash. 
Withdrawal fee for a primary bank: KRW 1,000; the fee is paid from the withdrawal account. 
Withdrawal fee for non-primary banks: KRW 2,000; the fee is paid from the withdrawal account. 
Account transfer fee between primary banks: KRW 2,000; the fee is paid from the source account. 
Account transfer fee between primary and non-primary banks: KRW 3,000; the fee is paid from the source account. 
Account transfer fee between non-primary banks: KRW 4,000; the fee is paid from the source account. 
Cash transfer fee to any bank type: KRW 1,000; the fee is paid by inserting additional cash.  
	
Deposit

cash, check 슬롯 통해 user로부터 입금
한 거래 당 cash 50장, check 30장까지
넘기면 error
추가 금액 bank account에 반영되어야 함
fee 부과
check의 경우 fee는 cash로
cash -> ATM의 cash 증가
check -> ATM의 cash 증가 X
Withdraw
withdraw 횟수: 한 session 당 최대 3회
withdraw 한도: 한 거래 당 500,000까지
user에게 출금 금액 입력 요청
최소 개수의 지폐(?) 사용하여 cash 지급
account 돈 부족 or ATM 돈 부족 시 error
빠진 금액 bank accout에 반영되어야 함
fee 부과
ATM의 cash 감소
송금
- 처음에 송금 타입(cash 이체/ account 이체) 결정 -> 송금 보낼 계좌 물어보기, 사용자가 입력
- 거래 비용 청구
- 

- cash transfer : 각 송금마다 필요한 지폐 입력, 송금 보낼 금액+거래비용 만큼 insert하도록 요청 -> 금액 검증 후 거래비용 제외하고 송금(현금 INSERT시 ATM 속 현금 금액 증가)
- 
- account transfer : 출금할 계좌번호, 송금액 입력 요청  (카드 등의 수단으로 계좌번호 확인 되었으면 계좌번호는 요청 x)

- 거래 완료 후 출금 계좌 잔액 감소, 입금 계좌 잔액 증가가
- Receipt Print
	- user가 receipt 요청 시 거래 내역 receipt 제공



BANK

새로운 계좌 개설, 새로운 사용자 이름, 계좌 번호, 초기 잔액 설정 이를 계좌 목록에 추가
은행 이름 return 
잔액 return 
카드 번호와 비밀번호를 이용해 사용자 인증 
계좌 잔액 업데이트
수수료 계산?
주거래은행/비주거래은행 -> 입금/출금/이체
시스템 전체의 중앙 거래 기록 = >거래 내역 정보는 외부 파일(txt file)로 출력 가능
ACCOUNT
 ‘/’문자 입력 시 모든 atm, 계좌 정보 출력
password 체크 T/F
잔액
개별 계좌별 거래내역

예외처리

ATM 사용 중 현금 부족
ATM내 현금 부족
패스워드 오류
출금 횟수초과
유효하지 않은 현금
유효하지 않은 계좌
문자열 관련 오류(string input -> int, int input -> string…)
잘못된 선택한 경우
account 오류
허용범위 오류(현금 투입 range 설정)
현금이나 수표 너무 많이 넣은 경우
계좌 잔액 부족 오류
	=>각각의 오류마다 에러 메시지를 출력해야됨

