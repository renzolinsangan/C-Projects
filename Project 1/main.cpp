#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <sqlite3.h>

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

// SQLite database pointer
sqlite3* db = nullptr;

// ------------------------
// Input helpers
// ------------------------
int getIntInput(const string &prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input! Please enter a number.\n" << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double getDoubleInput(const string &prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input! Please enter a number.\n" << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// ------------------------
// Fetch products from DB
// ------------------------
vector<Product> fetchAllProducts() {
    vector<Product> products;
    const char* sql_select = "SELECT name, category, quantity, price FROM products;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_select, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p;
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        p.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.quantity = sqlite3_column_int(stmt, 2);
        p.price = sqlite3_column_double(stmt, 3);
        products.push_back(p);
    }

    sqlite3_finalize(stmt);
    return products;
}

// ------------------------
// Display Table
// ------------------------
void displayTable() {
    vector<Product> inventory = fetchAllProducts();

    if (inventory.empty()) {
        cout << RED << "\nNo products found!\n" << RESET;
        return;
    }

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

        p.quantity = getIntInput("Enter Quantity: ");
        p.price = getDoubleInput("Enter Price: ");

        const char* sql_insert = "INSERT INTO products (name, category, quantity, price) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, p.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p.category.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, p.quantity);
        sqlite3_bind_double(stmt, 4, p.price);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            cerr << RED << "Error inserting product.\n" << RESET;
        sqlite3_finalize(stmt);

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
    cout << GREEN << "\n===== All Products =====\n" << RESET;
    displayTable();
}

// ------------------------
// Search Records
// ------------------------
void searchRecords() {
    cin.ignore();
    string keyword;

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cout << BLUE << "\nEnter product name to search: " << RESET;
    getline(cin, keyword);

    const char* sql_search = "SELECT name, category, quantity, price FROM products WHERE name LIKE ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_search, -1, &stmt, nullptr);
    string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        cout << GREEN << "\nProduct Found:\n" << RESET;
        cout << "Name: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
             << "\nCategory: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
             << "\nQuantity: " << sqlite3_column_int(stmt, 2)
             << "\nPrice: " << sqlite3_column_double(stmt, 3) << "\n";
    }
    sqlite3_finalize(stmt);

    if (!found)
        cout << RED << "\nNo matching product found.\n" << RESET;

    cout << GREEN << "\n===== Updated Inventory Table =====\n" << RESET;
    displayTable();
}

// ------------------------
// Update Product
// ------------------------
void updateProduct() {
    cin.ignore();
    string name;

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cout << BLUE << "Enter product name to update: " << RESET;
    getline(cin, name);

    Product p;
    cout << "New Name: ";
    getline(cin, p.name);
    cout << "New Category: ";
    getline(cin, p.category);
    p.quantity = getIntInput("New Quantity: ");
    p.price = getDoubleInput("New Price: ");

    const char* sql_update = "UPDATE products SET name=?, category=?, quantity=?, price=? WHERE name=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_update, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, p.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, p.category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, p.quantity);
    sqlite3_bind_double(stmt, 4, p.price);
    sqlite3_bind_text(stmt, 5, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        cerr << RED << "Error updating product.\n" << RESET;
    else
        cout << GREEN << "Product updated successfully!\n" << RESET;

    sqlite3_finalize(stmt);
    displayTable();
}

// ------------------------
// Delete Product
// ------------------------
void deleteProduct() {
    cin.ignore();
    string name;

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cout << BLUE << "Enter product name to delete: " << RESET;
    getline(cin, name);

    const char* sql_delete = "DELETE FROM products WHERE name=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_delete, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        cerr << RED << "Error deleting product.\n" << RESET;
    else
        cout << GREEN << "Product deleted successfully!\n" << RESET;

    sqlite3_finalize(stmt);
    displayTable();
}

// ------------------------
// Process Sales
// ------------------------
void processSales() {
    cin.ignore();
    string name;

    cout << GREEN << "\n===== Current Inventory =====\n" << RESET;
    displayTable();

    cout << BLUE << "Enter product name to sell: " << RESET;
    getline(cin, name);

    int qty;
    cout << "Enter quantity sold: ";
    cin >> qty;

    const char* sql_check = "SELECT quantity FROM products WHERE name=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_check, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        cout << RED << "\nProduct not found.\n" << RESET;
        sqlite3_finalize(stmt);
        return;
    }

    int stock = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (qty > stock) {
        cout << RED << "\nNot enough stock!\n" << RESET;
        return;
    }

    const char* sql_update = "UPDATE products SET quantity=? WHERE name=?;";
    sqlite3_prepare_v2(db, sql_update, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, stock - qty);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        cerr << RED << "Error updating stock.\n" << RESET;

    sqlite3_finalize(stmt);
    cout << GREEN << "\nSale processed successfully!\n" << RESET;
    displayTable();
}

// ------------------------
// Low Stock Alerts
// ------------------------
void lowStockAlerts() {
    const char* sql_low = "SELECT name, quantity FROM products WHERE quantity < 5;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_low, -1, &stmt, nullptr);

    bool any = false;
    cout << YELLOW << "\n===== LOW STOCK PRODUCTS (Qty < 5) =====\n" << RESET;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        any = true;
        cout << RED << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
             << " Qty: " << sqlite3_column_int(stmt, 1) << RESET << "\n";
    }
    sqlite3_finalize(stmt);

    if (!any)
        cout << GREEN << "All stocks are sufficient.\n" << RESET;
}

// ------------------------
// Menu
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
// Main
// ------------------------
int main() {
    // Open DB
    if (sqlite3_open("inventory.db", &db)) {
        cerr << RED << "Can't open database: " << sqlite3_errmsg(db) << RESET << endl;
        return 1;
    }

    // Create table if not exists
    const char* sql_create = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            category TEXT,
            quantity INTEGER,
            price REAL
        );
    )";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql_create, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << RED << "SQL error: " << errMsg << RESET << endl;
        sqlite3_free(errMsg);
    }

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

    sqlite3_close(db);
    return 0;
}