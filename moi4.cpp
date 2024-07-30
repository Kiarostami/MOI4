#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
//#include <xlnt/xlnt.hpp>
using namespace std;

//TODO:
// Test for players.
// Weight for countries: map of countries and effective power.
// Reserve matchmaking for some players and countiers.
// Range for players power.

int getTierValue(const string& tier) {
    if (tier == "S") return 6;
    if (tier == "A") return 5;
    if (tier == "B") return 4;
    if (tier == "C") return 3;
    if (tier == "D") return 2;
    if (tier == "F") return 1;
    return 0;
}

class Player {
private:
    string name;
    map<string, string> countries; // Key: Country, Value: Tier

public:
    Player(const string& playerName, const map<string, string>& playerCountries) {
        name = playerName;
        countries = playerCountries;
    }

    Player() {}

    string getName() const {
        return name;
    }

    map<string, string> getCountries() const {
        return countries;
    }

    void setName(const string& playerName) {
        name = playerName;
    }

    void setCountries(const map<string, string>& playerCountries) {
        countries = playerCountries;
    }

    void displayPlayerDetails() const {
        cout << "Player Name: " << name << endl;
        cout << "Countries and Tiers: " << endl;
        for (const auto& country : countries) {
            cout << country.first << " - " << country.second << endl;
        }
        cout << "----------------------" << endl;
    }

    // Player's total power of all countries:
    int getPlayerTotalPower() const {
        int totalPower = 0;
        for (const auto& country : countries) {
            totalPower += getTierValue(country.second);
        }
        return totalPower;
    }

    // Player's best country:
    pair<string, string> getBestCountry() const {
        pair<string, string> bestCountry;
        int bestTierValue = 0;
        for (const auto& country : countries) {
            int tierValue = getTierValue(country.second);
            if (tierValue > bestTierValue) {
                bestCountry = country;
                bestTierValue = tierValue;
            }
        }
        return bestCountry;
    }

    // Player's average of all countries:
    double getAverageTier() const {
        double total = 0;
        for (const auto& country : countries) {
            total += getTierValue(country.second);
        }
        return total / countries.size();
    }
};

/* vector<Player> readPlayersFromExcel(const string& filename) {
    vector<Player> players;
    xlnt::workbook wb;
    wb.load(filename);
    xlnt::worksheet ws = wb.active_sheet();

    for (auto row : ws.rows(false)) {
        string name = row[0].to_string();
        map<string, string> countries;

        for (size_t i = 1; i < row.length(); i += 2) {
            string country = row[i].to_string();
            string tier = row[i + 1].to_string();
            countries[country] = tier;
        }

        Player player(name, countries);
        players.push_back(player);
    }

    return players;
} */

vector<Player> readPlayersFromTextFile(const string& filename) {
    vector<Player> players;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        string name;
        map<string, string> countries;
        iss >> name;
        string country, tier;
        while (iss >> country >> tier) {
            countries[country] = tier;
        }
        Player player(name, countries);
        players.push_back(player);
    }
    return players;
}

// Calculate total team power based on balance criteria:
double calculateTeamPower(const vector<Player>& team, const string& balanceCriteria) {
    double totalPower = 0;
    for (const auto& player : team) {
        if (balanceCriteria == "total") {
            totalPower += player.getPlayerTotalPower();
        } else if (balanceCriteria == "best") {
            totalPower += getTierValue(player.getBestCountry().second);
        } else if (balanceCriteria == "average") {
            totalPower += player.getAverageTier();
        }
    }
    return totalPower;
}

// Optimized matchmaking function considering different balance criteria
pair<vector<Player>, vector<Player>> matchmakePlayers(vector<Player>& players, const string& balanceCriteria) {
    // Sort by criteria:
    sort(players.begin(), players.end(), [&balanceCriteria](const Player& a, const Player& b) {
        if (balanceCriteria == "total") {
            return a.getPlayerTotalPower() > b.getPlayerTotalPower();
        } else if (balanceCriteria == "best") {
            return getTierValue(a.getBestCountry().second) > getTierValue(b.getBestCountry().second);
        } else if (balanceCriteria == "average") {
            return a.getAverageTier() > b.getAverageTier();
        }
        return false;
    });

    vector<Player> team1, team2;

    // Distribute players:
    for (const auto& player : players) {
        if (calculateTeamPower(team1, balanceCriteria) <= calculateTeamPower(team2, balanceCriteria)) {
            team1.push_back(player);
        } else {
            team2.push_back(player);
        }
    }

    return {team1, team2};
}

int main() {
    string filename = "players.txt";
    vector<Player> players = readPlayersFromTextFile(filename);

    // Choose the balance criteria:
    string balanceCriteria = "total"; // Or "best" or "average"

    // Matchmaking:
    auto teams = matchmakePlayers(players, balanceCriteria);
    vector<Player> team1 = teams.first;
    vector<Player> team2 = teams.second;

    // Display team details:
    cout << "Team 1:" << endl;
    for (const auto& player : team1) {
        player.displayPlayerDetails();
    }

    cout << "Team 2:" << endl;
    for (const auto& player : team2) {
        player.displayPlayerDetails();
    }

    return 0;
}
