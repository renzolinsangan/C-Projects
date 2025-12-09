#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>
using namespace std;

// Color codes
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[97m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

// ------------------------
// Data Structures
// ------------------------

struct Resident {
    string name;
    string address;
    string contact;
};

struct Incident {
    string type;
    string location;
    string date;
    string time;
    string description;
};

struct Announcement {
    string title;
    string content;
    string date;
};

// Global storage (in-memory only, resets on exit)
vector<Resident> residents;
vector<Incident> incidents;
vector<Announcement> announcements;

// ------------------------
// Helper: Clear input buffer
// ------------------------
void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------
// Resident Management
// ------------------------

void displayResidentsTable() {
    if (residents.empty()) {
        cout << RED << "\nNo resident records found.\n" << RESET;
        return;
    }

    int nameWidth = 25, addrWidth = 30, contactWidth = 15;
    int totalWidth = nameWidth + addrWidth + contactWidth + 7;

    cout << "+" << string(totalWidth - 2, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(nameWidth) << "Name"
         << "| " << setw(addrWidth) << "Address"
         << "| " << setw(contactWidth) << "Contact"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth - 2, '-') << "+\n";

    for (auto &r : residents) {
        cout << "| " << left << setw(nameWidth) << r.name
             << "| " << setw(addrWidth) << r.address
             << "| " << setw(contactWidth) << r.contact
             << "|\n";
    }

    cout << "+" << string(totalWidth - 2, '-') << "+\n";
}

bool residentExists(const string &name) {
    for (auto &r : residents) {
        if (r.name == name) return true;
    }
    return false;
}

void addResident() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        Resident r;

        cout << GREEN << "\n===== Add New Resident =====\n" << RESET;

        clearInput();
        cout << "Enter Full Name: ";
        getline(cin, r.name);

        if (residentExists(r.name)) {
            cout << RED << "Resident with this name already exists. Entry cancelled.\n" << RESET;
        } else {
            cout << "Enter Address: ";
            getline(cin, r.address);

            cout << "Enter Contact Number: ";
            getline(cin, r.contact);

            residents.push_back(r);
            cout << GREEN << "\nResident added successfully!\n" << RESET;
        }

        cout << GREEN << "\n===== Resident Records =====\n" << RESET;
        displayResidentsTable();

        cout << "\nAdd another resident? (Y/N): ";
        cin >> more;
    }
}

void viewResidents() {
    cout << GREEN << "\n===== Resident Records =====\n" << RESET;
    displayResidentsTable();
}

