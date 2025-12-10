#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sqlite3.h>
#include <algorithm>

using namespace std;

// Color codes
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

// SQLite DB pointer
sqlite3* db = nullptr;

// ------------------------
// Helper: Clear input buffer
// ------------------------
void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------
// RESIDENTS
// ------------------------
void displayResidentsTable() {
    const char* sql_select = "SELECT name, address, contact FROM residents;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << RED << "Failed to fetch residents.\n" << RESET;
        return;
    }

    int nameWidth = 25, addrWidth = 30, contactWidth = 15;
    int totalWidth = nameWidth + addrWidth + contactWidth + 7;

    // Table header
    cout << "+" << string(totalWidth - 2, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(nameWidth) << "Name"
         << "| " << setw(addrWidth) << "Address"
         << "| " << setw(contactWidth) << "Contact"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth - 2, '-') << "+\n";

    // Helper to wrap text into multiple lines
    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.length()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    // Table rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string addr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        vector<string> nameLines = wrapText(name, nameWidth);
        vector<string> addrLines = wrapText(addr, addrWidth);
        vector<string> contactLines = wrapText(contact, contactWidth);

        size_t maxLines = max({nameLines.size(), addrLines.size(), contactLines.size()});

        for (size_t i = 0; i < maxLines; ++i) {
            cout << "| "
                 << left << setw(nameWidth) << (i < nameLines.size() ? nameLines[i] : "")
                 << "| " << setw(addrWidth) << (i < addrLines.size() ? addrLines[i] : "")
                 << "| " << setw(contactWidth) << (i < contactLines.size() ? contactLines[i] : "")
                 << "|\n";
        }

        cout << "+" << string(totalWidth - 2, '-') << "+\n";
    }

    sqlite3_finalize(stmt);
}

void addResident() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        string name, address, contact;
        clearInput();
        cout << "Enter Full Name: ";
        getline(cin, name);

        cout << "Enter Address: ";
        getline(cin, address);

        cout << "Enter Contact Number: ";
        getline(cin, contact);

        const char* sql_insert = "INSERT INTO residents (name, address, contact) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, address.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, contact.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) == SQLITE_DONE)
                cout << GREEN << "\nResident added successfully!\n" << RESET;
            else
                cout << RED << "\nError: Resident might already exist.\n" << RESET;

            sqlite3_finalize(stmt);
        }

        cout << GREEN << "\n===== Updated Resident Records =====\n" << RESET;
        displayResidentsTable();

        cout << "\nAdd another resident? (Y/N): ";
        cin >> more;
    }
}

void updateResident() {
    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string oldName;
    cout << "Enter resident name to update: ";
    getline(cin, oldName);

    string newName, newAddress, newContact;
    cout << "New Full Name (leave blank to keep current): ";
    getline(cin, newName);
    cout << "New Address (leave blank to keep current): ";
    getline(cin, newAddress);
    cout << "New Contact (leave blank to keep current): ";
    getline(cin, newContact);

    const char* sql_update = "UPDATE residents SET name=COALESCE(NULLIF(?,''),name), "
                             "address=COALESCE(NULLIF(?,''),address), "
                             "contact=COALESCE(NULLIF(?,''),contact) WHERE name=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_update, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, newAddress.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, newContact.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, oldName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        cout << GREEN << "\nResident updated successfully!\n" << RESET;
    else
        cout << RED << "\nResident not found or update failed.\n" << RESET;

    sqlite3_finalize(stmt);

    cout << GREEN << "\n===== Updated Resident Records =====\n" << RESET;
    displayResidentsTable();
}

void searchResident() {
    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string keyword;
    cout << "Enter name keyword to search: ";
    getline(cin, keyword);

    const char* sql_search = "SELECT name, address, contact FROM residents WHERE name LIKE ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_search, -1, &stmt, nullptr);
    string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);

    cout << GREEN << "\n===== Search Results =====\n" << RESET;
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string addr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        cout << "Name   : " << name << "\nAddress: " << addr << "\nContact: " << contact << "\n------------------\n";
    }
    if (!found) cout << RED << "No matching residents found.\n" << RESET;
    sqlite3_finalize(stmt);
}

void deleteResident() {
    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string name;
    cout << "Enter resident name to delete: ";
    getline(cin, name);

    const char* sql_delete = "DELETE FROM residents WHERE name=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_delete, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        cout << GREEN << "\nResident deleted successfully!\n" << RESET;
    else
        cout << RED << "\nResident not found or deletion failed.\n" << RESET;

    sqlite3_finalize(stmt);
    cout << GREEN << "\n===== Updated Resident Records =====\n" << RESET;
    displayResidentsTable();
}

