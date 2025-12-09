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
vector<Product> inventory;

// ------------------------
// Display Table
// ------------------------
void displayTable() {
    int idWidth = 12, nameWidth = 15, catWidth = 12, qtyWidth = 5, priceWidth = 8;
    int tableWidth = idWidth + nameWidth + catWidth + qtyWidth + priceWidth + 6; // 6 for separators

    auto wrapText = [](const string &text, int width) -> vector<string> {
        vector<string> lines;
        size_t start = 0;
        while (start < text.size()) {
            lines.push_back(text.substr(start, width));
            start += width;
        }
        return lines;
    };

    // Top line
    cout << "+" << string(tableWidth, '-') << "+\n";

    // Header
    cout << CYAN
         << "| " << left << setw(idWidth) << "Product ID"
         << "| " << setw(nameWidth) << "Product Name"
         << "| " << setw(catWidth) << "Category"
         << "| " << setw(qtyWidth) << "Qty"
         << "| " << setw(priceWidth) << "Price"
         << "|\n" << RESET;

    // Separator
    cout << "+" << string(tableWidth, '-') << "+\n";

    // Rows
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

        cout << "+" << string(tableWidth, '-') << "+\n"; // separator between products
    }

    // Bottom line
    cout << "+" << string(tableWidth, '-') << "+\n";
}

// ------------------------
// Add Product with live table
// ------------------------
void addProduct() {
    char more = 'Y';

    while (toupper(more) == 'Y') {
        Product p;
        bool valid;

        cout << GREEN << "\n===== Add New Product =====\n" << RESET;

        // Input Product ID
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

        // Input Product Name
        do {
            cout << "Enter Product Name: ";
            getline(cin, p.name);
            if (p.name.empty()) cout << RED << "Product Name cannot be empty!\n" << RESET;
        } while (p.name.empty());

        // Input Category
        do {
            cout << "Enter Product Category: ";
            getline(cin, p.category);
            if (p.category.empty()) cout << RED << "Category cannot be empty!\n" << RESET;
        } while (p.category.empty());

        // Quantity
        do {
            cout << "Enter Quantity: ";
            cin >> p.quantity;
            if (cin.fail() || p.quantity <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Invalid input! Quantity must be positive number.\n" << RESET;
                p.quantity = -1;
            }
        } while (p.quantity <= 0);

        // Price
        do {
            cout << "Enter Price: ";
            cin >> p.price;
            if (cin.fail() || p.price <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Invalid input! Price must be positive number.\n" << RESET;
                p.price = -1;
            }
        } while (p.price <= 0);

        // Add to inventory
        inventory.push_back(p);
        cout << GREEN << "\nProduct added successfully!\n" << RESET;

        // Display full table after addition
        displayTable();

        // Ask if user wants to add more
        cout << "\nAdd another product? (Y/N): ";
        cin >> more;
        cin.ignore();
    }
}

// ------------------------
// View All Products
// ------------------------
void viewAllProducts() {
    if (inventory.empty()) {
        cout << RED << "\nNo products found!\n" << RESET;
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
        cout << RED << "\nNo products to search!\n" << RESET;
        return;
    }

    string keyword;
    cin.ignore();
    cout << "\nEnter product ID, name, or category to search: ";
    getline(cin, keyword);

    bool found = false;
    for (auto &p : inventory) {
        if (p.id == keyword || p.name == keyword || p.category == keyword) {
            cout << GREEN << "\nProduct Found:\n" << RESET;
            cout << "ID: " << p.id << "\nName: " << p.name 
                 << "\nCategory: " << p.category 
                 << "\nQuantity: " << p.quantity 
                 << "\nPrice: " << p.price << "\n";
            found = true;
            break;
        }
    }
    if (!found) cout << RED << "\nRecord not found!\n" << RESET;
}

// ------------------------
// Update Product
// ------------------------
void updateProduct() {
    if (inventory.empty()) {
        cout << RED << "\nNo products to update!\n" << RESET;
        return;
    }

    string id;
    cout << "\nEnter Product ID to update: ";
    cin >> id;

    for (auto &p : inventory) {
        if (p.id == id) {
            cin.ignore();
            cout << "Enter new name: ";
            getline(cin, p.name);
            cout << "Enter new category: ";
            getline(cin, p.category);
            cout << "Enter new quantity: ";
            cin >> p.quantity;
            cout << "Enter new price: ";
            cin >> p.price;
            cout << GREEN << "\nProduct updated successfully!\n" << RESET;
            displayTable();
            return;
        }
    }

    cout << RED << "\nProduct ID not found!\n" << RESET;
}

// ------------------------
// Delete Product
// ------------------------
void deleteProduct() {
    if (inventory.empty()) {
        cout << RED << "\nNo products to delete!\n" << RESET;
        return;
    }

    string id;
    cout << "\nEnter Product ID to delete: ";
    cin >> id;

    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->id == id) {
            inventory.erase(it);
            cout << GREEN << "\nProduct deleted successfully!\n" << RESET;
            displayTable();
            return;
        }
    }

    cout << RED << "\nProduct ID not found!\n" << RESET;
}