void updateResident() {
    if (residents.empty()) {
        cout << RED << "\nNo residents to update.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string name;
    cout << BLUE << "\nEnter resident name to update: " << RESET;
    getline(cin, name);

    for (auto &r : residents) {
        if (r.name == name) {
            cout << GREEN << "\nUpdating Resident...\n" << RESET;

            cout << "New Full Name (leave blank to keep current): ";
            string newName;
            getline(cin, newName);
            if (!newName.empty()) r.name = newName;

            cout << "New Address (leave blank to keep current): ";
            string newAddress;
            getline(cin, newAddress);
            if (!newAddress.empty()) r.address = newAddress;

            cout << "New Contact (leave blank to keep current): ";
            string newContact;
            getline(cin, newContact);
            if (!newContact.empty()) r.contact = newContact;

            cout << GREEN << "\nResident updated successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Resident Records =====\n" << RESET;
            displayResidentsTable();
            return;
        }
    }

    cout << RED << "\nResident not found.\n" << RESET;
}

void searchResident() {
    if (residents.empty()) {
        cout << RED << "\nNo residents to search.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string keyword;
    cout << BLUE << "\nEnter name keyword to search: " << RESET;
    getline(cin, keyword);

    bool found = false;
    cout << GREEN << "\n===== Search Results =====\n" << RESET;
    for (auto &r : residents) {
        if (r.name.find(keyword) != string::npos) {
            found = true;
            cout << "Name   : " << r.name << "\n";
            cout << "Address: " << r.address << "\n";
            cout << "Contact: " << r.contact << "\n";
            cout << "-------------------------\n";
        }
    }

    if (!found)
        cout << RED << "No matching residents found.\n" << RESET;
}

void deleteResident() {
    if (residents.empty()) {
        cout << RED << "\nNo residents to delete.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Residents =====\n" << RESET;
    displayResidentsTable();

    clearInput();
    string name;
    cout << BLUE << "\nEnter resident name to delete: " << RESET;
    getline(cin, name);

    for (size_t i = 0; i < residents.size(); ++i) {
        if (residents[i].name == name) {
            residents.erase(residents.begin() + i);
            cout << GREEN << "\nResident deleted successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Resident Records =====\n" << RESET;
            displayResidentsTable();
            return;
        }
    }

    cout << RED << "\nResident not found.\n" << RESET;
}

// ------------------------
// Incident Reporting
// ------------------------

void displayIncidentsTable() {
    if (incidents.empty()) {
        cout << RED << "\nNo incident reports found.\n" << RESET;
        return;
    }

    int typeWidth = 15, locWidth = 20, dateWidth = 12, timeWidth = 8;
    int totalWidth = typeWidth + locWidth + dateWidth + timeWidth + 9;

    cout << "+" << string(totalWidth, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(typeWidth) << "Type"
         << "| " << setw(locWidth) << "Location"
         << "| " << setw(dateWidth) << "Date"
         << "| " << setw(timeWidth) << "Time"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth, '-') << "+\n";

    for (auto &i : incidents) {
        cout << "| " << left << setw(typeWidth) << i.type
             << "| " << setw(locWidth) << i.location
             << "| " << setw(dateWidth) << i.date
             << "| " << setw(timeWidth) << i.time
             << "|\n";
    }

    cout << "+" << string(totalWidth, '-') << "+\n";
}

void reportIncident() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        Incident inc;

        cout << GREEN << "\n===== Report New Incident =====\n" << RESET;

        clearInput();
        cout << "Enter Incident Type (e.g., Crime, Accident): ";
        getline(cin, inc.type);

        cout << "Enter Location: ";
        getline(cin, inc.location);

        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, inc.date);

        cout << "Enter Time (HH:MM): ";
        getline(cin, inc.time);

        cout << "Enter Description: ";
        getline(cin, inc.description);

        incidents.push_back(inc);
        cout << GREEN << "\nIncident reported successfully!\n" << RESET;

        cout << GREEN << "\n===== Incident Reports =====\n" << RESET;
        displayIncidentsTable();

        cout << "\nReport another incident? (Y/N): ";
        cin >> more;
    }
}

void viewIncidents() {
    cout << GREEN << "\n===== Incident Reports =====\n" << RESET;
    displayIncidentsTable();

    if (!incidents.empty()) {
        cout << BLUE << "\nDetails of each incident:\n" << RESET;
        for (size_t i = 0; i < incidents.size(); ++i) {
            auto &inc = incidents[i];
            cout << "\n[" << i + 1 << "] " << inc.type << " at " << inc.location << "\n";
            cout << "Date: " << inc.date << " Time: " << inc.time << "\n";
            cout << "Description: " << inc.description << "\n";
        }
    }
}

void searchIncident() {
    if (incidents.empty()) {
        cout << RED << "\nNo incidents to search.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Incidents =====\n" << RESET;
    displayIncidentsTable();

    clearInput();
    string keyword;
    cout << BLUE << "\nEnter incident type or location keyword to search: " << RESET;
    getline(cin, keyword);

    bool found = false;
    cout << GREEN << "\n===== Search Results =====\n" << RESET;
    for (auto &inc : incidents) {
        if (inc.type.find(keyword) != string::npos ||
            inc.location.find(keyword) != string::npos) {
            found = true;
            cout << "Type       : " << inc.type << "\n";
            cout << "Location   : " << inc.location << "\n";
            cout << "Date/Time  : " << inc.date << " " << inc.time << "\n";
            cout << "Description: " << inc.description << "\n";
            cout << "-------------------------\n";
        }
    }

    if (!found)
        cout << RED << "No matching incidents found.\n" << RESET;
}

void deleteIncident() {
    if (incidents.empty()) {
        cout << RED << "\nNo incidents to delete.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Incidents =====\n" << RESET;
    displayIncidentsTable();

    clearInput();
    string keyword;
    cout << BLUE << "\nEnter incident type or location to delete: " << RESET;
    getline(cin, keyword);

    for (size_t i = 0; i < incidents.size(); ++i) {
        if (incidents[i].type == keyword || incidents[i].location == keyword) {
            incidents.erase(incidents.begin() + i);
            cout << GREEN << "\nIncident deleted successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Incident Reports =====\n" << RESET;
            displayIncidentsTable();
            return;
        }
    }

    cout << RED << "\nIncident not found.\n" << RESET;
}

// ------------------------
// Announcements
// ------------------------

void displayAnnouncementsTable() {
    if (announcements.empty()) {
        cout << RED << "\nNo announcements found.\n" << RESET;
        return;
    }

    int titleWidth = 25, dateWidth = 12;
    int totalWidth = titleWidth + dateWidth + 5;

    cout << "+" << string(totalWidth, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(titleWidth) << "Title"
         << "| " << setw(dateWidth) << "Date"
         << "|\n" << RESET;
    cout << "+" << string(totalWidth, '-') << "+\n";

    for (auto &a : announcements) {
        cout << "| " << left << setw(titleWidth) << a.title
             << "| " << setw(dateWidth) << a.date
             << "|\n";
    }

    cout << "+" << string(totalWidth, '-') << "+\n";
}

void addAnnouncement() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        Announcement a;

        cout << GREEN << "\n===== Add New Announcement =====\n" << RESET;

        clearInput();
        cout << "Enter Title: ";
        getline(cin, a.title);

        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, a.date);

        cout << "Enter Content: ";
        getline(cin, a.content);

        announcements.push_back(a);
        cout << GREEN << "\nAnnouncement posted successfully!\n" << RESET;

        cout << GREEN << "\n===== Announcements =====\n" << RESET;
        displayAnnouncementsTable();

        cout << "\nAdd another announcement? (Y/N): ";
        cin >> more;
    }
}

