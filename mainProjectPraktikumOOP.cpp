#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // Untuk rand() dan srand() (angka acak)
#include <ctime>   // Untuk time() (seed angka acak)
#include <limits>  // Untuk validasi input (menghindari error huruf di menu angka)
#include <cctype> // Untuk fungsi isalpha() dan isspace()

using namespace std;

// ==========================================
// FUNGSI BANTUAN (HELPER)
// ==========================================

// Fungsi untuk mencegah program error/looping jika user memasukkan huruf pada input angka
int getValidInt() {
    int value;
    while (true) {
        if (cin >> value) {
            break; // Input benar berupa angka, keluar dari loop
        }
        // Jika gagal (input huruf/simbol)
        cout << "Input tidak valid! Harap masukkan angka: ";
        cin.clear(); // Menghapus status error pada cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membuang sisa input yang salah di buffer
    }
    return value;
}

// Fungsi untuk memvalidasi bahwa input nama hanya berisi huruf dan spasi
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
                    break; // Keluar dari loop pengecekan karakter
                }
            }
        }

        if (isValid) {
            return name; // Jika lolos semua cek, kembalikan nama
        } else {
            cout << "Input tidak valid! Nama hanya boleh berisi huruf dan spasi." << endl;
            cout << "Masukkan Nama Lengkap: ";
        }
    }
}

// ==========================================
// 1. ABSTRACTION & ENCAPSULATION
// ==========================================

// Kelas dasar (Base Class) yang bersifat abstrak
class Account {
protected: // Encapsulation: Atribut hanya bisa diakses oleh class ini dan class turunannya
    string accountNumber;
    string username;
    string password;
    double balance;
    vector<string> transactionHistory; // Menyimpan riwayat mutasi rekening

public:
    // Constructor untuk inisialisasi data awal
    Account(string accNum, string name, string pass, double initialBalance) {
        accountNumber = accNum;
        username = name;
        password = pass;
        balance = initialBalance;
        addHistory("Account created with initial deposit: Rp " + to_string(initialBalance));
    }

    // Abstraction: Pure virtual function. Memaksa class turunan untuk membuat fungsi ini
    virtual string getAccountType() const = 0;

    // Getter untuk mengambil data (karena atribut bersifat protected)
    string getAccountNumber() { return accountNumber; }
    string getUsername() { return username; }
    double getBalance() { return balance; }

    // Fungsi untuk mencocokkan password saat login
    bool verifyPassword(string pass) {
        return password == pass;
    }

    // Fungsi untuk menambah catatan riwayat transaksi
    void addHistory(string record) {
        transactionHistory.push_back(record);
    }

    void showHistory() {
        cout << "\n=== Riwayat Transaksi ===" << endl;
        if (transactionHistory.empty()) {
            cout << "Belum ada transaksi." << endl;
        } else {
            // Looping untuk menampilkan isi vector history
            for (size_t i = 0; i < transactionHistory.size(); i++) {
                cout << i + 1 << ". " << transactionHistory[i] << endl;
            }
        }
        cout << "=========================" << endl;
    }

    // Fungsi setor tunai
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

// ==========================================
// 2. INHERITANCE (Pewarisan)
// ==========================================

// Class SavingsAccount (Tabungan) mewarisi class Account
class SavingsAccount : public Account {
public:
    // Memanggil constructor dari Base Class
    SavingsAccount(string accNum, string name, string pass, double initialBalance)
        : Account(accNum, name, pass, initialBalance) {}

    // Polymorphism: Mengimplementasikan pure virtual function dari Base Class
    string getAccountType() const override {
        return "Regular Savings";
    }