// ------------------------
// Process Sales
// ------------------------
void processSales() {
    if (inventory.empty()) {
        cout << RED << "\nNo products available!\n" << RESET;
        return;
    }

    string id;
    int qty;
    cout << "\nEnter Product ID to sell: ";
    cin >> id;

    for (auto &p : inventory) {
        if (p.id == id) {
            cout << "Enter quantity sold: ";
            cin >> qty;
            if (qty > p.quantity) {
                cout << RED << "Not enough stock!\n" << RESET;
            } else {
                p.quantity -= qty;
                double total = qty * p.price;
                cout << GREEN << "Sale processed. Total: $" << fixed << setprecision(2) << total << "\n" << RESET;
            }
            displayTable();
            return;
        }
    }
    cout << RED << "\nProduct ID not found!\n" << RESET;
}

// ------------------------
// Low Stock Alerts
// ------------------------
void lowStockAlerts() {
    if (inventory.empty()) {
        cout << RED << "\nNo products available!\n" << RESET;
        return;
    }

    int threshold;
    cout << "\nEnter low stock threshold: ";
    cin >> threshold;

    cout << GREEN << "\nLow Stock Products:\n" << RESET;
    bool found = false;
    for (auto &p : inventory) {
        if (p.quantity < threshold) {
            cout << "ID: " << p.id << ", Name: " << p.name 
                 << ", Qty: " << p.quantity << "\n";
            found = true;
        }
    }
    if (!found) cout << RED << "None\n" << RESET;
}

// ------------------------
// Save / Load (Static Demo)
// ------------------------
void saveToFile() {
    cout << GREEN << "\nSaving inventory to file (simulated)...\n" << RESET;
}

void loadFromFile() {
    cout << GREEN << "\nLoading inventory from file (simulated)...\n" << RESET;
    inventory.push_back({"P001", "Laptop", "Electronics", 10, 1200.0});
    inventory.push_back({"P002", "Mouse", "Accessories", 50, 25.0});
    inventory.push_back({"P003", "Chair", "Furniture", 5, 75.0});
    displayTable();
}

// ------------------------
// Display Menu
// ------------------------
void displayMenu() {
    const int boxWidth = 60; // total width of the box
    const int menuIndent = 15; // shift menu items toward center

    auto printLine = [&](const string &text, const string &color = "") {
        int spaceAfter = boxWidth - menuIndent - text.length(); // spaces after text
        cout << CYAN << "|"
             << string(menuIndent, ' ')
             << color << text << RESET
             << CYAN << string(spaceAfter, ' ')
             << "|\n" << CYAN;
    };

    // Top border
    cout << CYAN << "+" << string(boxWidth, '-') << "+\n";

    // Title (centered)
    string title = "INVENTORY MANAGEMENT SYSTEM";
    int titlePadding = (boxWidth - title.length()) / 2;
    cout << CYAN << "|"
         << string(titlePadding, ' ')
         << BOLD << title << RESET
         << string(boxWidth - titlePadding - title.length(), ' ')
         << CYAN << "|\n";

    // Separator under title
    cout << "+" << CYAN << string(boxWidth, '-') << "+\n" << CYAN;

    // Menu items (all on one line, slightly centered)
    printLine("[A] Add New Products", YELLOW);
    printLine("[B] View All Products", YELLOW);
    printLine("[C] Search Records", YELLOW);
    printLine("[D] Update Product Details", YELLOW);
    printLine("[E] Delete Product Details", YELLOW);
    printLine("[F] Process Sales", YELLOW);
    printLine("[G] Display Low Stock Alerts", YELLOW);
    printLine("[H] Save Inventory to File", YELLOW);
    printLine("[I] Load Inventory Data at Startup", YELLOW);
    printLine("[X] Exit Program", YELLOW);

    // Bottom border
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

        if ((choice >= 'A' && choice <= 'I') || choice == 'X') {
            cout << "You entered: " << GREEN << choice << RESET;
            cout << "\nConfirm? (Y/N): ";
            cin >> confirm;
            confirm = toupper(confirm);
            if (confirm == 'Y') return choice;
            cout << RED << "\nChoice canceled. Enter again.\n\n" << RESET;
        } else {
            cout << RED << "\nInvalid option! Please enter A-I or X.\n\n" << RESET;
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
            case 'C': searchRecords(); break;
            case 'D': updateProduct(); break;
            case 'E': deleteProduct(); break;
            case 'F': processSales(); break;
            case 'G': lowStockAlerts(); break;
            case 'H': saveToFile(); break;
            case 'I': loadFromFile(); break;
            case 'X':
                cout << MAGENTA << "\nExiting program... Goodbye!\n" << RESET;
                break;
        }

        if (choice != 'X') {
            cout << "\nPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush leftover newline
            cin.get(); // wait for Enter
            cout << "\n\n";
        }

    } while (choice != 'X');

    return 0;
}