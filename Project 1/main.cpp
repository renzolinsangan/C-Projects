#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip> // setw, setprecision
using namespace std;

// Color codes for styling
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[97m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

// Product structure
struct Product {
    string id;
    string name;
    string category;
    int quantity;
    double price;
};

// Global inventory
vector<Product> inventory = {
    {"P001", "Laptop", "Electronics", 10, 1200.0},
    {"P002", "Mouse", "Accessories", 50, 25.0},
    {"P003", "Chair", "Furniture", 5, 75.0}
};

// ------------------------
// Display Table
// ------------------------
void displayTable() {
    int idWidth = 12, nameWidth = 15, catWidth = 12, qtyWidth = 5, priceWidth = 8;
    int tableWidth = idWidth + nameWidth + catWidth + qtyWidth + priceWidth + 6;

    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.size()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    cout << "+" << string(tableWidth + 3, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(idWidth) << "Product ID"
         << "| " << setw(nameWidth) << "Product Name"
         << "| " << setw(catWidth) << "Category"
         << "| " << setw(qtyWidth) << "Qty"
         << "| " << setw(priceWidth) << "Price"
         << "|\n" << RESET;
    cout << "+" << string(tableWidth + 3, '-') << "+\n";

    for (auto &p : inventory) {
        vector<string> idLines = wrapText(p.id, idWidth);
        vector<string> nameLines = wrapText(p.name, nameWidth);
        vector<string> catLines = wrapText(p.category, catWidth);
        vector<string> qtyLines = wrapText(to_string(p.quantity), qtyWidth);
        vector<string> priceLines = wrapText(to_string(p.price), priceWidth);

        size_t maxLines = idLines.size();
        maxLines = max(maxLines, nameLines.size());
        maxLines = max(maxLines, catLines.size());
        maxLines = max(maxLines, qtyLines.size());
        maxLines = max(maxLines, priceLines.size());

        for (size_t i = 0; i < maxLines; i++) {
            cout << "| "
                 << left << setw(idWidth) << (i < idLines.size() ? idLines[i] : "")
                 << "| " << setw(nameWidth) << (i < nameLines.size() ? nameLines[i] : "")
                 << "| " << setw(catWidth) << (i < catLines.size() ? catLines[i] : "")
                 << "| " << setw(qtyWidth) << (i < qtyLines.size() ? qtyLines[i] : "")
                 << "| " << setw(priceWidth) << (i < priceLines.size() ? priceLines[i] : "")
                 << "|\n";
        }

        cout << "+" << string(tableWidth + 3, '-') << "+\n";
    }
}

// ------------------------
// Add Product (Static)
// ------------------------
void addProduct() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        Product p;
        bool valid;

        cout << GREEN << "\n===== Add New Product =====\n" << RESET;

        // Product ID input
        do {
            valid = true;
            cout << "Enter Product ID: ";
            cin >> p.id;

            for (auto &item : inventory) {
                if (item.id == p.id) {
                    cout << RED << "ID already exists! Enter a different ID.\n" << RESET;
                    valid = false;
                    break;
                }
            }
        } while (!valid);

        cin.ignore();
        cout << "Enter Product Name: ";
        getline(cin, p.name);
        cout << "Enter Product Category: ";
        getline(cin, p.category);

        cout << "Enter Quantity: ";
        cin >> p.quantity;
        cout << "Enter Price: ";
        cin >> p.price;

        inventory.push_back(p);
        cout << GREEN << "\nProduct added successfully!\n" << RESET;

        displayTable();

        cout << "\nAdd another product? (Y/N): ";
        cin >> more;
        cin.ignore();
    }
}

// ------------------------
// Other functions remain the same but without DB connection
// ------------------------
void viewAllProducts() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found!\n" << RESET;
        return;
    }
    cout << GREEN << "\n===== All Products =====\n" << RESET;
    displayTable();
}

// ... searchRecords, updateProduct, deleteProduct, processSales, lowStockAlerts
// You can keep them exactly as in your previous code, no DB calls needed

// ------------------------
// Display Menu
// ------------------------
void displayMenu() {
    const int boxWidth = 60;
    const int menuIndent = 15;

    auto printLine = [&](const string &text, const string &color = "") {
        int spaceAfter = boxWidth - menuIndent - text.length();
        cout << CYAN << "|"
             << string(menuIndent, ' ')
             << color << text << RESET
             << CYAN << string(spaceAfter, ' ')
             << "|\n" << CYAN;
    };

    cout << CYAN << "+" << string(boxWidth, '-') << "+\n";

    string title = "INVENTORY MANAGEMENT SYSTEM";
    int titlePadding = (boxWidth - title.length()) / 2;
    cout << CYAN << "|"
         << string(titlePadding, ' ')
         << BOLD << title << RESET
         << string(boxWidth - titlePadding - title.length(), ' ')
         << CYAN << "|\n";

    cout << "+" << CYAN << string(boxWidth, '-') << "+\n" << CYAN;

    printLine("[A] Add New Products", YELLOW);
    printLine("[B] View All Products", YELLOW);
    printLine("[C] Search Records", YELLOW);
    printLine("[D] Update Product Details", YELLOW);
    printLine("[E] Delete Product Details", YELLOW);
    printLine("[F] Process Sales", YELLOW);
    printLine("[G] Display Low Stock Alerts", YELLOW);
    printLine("[X] Exit Program", YELLOW);

    cout << CYAN << "+" << string(boxWidth, '-') << "+\n" << RESET;
}

// ------------------------
// Get User Choice
// ------------------------
char getUserChoice() {
    char choice;
    char confirm;

    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        choice = toupper(choice);

        if ((choice >= 'A' && choice <= 'G') || choice == 'X') {
            cout << "You entered: " << GREEN << choice << RESET;
            cout << "\nConfirm? (Y/N): ";
            cin >> confirm;
            confirm = toupper(confirm);
            if (confirm == 'Y') return choice;
            cout << RED << "\nChoice canceled. Enter again.\n\n" << RESET;
        } else {
            cout << RED << "\nInvalid option! Please enter A-G or X.\n\n" << RESET;
        }
    }
}

// ------------------------
// Main
// ------------------------
int main() {
    char choice;

    do {
        displayMenu();
        choice = getUserChoice();

        switch (choice) {
            case 'A': addProduct(); break;
            case 'B': viewAllProducts(); break;
            case 'C': /*searchRecords();*/ break;
            case 'D': /*updateProduct();*/ break;
            case 'E': /*deleteProduct();*/ break;
            case 'F': /*processSales();*/ break;
            case 'G': /*lowStockAlerts();*/ break;
            case 'X':
                cout << MAGENTA << "\nExiting program... Goodbye!\n" << RESET;
                break;
        }

        if (choice != 'X') {
            cout << "\nPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            cout << "\n\n";
        }

    } while (choice != 'X');

    return 0;
}