// ------------------------
// INCIDENTS
// ------------------------
void displayIncidentsTable() {
    const char* sql_select = "SELECT type, location, date, time, description FROM incidents;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << RED << "Failed to fetch incidents.\n" << RESET;
        return;
    }

    int typeWidth = 15, locWidth = 20, dateWidth = 12, timeWidth = 8, descWidth = 40;
    int totalWidth = typeWidth + locWidth + dateWidth + timeWidth + descWidth + 11;

    // Table header
    cout << "+" << string(totalWidth - 2, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(typeWidth) << "Type"
         << "| " << setw(locWidth) << "Location"
         << "| " << setw(dateWidth) << "Date"
         << "| " << setw(timeWidth) << "Time"
         << "| " << setw(descWidth) << "Description"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth - 2, '-') << "+\n";

    // Helper to wrap text into multiple lines
    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.length()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    // Table rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string loc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        vector<string> typeLines = wrapText(type, typeWidth);
        vector<string> locLines = wrapText(loc, locWidth);
        vector<string> dateLines = wrapText(date, dateWidth);
        vector<string> timeLines = wrapText(time, timeWidth);
        vector<string> descLines = wrapText(desc, descWidth);

        size_t maxLines = max({typeLines.size(), locLines.size(), dateLines.size(), timeLines.size(), descLines.size()});

        for (size_t i = 0; i < maxLines; ++i) {
            cout << "| "
                 << left << setw(typeWidth) << (i < typeLines.size() ? typeLines[i] : "")
                 << "| " << setw(locWidth) << (i < locLines.size() ? locLines[i] : "")
                 << "| " << setw(dateWidth) << (i < dateLines.size() ? dateLines[i] : "")
                 << "| " << setw(timeWidth) << (i < timeLines.size() ? timeLines[i] : "")
                 << "| " << setw(descWidth) << (i < descLines.size() ? descLines[i] : "")
                 << "|\n";
        }

        cout << "+" << string(totalWidth - 2, '-') << "+\n";
    }

    sqlite3_finalize(stmt);
}

void reportIncident() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        string type, location, date, time, description;
        clearInput();
        cout << "Enter Incident Type: ";
        getline(cin, type);
        cout << "Enter Location: ";
        getline(cin, location);
        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, date);
        cout << "Enter Time (HH:MM): ";
        getline(cin, time);
        cout << "Enter Description: ";
        getline(cin, description);

        const char* sql_insert = "INSERT INTO incidents (type, location, date, time, description) VALUES (?,?,?,?,?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, time.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            cout << GREEN << "\nIncident reported successfully!\n" << RESET;
        else
            cout << RED << "\nError reporting incident.\n" << RESET;
        sqlite3_finalize(stmt);

        cout << GREEN << "\n===== Updated Incident Records =====\n" << RESET;
        displayIncidentsTable();

        cout << "\nReport another incident? (Y/N): ";
        cin >> more;
    }
}

// ------------------------
// ANNOUNCEMENTS
// ------------------------
void displayAnnouncementsTable() {
    const char* sql_select = "SELECT title, date, content FROM announcements;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << RED << "Failed to fetch announcements.\n" << RESET;
        return;
    }

    int titleWidth = 25, dateWidth = 12, contentWidth = 40;
    int totalWidth = titleWidth + dateWidth + contentWidth + 7;

    // Table header
    cout << "+" << string(totalWidth - 2, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(titleWidth) << "Title"
         << "| " << setw(dateWidth) << "Date"
         << "| " << setw(contentWidth) << "Content"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth - 2, '-') << "+\n";

    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.length()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    // Table rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        vector<string> titleLines = wrapText(title, titleWidth);
        vector<string> dateLines = wrapText(date, dateWidth);
        vector<string> contentLines = wrapText(content, contentWidth);

        size_t maxLines = max({titleLines.size(), dateLines.size(), contentLines.size()});

        for (size_t i = 0; i < maxLines; ++i) {
            cout << "| "
                 << left << setw(titleWidth) << (i < titleLines.size() ? titleLines[i] : "")
                 << "| " << setw(dateWidth) << (i < dateLines.size() ? dateLines[i] : "")
                 << "| " << setw(contentWidth) << (i < contentLines.size() ? contentLines[i] : "")
                 << "|\n";
        }

        cout << "+" << string(totalWidth - 2, '-') << "+\n";
    }

    sqlite3_finalize(stmt);
}

