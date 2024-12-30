#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

// Karakter sınıfı, oyuncu ve düşmanların ortak özelliklerini tanımlar
class Character {
protected:
    string name; // Karakterin adı
    int health;  // Karakterin sağlık puanı
    int attackPower; // Karakterin saldırı gücü

public:
    // Karakter sınıfı için yapıcı fonksiyon
    Character(string name, int health, int attackPower)
        : name(name), health(health), attackPower(attackPower) {}

    // Sağlık değerine erişim sağlayan getter
    int getHealth() const {
        return health;
    }

    // İsme erişim sağlayan getter
    string getName() const {
        return name;
    }

    // Hasar alma fonksiyonu
    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0; // Sağlık negatif olamaz
    }

    // Saldırı fonksiyonu
    virtual void attack(Character &other) {
        int damage = rand() % attackPower + 1; // Rastgele hasar
        other.takeDamage(damage);             // Güvenli hasar uygulama
        cout << name << " " << other.getName() << " isimli karaktere " << damage << " hasar verdi!\n";
    }

    // Karakterin hayatta olup olmadığını kontrol eder
    bool isAlive() const {
        return health > 0;
    }
};

// Oyuncu sınıfı, karakter sınıfından türetilmiştir ve oyuncuya özel özellikler içerir
class Player : public Character {
private:
    vector<string> inventory; // Oyuncunun envanteri

public:
    // Oyuncu sınıfı için yapıcı fonksiyon
    Player(string name)
        : Character(name, 100, 20) {} // Oyuncunun başlangıç sağlık ve saldırı gücü atanır

    // Oyuncunun sağlık iksiri kullanarak iyileşmesini sağlar
    void heal() {
        auto it = find(inventory.begin(), inventory.end(), string("health_potion"));
        if (it != inventory.end()) {
            health += 20; // Sağlık 20 puan artırılır
            inventory.erase(it); // İksir envanterden çıkarılır
            cout << name << " bir can iksiri kullandi ve 20 can yeniledi!\n";
        } else {
            cout << "Can iksiri yok!\n";
        }
    }

    // Oyuncunun envanterine bir eşya ekler
    void addItem(string item) {
        inventory.push_back(item);
        cout << item << " buldunuz!\n";
    }

    // Oyuncunun saldırısını özelleştir
    void attack(Character &other) override {
        int damage = (rand() % attackPower + 1) + 5; // Bonuslu saldırı
        other.takeDamage(damage);                    // Güvenli hasar uygulama
        cout << name << " " << other.getName() << " isimli karaktere " << damage << " hasar verdi (Bonuslu)!\n";
    }
};

// Düşman sınıfı, karakter sınıfından türetilmiştir
class Enemy : public Character {
public:
    // Düşman sınıfı için yapıcı fonksiyon
    Enemy(string name, int health, int attackPower)
        : Character(name, health, attackPower) {}

    // Düşmanın saldırısını özelleştir
    void attack(Character &other) override {
        int critical = rand() % 2 == 0 ? 10 : 0; // %50 şansla kritik vuruş
        int damage = (rand() % attackPower + 1) + critical;
        other.takeDamage(damage);                // Güvenli hasar uygulama
        cout << name << " " << other.getName() << " isimli karaktere " << damage << " hasar verdi (Kritik: " << critical << ")!\n";
    }

    // Eşitlik operatörü: İki düşmanın aynı olup olmadığını kontrol eder
    bool operator==(const Enemy &other) const {
        return name == other.name && health == other.health && attackPower == other.attackPower;
    }
};

// Oyun sınıfı, oyunun genel işleyişini kontrol eder
class Game {
private:
    Player player; // Oyuncu nesnesi
    vector<Enemy> enemies; // Düşmanların listesi
    bool running; // Oyunun devam edip etmediğini belirten bayrak

    // Düşman karşılaşması sırasında yapılan işlemleri yönetir
    void encounterEnemy() {
        if (enemies.empty()) {
            cout << "Dusman kalmadi! Kazandiniz!\n";
            running = false;
            return;
        }

        Enemy &enemy = enemies[rand() % enemies.size()]; // Rastgele bir düşman seçilir
        cout << "Bir " << enemy.getName() << " ortaya cikti!\n";

        while (enemy.isAlive() && player.isAlive()) {
            // Oyuncunun ve düşmanın sağlık durumunu yazdırır
            cout << "Oyuncu Can: " << player.getHealth() << "\n";
            cout << enemy.getName() << " Can: " << enemy.getHealth() << "\n";

            // Oyuncunun yapacağı eylemi seçmesini ister
            cout << "Bir secim yapin: (saldir/iyiles/kac): ";
            string action;
            cin >> action;

            if (action == "saldir") {
                player.attack(enemy); // Oyuncu düşmana saldırır
                if (enemy.isAlive()) {
                    enemy.attack(player); // Düşman hayatta ise oyuncuya saldırır
                }
            } else if (action == "iyiles") {
                player.heal(); // Oyuncu iyileşir
                if (enemy.isAlive()) {
                    enemy.attack(player); // Düşman hayatta ise oyuncuya saldırır
                }
            } else if (action == "kac") {
                cout << player.getName() << " " << enemy.getName() << " isimli dusmandan kacti!\n";
                return; // Oyuncu karşılaşmadan kaçar
            } else {
                cout << "Gecersiz secim!\n";
            }
        }

        if (!enemy.isAlive()) {
            cout << enemy.getName() << " yok edildi!\n";
            enemies.erase(remove(enemies.begin(), enemies.end(), enemy), enemies.end()); // Düşman listeden çıkarılır
            player.addItem("health_potion"); // Oyuncuya bir sağlık iksiri verilir
        }
    }

public:
    // Oyun sınıfı için yapıcı fonksiyon
    Game(string playerName)
        : player(playerName), running(true) {
        // Oyunun başında üç düşman oluşturulur
        enemies.push_back(Enemy("Goblin", 50, 10));
        enemies.push_back(Enemy("Ork", 80, 15));
        enemies.push_back(Enemy("Ejderha", 120, 25));
    }

    // Oyunun ana döngüsünü kontrol eder
    void play() {
        cout << "Hos geldiniz, " << player.getName() << "! Maceraniz basliyor...\n";
        while (running && player.isAlive()) {
            cout << "Ne yapmak istersiniz? (kesfet/cik): ";
            string command;
            cin >> command;

            if (command == "kesfet") {
                encounterEnemy(); // Oyuncu düşmanla karşılaşır
            } else if (command == "cik") {
                running = false; // Oyun sona erer
                cout << "Oynadiginiz icin tesekkurler!\n";
            } else {
                cout << "Gecersiz komut!\n";
            }
        }

        if (!player.isAlive()) {
            cout << "Maglup oldunuz. Oyun bitti.\n";
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Rastgelelik için zaman tabanlı seed

    cout << "Karakterinizin ismini girin: ";
    string playerName;
    cin >> playerName;

    Game game(playerName); // Oyun başlatılır
    game.play(); // Oyun oynanır

    return 0;
}