void viewAnnouncements() {
    cout << GREEN << "\n===== Announcements =====\n" << RESET;
    displayAnnouncementsTable();

    if (!announcements.empty()) {
        cout << BLUE << "\nDetails of each announcement:\n" << RESET;
        for (size_t i = 0; i < announcements.size(); ++i) {
            auto &a = announcements[i];
            cout << "\n[" << i + 1 << "] " << a.title << " (" << a.date << ")\n";
            cout << "Content: " << a.content << "\n";
        }
    }
}

void deleteAnnouncement() {
    if (announcements.empty()) {
        cout << RED << "\nNo announcements to delete.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Announcements =====\n" << RESET;
    displayAnnouncementsTable();

    clearInput();
    string title;
    cout << BLUE << "\nEnter announcement title to delete: " << RESET;
    getline(cin, title);

    for (size_t i = 0; i < announcements.size(); ++i) {
        if (announcements[i].title == title) {
            announcements.erase(announcements.begin() + i);
            cout << GREEN << "\nAnnouncement deleted successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Announcements =====\n" << RESET;
            displayAnnouncementsTable();
            return;
        }
    }

    cout << RED << "\nAnnouncement not found.\n" << RESET;
}

// ------------------------
// Main Menu
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
    printLine("[H] Search Incident", YELLOW);
    printLine("[I] Delete Incident", YELLOW);

    printLine("[J] Add Announcement", YELLOW);
    printLine("[K] View Announcements", YELLOW);
    printLine("[L] Delete Announcement", YELLOW);

    printLine("[X] Exit Program", YELLOW);

    cout << "+" << string(boxWidth, '-') << "+\n" << RESET;
}

char getUserChoice() {
    char choice, confirm;

    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        choice = toupper(choice);

        if ((choice >= 'A' && choice <= 'L') || choice == 'X') {
            cout << "You selected: " << GREEN << choice << RESET;
            cout << "\nProceed? (Y/N): ";
            cin >> confirm;

            if (toupper(confirm) == 'Y')
                return choice;

            cout << RED << "\nAction cancelled. Returning to menu...\n\n" << RESET;
        } else {
            cout << RED << "\nInvalid option! Please enter A–L or X.\n\n" << RESET;
        }
    }
}

// ------------------------
// main
// ------------------------

int main() {
    char choice;

    do {
        displayMenu();
        choice = getUserChoice();

        switch (choice) {
            case 'A': addResident(); break;
            case 'B': viewResidents(); break;
            case 'C': updateResident(); break;
            case 'D': searchResident(); break;
            case 'E': deleteResident(); break;

            case 'F': reportIncident(); break;
            case 'G': viewIncidents(); break;
            case 'H': searchIncident(); break;
            case 'I': deleteIncident(); break;

            case 'J': addAnnouncement(); break;
            case 'K': viewAnnouncements(); break;
            case 'L': deleteAnnouncement(); break;

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

    return 0;
}