    // Polymorphism: Menimpa (override) fungsi tarik tunai dari Base Class
    // Misalnya ada aturan bisnis: Tabungan reguler harus menyisakan saldo minimal Rp 50.000
    bool withdraw(double amount) override {
        if (balance - amount < 50000) {
            cout << "Tarik tunai gagal! Saldo mengendap minimal adalah Rp 50.000." << endl;
            return false;
        }
        // Jika lolos pengecekan di atas, panggil fungsi withdraw dari class induk
        return Account::withdraw(amount); 
    }
};

// ==========================================
// KELAS UTAMA: SISTEM BANK
// ==========================================

class BankSystem {
private:
    // Vector untuk menyimpan pointer ke Account. 
    // Menggunakan pointer agar bisa menyimpan objek dari class turunan (Polymorphism)
    vector<Account*> accounts;

    // Fungsi membuat nomor rekening acak 6 digit
    string generateRandomAccountNumber() {
        string accNum = "";
        for (int i = 0; i < 6; i++) {
            int randomDigit = rand() % 10; // Mengambil angka acak 0-9
            accNum += to_string(randomDigit);
        }
        return accNum;
    }

public:
    // Destructor: Membersihkan memori pointer saat program ditutup
    ~BankSystem() {
        for (size_t i = 0; i < accounts.size(); i++) {
            delete accounts[i];
        }
    }

    // Fitur Registrasi / Pembuatan Rekening Baru
    void createAccount() {
        string name, pass;
        double initialDeposit;

        cout << "\n=== PEMBUATAN REKENING BARU ===" << endl;
        cout << "Masukkan Nama Lengkap: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer secara total sebelum getline
        
        // Memanggil fungsi validasi nama
        name = getValidName(); 
        
        cout << "Buat PIN/Password: ";
        cin >> pass;

        cout << "Masukkan Setoran Awal (Minimal Rp 100000): ";
        while (true) {
            initialDeposit = getValidInt(); // Memakai fungsi validasi
            if (initialDeposit >= 100000) {
                break; // Lolos aturan bisnis
            }
            cout << "Setoran kurang! Minimal Rp 100000. Masukkan lagi: ";
        }

        string newAccNum = generateRandomAccountNumber();

        // Membuat objek SavingsAccount baru dan memasukkannya ke vector Account*
        Account* newAccount = new SavingsAccount(newAccNum, name, pass, initialDeposit);
        accounts.push_back(newAccount);

        cout << "\nSelamat! Rekening berhasil dibuat." << endl;
        cout << "Nomor Rekening Anda : " << newAccNum << " (HARAP DICATAT!)" << endl;
        cout << "Jenis Rekening      : " << newAccount->getAccountType() << endl;
    }

    // Fitur Login yang mengembalikan pointer ke akun yang berhasil login
    Account* login() {
        string accNum, pass;
        cout << "\n=== LOGIN NASABAH ===" << endl;
        cout << "Nomor Rekening: ";
        cin >> accNum;
        cout << "PIN/Password  : ";
        cin >> pass;

        // Mencari akun di dalam vector
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i]->getAccountNumber() == accNum && accounts[i]->verifyPassword(pass)) {
                return accounts[i]; // Login berhasil, kembalikan objek akunnya
            }
        }
        
        cout << "Login Gagal! Nomor rekening atau password salah." << endl;
        return nullptr; // Mengembalikan null jika tidak ketemu
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

        // Mencari rekening tujuan
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

        // Jika saldo pengirim berhasil ditarik, tambahkan ke penerima
        if (sender->withdraw(amount)) {
            // Karena fungsi withdraw Base Class menambah history "Withdrawal", 
            // kita bisa hapus/ganti logic historynya khusus untuk transfer agar lebih rapi (bisa dikembangkan nanti)
            receiver->deposit(amount); 
            cout << "Transfer ke " << receiver->getUsername() << " berhasil!" << endl;
        }
    }
};

// ==========================================
// FUNGSI UTAMA (MAIN)
// ==========================================

int main() {
    // Seed untuk membuat angka acak yang benar-benar berbeda setiap kali run
    srand(time(0)); 

    BankSystem myBank;
    int choice;

    // Loop Menu Utama Bank
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
            
            // Jika login berhasil, masuk ke Menu Nasabah
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