void addAnnouncement() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        string title, date, content;
        clearInput();
        cout << "Enter Title: ";
        getline(cin, title);
        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, date);
        cout << "Enter Content: ";
        getline(cin, content);

        const char* sql_insert = "INSERT INTO announcements (title, date, content) VALUES (?,?,?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            cout << GREEN << "\nAnnouncement posted successfully!\n" << RESET;
        else
            cout << RED << "\nError posting announcement.\n" << RESET;
        sqlite3_finalize(stmt);

        cout << GREEN << "\n===== Updated Announcements =====\n" << RESET;
        displayAnnouncementsTable();

        cout << "\nAdd another announcement? (Y/N): ";
        cin >> more;
    }
}

// ------------------------
// MENU
// ------------------------
void displayMenu() {
    const int boxWidth = 70;
    const int menuIndent = 21;

    auto printLine = [&](const string &text, const string &color = "") {
        int spaceAfter = boxWidth - menuIndent - (int)text.length();
        if (spaceAfter < 0) spaceAfter = 0;
        cout << CYAN << "|"
             << string(menuIndent, ' ')
             << color << text << RESET
             << CYAN << string(spaceAfter, ' ')
             << "|\n" << CYAN;
    };

    cout << CYAN << "+" << string(boxWidth, '-') << "+\n";

    string title = "BARANGAY MANAGEMENT SYSTEM";
    int titlePadding = (boxWidth - (int)title.length()) / 2;
    cout << CYAN << "|"
         << string(titlePadding, ' ')
         << BOLD << title << RESET
         << string(boxWidth - titlePadding - (int)title.length(), ' ')
         << CYAN << "|\n";

    cout << "+" << string(boxWidth, '-') << "+\n" << CYAN;

    printLine("[A] Add Resident", YELLOW);
    printLine("[B] View All Residents", YELLOW);
    printLine("[C] Update Resident", YELLOW);
    printLine("[D] Search Resident", YELLOW);
    printLine("[E] Delete Resident", YELLOW);

    printLine("[F] Report Incident", YELLOW);
    printLine("[G] View Incidents", YELLOW);

    printLine("[J] Add Announcement", YELLOW);
    printLine("[K] View Announcements", YELLOW);

    printLine("[X] Exit Program", YELLOW);

    cout << "+" << string(boxWidth, '-') << "+\n" << RESET;
}

char getUserChoice() {
    char choice, confirm;
    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        choice = toupper(choice);

        if ((choice >= 'A' && choice <= 'K') || choice == 'X') {
            cout << "You selected: " << GREEN << choice << RESET;
            cout << "\nProceed? (Y/N): ";
            cin >> confirm;

            if (toupper(confirm) == 'Y')
                return choice;

            cout << RED << "\nAction cancelled. Returning to menu...\n\n" << RESET;
        } else {
            cout << RED << "\nInvalid option! Please enter A–K or X.\n\n" << RESET;
        }
    }
}

// ------------------------
// MAIN
// ------------------------
int main() {
    // Open database
    if (sqlite3_open("barangay.db", &db)) {
        cerr << RED << "Can't open database: " << sqlite3_errmsg(db) << RESET << endl;
        return 1;
    }

    // Create tables if not exist
    const char* sql_create_residents = R"(
        CREATE TABLE IF NOT EXISTS residents (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE,
            address TEXT,
            contact TEXT
        );
    )";
    sqlite3_exec(db, sql_create_residents, nullptr, nullptr, nullptr);

    const char* sql_create_incidents = R"(
        CREATE TABLE IF NOT EXISTS incidents (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT,
            location TEXT,
            date TEXT,
            time TEXT,
            description TEXT
        );
    )";
    sqlite3_exec(db, sql_create_incidents, nullptr, nullptr, nullptr);

    const char* sql_create_announcements = R"(
        CREATE TABLE IF NOT EXISTS announcements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT UNIQUE,
            content TEXT,
            date TEXT
        );
    )";
    sqlite3_exec(db, sql_create_announcements, nullptr, nullptr, nullptr);

    char choice;
    do {
        displayMenu();
        choice = getUserChoice();

        switch (choice) {
            case 'A': addResident(); break;
            case 'B': displayResidentsTable(); break;
            case 'C': updateResident(); break;
            case 'D': searchResident(); break;
            case 'E': deleteResident(); break;
            case 'F': reportIncident(); break;
            case 'G': displayIncidentsTable(); break;
            case 'J': addAnnouncement(); break;
            case 'K': displayAnnouncementsTable(); break;
            case 'X':
                cout << MAGENTA << "\nExiting program... Goodbye!\n" << RESET;
                break;
        }

        if (choice != 'X') {
            cout << "\nPress Enter to continue...";
            clearInput();
            cin.get();
            cout << "\n\n";
        }

    } while (choice != 'X');

    sqlite3_close(db);
    return 0;
}