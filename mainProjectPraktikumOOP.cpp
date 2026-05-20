#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cctype>
using namespace std;

int getValidInt() {
    int value;
    while (true) {
        if (cin >> value) {
            break;
        }
        cout << "Input tidak valid! Harap masukkan angka: ";
        cin.clear(); // Menghapus status error pada cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

string getValidName() {
    string name;
    bool isValid;
    
    while (true) {
        getline(cin, name);
        isValid = true;

        // Mengecek apakah input kosong atau hanya berisi spasi
        if (name.empty() || name.find_first_not_of(' ') == string::npos) {
            isValid = false;
        } else {
            // Mengecek setiap karakter (huruf per huruf) di dalam string
            for (size_t i = 0; i < name.length(); i++) {
                // Jika karakter bukan huruf dan bukan spasi, maka tidak valid
                if (!isalpha(name[i]) && !isspace(name[i])) {
                    isValid = false;
                    break;
                }
            }
        }

        if (isValid) {
            return name;
        } else {
            cout << "Input tidak valid! Nama hanya boleh berisi huruf dan spasi." << endl;
            cout << "Masukkan Nama Lengkap: ";
        }
    }
}

class Account {
protected:
    string accountNumber;
    string username;
    string password;
    double balance;
    vector<string> transactionHistory;

public:
    Account(string accNum, string name, string pass, double initialBalance) {
        accountNumber = accNum;
        username = name;
        password = pass;
        balance = initialBalance;
        addHistory("Account created with initial deposit: Rp " + to_string(initialBalance));
    }

    virtual string getAccountType() const = 0;

    string getAccountNumber() { return accountNumber; }
    string getUsername() { return username; }
    double getBalance() { return balance; }

    bool verifyPassword(string pass) {
        return password == pass;
    }

    void addHistory(string record) {
        transactionHistory.push_back(record);
    }

    void showHistory() {
        cout << "\n=== Riwayat Transaksi ===" << endl;
        if (transactionHistory.empty()) {
            cout << "Belum ada transaksi." << endl;
        } else {
            for (size_t i = 0; i < transactionHistory.size(); i++) {
                cout << i + 1 << ". " << transactionHistory[i] << endl;
            }
        }
        cout << "=========================" << endl;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            addHistory("Deposit: Rp " + to_string(amount));
            cout << "Berhasil setor tunai! Saldo Anda sekarang: Rp " << balance << endl;
        } else {
            cout << "Jumlah tidak valid!" << endl;
        }
    }

    // Polymorphism: Fungsi virtual yang bisa ditimpa (override) oleh class turunan
    virtual bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            addHistory("Withdrawal: Rp " + to_string(amount));
            cout << "Berhasil tarik tunai! Saldo Anda sekarang: Rp " << balance << endl;
            return true;
        } else {
            cout << "Saldo tidak mencukupi atau jumlah tidak valid!" << endl;
            return false;
        }
    }
};

class SavingsAccount : public Account {
public:
    SavingsAccount(string accNum, string name, string pass, double initialBalance)
        : Account(accNum, name, pass, initialBalance) {}

    string getAccountType() const override {
        return "Regular Savings";
    }

    bool withdraw(double amount) override {
        if (balance - amount < 50000) {
            cout << "Tarik tunai gagal! Saldo mengendap minimal adalah Rp 50.000." << endl;
            return false;
        }
        return Account::withdraw(amount); 
    }
};

