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

// Product structure
struct Product {
    string name;
    string category;
    int quantity;
    double price;
};

// Global inventory
vector<Product> inventory = {};

// ------------------------
// Display Table
// ------------------------
void displayTable() {
    int nameWidth = 20, catWidth = 15, qtyWidth = 8, priceWidth = 10;
    int tableWidth = nameWidth + catWidth + qtyWidth + priceWidth + 6;

    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.size()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    cout << "+" << string(tableWidth + 1, '-') << "+\n";
    cout << CYAN
         << "| " << left << setw(nameWidth) << "Product Name"
         << "| " << setw(catWidth) << "Category"
         << "| " << setw(qtyWidth) << "Qty"
         << "| " << setw(priceWidth) << "Price"
         << "|\n" << RESET;
    cout << "+" << string(tableWidth + 1, '-') << "+\n";

    for (auto &p : inventory) {
        vector<string> nameLines = wrapText(p.name, nameWidth);
        vector<string> catLines = wrapText(p.category, catWidth);
        vector<string> qtyLines = wrapText(to_string(p.quantity), qtyWidth);
        vector<string> priceLines = wrapText(to_string(p.price), priceWidth);

        size_t maxLines = max({nameLines.size(), catLines.size(), qtyLines.size(), priceLines.size()});

        for (size_t i = 0; i < maxLines; i++) {
            cout << "| "
                 << left << setw(nameWidth) << (i < nameLines.size() ? nameLines[i] : "")
                 << "| " << setw(catWidth) << (i < catLines.size() ? catLines[i] : "")
                 << "| " << setw(qtyWidth) << (i < qtyLines.size() ? qtyLines[i] : "")
                 << "| " << setw(priceWidth) << (i < priceLines.size() ? priceLines[i] : "")
                 << "|\n";
        }

        cout << "+" << string(tableWidth + 1, '-') << "+\n";
    }
}

// ------------------------
// Add Product
// ------------------------
void addProduct() {
    char more = 'Y';
    while (toupper(more) == 'Y') {
        Product p;

        cout << GREEN << "\n===== Add New Product =====\n" << RESET;

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
    }
}

// ------------------------
// View All Products
// ------------------------
void viewAllProducts() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== All Products =====\n" << RESET;
    displayTable();
}

// ------------------------
// Search Records
// ------------------------
void searchRecords() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cin.ignore();
    string keyword;
    cout << BLUE << "\nEnter product name to search: " << RESET;
    getline(cin, keyword);

    bool found = false;
    for (auto &p : inventory) {
        if (p.name.find(keyword) != string::npos) {
            found = true;
            cout << GREEN << "\nProduct Found:\n" << RESET;
            cout << "Name: " << p.name << "\nCategory: " << p.category
                 << "\nQuantity: " << p.quantity << "\nPrice: " << p.price << "\n";
        }
    }

    if (!found)
        cout << RED << "\nNo matching product found.\n" << RESET;

    cout << GREEN << "\n===== Updated Inventory Table =====\n" << RESET;
    displayTable();
}

// ------------------------
// Update Product
// ------------------------
void updateProduct() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cin.ignore();
    string name;
    cout << BLUE << "\nEnter product name to update: " << RESET;
    getline(cin, name);

    for (auto &p : inventory) {
        if (p.name == name) {
            cout << GREEN << "\nUpdating Product...\n" << RESET;

            cout << "New Name: ";
            getline(cin, p.name);

            cout << "New Category: ";
            getline(cin, p.category);

            cout << "New Quantity: ";
            cin >> p.quantity;

            cout << "New Price: ";
            cin >> p.price;

            cout << GREEN << "\nProduct updated successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Inventory Table =====\n" << RESET;
            displayTable();
            return;
        }
    }

    cout << RED << "\nProduct not found.\n" << RESET;
    displayTable();
}

// ------------------------
// Delete Product
// ------------------------
void deleteProduct() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cin.ignore();
    string name;
    cout << BLUE << "\nEnter product name to delete: " << RESET;
    getline(cin, name);

    for (size_t i = 0; i < inventory.size(); i++) {
        if (inventory[i].name == name) {
            inventory.erase(inventory.begin() + i);
            cout << GREEN << "\nProduct deleted successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Inventory Table =====\n" << RESET;
            displayTable();
            return;
        }
    }

    cout << RED << "\nProduct not found.\n" << RESET;
    displayTable();
}

// ------------------------
// Process Sales
// ------------------------
void processSales() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cin.ignore();
    string name;
    cout << BLUE << "\nEnter product name to sell: " << RESET;
    getline(cin, name);

    for (auto &p : inventory) {
        if (p.name == name) {
            int qty;
            cout << "Enter quantity sold: ";
            cin >> qty;

            if (qty > p.quantity) {
                cout << RED << "\nNot enough stock!\n" << RESET;
                displayTable();
                return;
            }

            p.quantity -= qty;
            cout << GREEN << "\nSale processed successfully!\n" << RESET;

            cout << GREEN << "\n===== Updated Inventory Table =====\n" << RESET;
            displayTable();
            return;
        }
    }

    cout << RED << "\nProduct not found.\n" << RESET;
    displayTable();
}

// ------------------------
// Low Stock Alerts
// ------------------------
void lowStockAlerts() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found! Add products first.\n" << RESET;
        return;
    }

    cout << YELLOW << "\n===== LOW STOCK PRODUCTS (Qty < 5) =====\n" << RESET;

    bool any = false;
    for (auto &p : inventory) {
        if (p.quantity < 5) {
            any = true;
            cout << RED << p.name << " Qty: " << p.quantity << RESET << "\n";
        }
    }

    if (!any)
        cout << GREEN << "All stocks are sufficient.\n" << RESET;
}

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
char getUserChoice() {
    char choice, confirm;

    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        choice = toupper(choice);

        if ((choice >= 'A' && choice <= 'G') || choice == 'X') {
            cout << "You entered: " << GREEN << choice << RESET;
            cout << "\nConfirm? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') return choice;
            cout << RED << "\nChoice canceled. Enter again.\n\n" << RESET;
        } else {
            cout << RED << "\nInvalid option! Please enter A-G or X.\n\n" << RESET;
        }
    }
}

// ------------------------
int main() {
    char choice;

    do {
        displayMenu();
        choice = getUserChoice();

        switch (choice) {
            case 'A': addProduct(); break;
            case 'B': viewAllProducts(); break;
            case 'C': searchRecords(); break;
            case 'D': updateProduct(); break;
            case 'E': deleteProduct(); break;
            case 'F': processSales(); break;
            case 'G': lowStockAlerts(); break;
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