class BankSystem {
private:
    vector<Account*> accounts;
    string generateRandomAccountNumber() {
        string accNum = "";
        for (int i = 0; i < 6; i++) {
            int randomDigit = rand() % 10;
            accNum += to_string(randomDigit);
        }
        return accNum;
    }

public:
    ~BankSystem() {
        for (size_t i = 0; i < accounts.size(); i++) {
            delete accounts[i];
        }
    }
    void createAccount() {
        string name, pass;
        double initialDeposit;

        cout << "\n=== PEMBUATAN REKENING BARU ===" << endl;
        cout << "Masukkan Nama Lengkap: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        name = getValidName(); 
        
        cout << "Buat PIN/Password: ";
        cin >> pass;

        cout << "Masukkan Setoran Awal (Minimal Rp 100000): ";
        while (true) {
            initialDeposit = getValidInt();
            if (initialDeposit >= 100000) {
                break;
            }
            cout << "Setoran kurang! Minimal Rp 100000. Masukkan lagi: ";
        }

        string newAccNum = generateRandomAccountNumber();

        Account* newAccount = new SavingsAccount(newAccNum, name, pass, initialDeposit);
        accounts.push_back(newAccount);

        cout << "\nSelamat! Rekening berhasil dibuat." << endl;
        cout << "Nomor Rekening Anda : " << newAccNum << " (HARAP DICATAT!)" << endl;
        cout << "Jenis Rekening      : " << newAccount->getAccountType() << endl;
    }
    Account* login() {
        string accNum, pass;
        cout << "\n=== LOGIN NASABAH ===" << endl;
        cout << "Nomor Rekening: ";
        cin >> accNum;
        cout << "PIN/Password  : ";
        cin >> pass;

        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i]->getAccountNumber() == accNum && accounts[i]->verifyPassword(pass)) {
                return accounts[i];
            }
        }
        
        cout << "Login Gagal! Nomor rekening atau password salah." << endl;
        return nullptr;
    }

    // Fitur Transfer Antar Rekening
    void transferFunds(Account* sender) {
        string targetAccNum;
        double amount;

        cout << "\n=== TRANSFER UANG ===" << endl;
        cout << "Masukkan Nomor Rekening Tujuan: ";
        cin >> targetAccNum;

        if (targetAccNum == sender->getAccountNumber()) {
            cout << "Tidak bisa transfer ke rekening sendiri!" << endl;
            return;
        }

        Account* receiver = nullptr;
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i]->getAccountNumber() == targetAccNum) {
                receiver = accounts[i];
                break;
            }
        }

        if (receiver == nullptr) {
            cout << "Rekening tujuan tidak ditemukan!" << endl;
            return;
        }

        cout << "Masukkan jumlah yang ingin ditransfer: Rp ";
        amount = getValidInt();

        if (sender->withdraw(amount)) {
            receiver->deposit(amount); 
            cout << "Transfer ke " << receiver->getUsername() << " berhasil!" << endl;
        }
    }
};

int main() {
    srand(time(0)); 

    BankSystem myBank;
    int choice;

    while (true) {
        cout << "\n=== SIMULASI SISTEM BANK ===" << endl;
        cout << "1. Buka Rekening Baru" << endl;
        cout << "2. Login Nasabah" << endl;
        cout << "3. Keluar" << endl;
        cout << "Pilih menu (1-3): ";
        choice = getValidInt();

        if (choice == 1) {
            myBank.createAccount();
        } 
        else if (choice == 2) {
            Account* loggedInUser = myBank.login();
            
            if (loggedInUser != nullptr) {
                int userChoice;
                bool isLoggedOut = false;

                while (!isLoggedOut) {
                    cout << "\n=== MENU NASABAH: " << loggedInUser->getUsername() << " ===" << endl;
                    cout << "1. Cek Saldo & Info" << endl;
                    cout << "2. Setor Tunai" << endl;
                    cout << "3. Tarik Tunai" << endl;
                    cout << "4. Transfer" << endl;
                    cout << "5. Riwayat Transaksi (Mutasi)" << endl;
                    cout << "6. Logout" << endl;
                    cout << "Pilih menu (1-6): ";
                    userChoice = getValidInt();

                    switch (userChoice) {
                        case 1:
                            cout << "\nSaldo saat ini: Rp " << loggedInUser->getBalance() << endl;
                            cout << "Jenis Akun: " << loggedInUser->getAccountType() << endl;
                            break;
                        case 2:
                            cout << "\nMasukkan jumlah setor: Rp ";
                            loggedInUser->deposit(getValidInt());
                            break;
                        case 3:
                            cout << "\nMasukkan jumlah tarik: Rp ";
                            loggedInUser->withdraw(getValidInt());
                            break;
                        case 4:
                            myBank.transferFunds(loggedInUser);
                            break;
                        case 5:
                            loggedInUser->showHistory();
                            break;
                        case 6:
                            cout << "Logout berhasil..." << endl;
                            isLoggedOut = true;
                            break;
                        default:
                            cout << "Pilihan tidak valid!" << endl;
                    }
                }
            }
        } 
        else if (choice == 3) {
            cout << "Terima kasih telah menggunakan sistem bank kami!" << endl;
            break;
        } 
        else {
            cout << "Pilihan tidak valid! Silakan coba lagi." << endl;
        }
    }
    return 0;
}
