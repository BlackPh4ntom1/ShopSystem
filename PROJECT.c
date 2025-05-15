#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <conio.h>

#define MAX_NAME_LENGTH 50
#define MAX_PRODUCTS 100
#define MAX_ORDERS 100
#define MAX_USERS 100

// Structure to represent a product
typedef struct {
    char name[MAX_NAME_LENGTH];
    int quantity;
    float price;
} Product;

// Structure to represent an order
typedef struct {
    char productName[MAX_NAME_LENGTH];
    int quantity;
} Order;

// Structure to represent a user
typedef struct {
    char username[MAX_NAME_LENGTH];
    char password[MAX_NAME_LENGTH];
} User;

// Global variables
Product products[MAX_PRODUCTS]; // Assuming a fixed number of products, for simplicity
Order orders[MAX_ORDERS]; // Assuming a fixed number of orders, for simplicity
User users[MAX_USERS];
int numUsers = 0;
int numProducts = 0;
int numOrders = 0;
bool isLoggedIn = false; // Variable to track user login status

// Function prototypes
void login();
void registerUser();
void placeOrder();
void sellProducts();
void viewOrders();
void searchProducts();
int authenticateUser(const char *username, const char *password);
void loadUsers();
void saveUsers();
void loadProducts();
void saveProducts();
void saveOrders();

int main() {
    int choice;

    loadUsers(); // Load existing users from file
    loadProducts(); // Load existing products from file

    do {
        printf("\n======= Online Shop Menu =======\n");
        printf("1. Login\n");
        printf("2. Register\n");
        if (isLoggedIn) {
            printf("3. Place Order\n");
            printf("4. Sell Products\n");
            printf("5. View Orders\n");
            printf("6. Search Products\n");
        }
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                if (isLoggedIn)
                    placeOrder();
                else
                    printf("Please login first.\n");
                break;
            case 4:
                if (isLoggedIn)
                    sellProducts();
                else
                    printf("Please login first.\n");
                break;
            case 5:
                if (isLoggedIn)
                    viewOrders();
                else
                    printf("Please login first.\n");
                break;
            case 6:
                if (isLoggedIn)
                    searchProducts();
                else
                    printf("Please login first.\n");
                break;
            case 7:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice! Please enter a number between 1 and 7.\n");
        }
       
    } while (choice != 7);

    saveUsers(); // Save users to file before exiting
    saveProducts(); // Save products to file before exiting
    saveOrders(); // Save orders to file before exiting

    return 0;
}

void login() {
    char username[MAX_NAME_LENGTH];
    char password[MAX_NAME_LENGTH];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    if (authenticateUser(username, password)) {
        printf("Login successful!\n");
        isLoggedIn = true; // Update login status
        // Proceed with shop functionalities for authenticated user
    } else {
        printf("Invalid username or password. Please try again.\n");
    }
}

void registerUser() {
    if (numUsers >= MAX_USERS) {
        printf("Cannot register more users. Maximum limit reached.\n");
        return;
    }

    char username[MAX_NAME_LENGTH];
    char password[MAX_NAME_LENGTH];

    printf("Enter username: ");
    scanf("%s", username);

    // Check if the username already exists
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists. Please choose a different username.\n");
            return;
        }
    }

    printf("Enter password: ");
    scanf("%s", password);

    // Add the new user
    strcpy(users[numUsers].username, username);
    strcpy(users[numUsers].password, password);
    numUsers++;

    printf("User registered successfully!\n");
}



void placeOrder() {
    char productName[MAX_NAME_LENGTH];
    int quantity;

    printf("Enter product name: ");
    scanf("%s", productName);

    // Find the product in the array of products
    int productIndex = -1;
    for (int i = 0; i < numProducts; i++) {
        if (strcmp(products[i].name, productName) == 0) {
            productIndex = i;
            break;
        }
    }

    if (productIndex != -1) {
        printf("Enter quantity: ");
        scanf("%d", &quantity);

        // Check if the entered quantity is valid
        if (quantity <= 0) {
            printf("Invalid quantity. Please enter a positive integer.\n");
            return;
        }

        // Check if there is enough quantity available
        if (products[productIndex].quantity >= quantity) {
            // Add the order to the array of orders
            strcpy(orders[numOrders].productName, productName);
            orders[numOrders].quantity = quantity;
            numOrders++;

            // Decrease the quantity of the product
            products[productIndex].quantity -= quantity;

            // Save the updated product data back to the CSV file
            saveProducts();

            printf("Order placed successfully!\n");
        } else {
            printf("Sorry, the requested quantity is not available.\n");
        }
    } else {
        printf("Product not found.\n");
    }
}



void sellProducts() {
    char productName[MAX_NAME_LENGTH];
    int quantity;
    float price;

    if (numProducts >= MAX_PRODUCTS) {
        printf("Cannot add more products. Maximum limit reached.\n");
        return;
    }

    printf("Enter product name: ");
    scanf("%s", productName);

    // Check if the product already exists
    for (int i = 0; i < numProducts; i++) {
        if (strcmp(products[i].name, productName) == 0) {
            printf("Product already exists. Please choose a different name.\n");
            return;
        }
    }

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    printf("Enter price: ");
    scanf("%f", &price);

    // Add the new product
    strcpy(products[numProducts].name, productName);
    products[numProducts].quantity = quantity;
    products[numProducts].price = price;
    numProducts++;

    // Save the newly added product data to the file
    FILE *file = fopen("products.csv", "a");
    if (file == NULL) {
        printf("Error opening products file for appending!\n");
        return;
    }
    fprintf(file, "%s %d %.2f\n", productName, quantity, price);
    fclose(file);

    printf("Product added successfully!\n");
}


void viewOrders() {
    printf("\n======= Orders =======\n");

    if (numOrders == 0) {
        printf("No orders available.\n");
        return;
    }

    for (int i = 0; i < numOrders; i++) {
        printf("Order %d:\n", i + 1);
        printf("Product Name: %s\n", orders[i].productName);
        printf("Quantity: %d\n", orders[i].quantity);
        printf("--------------------------------\n");
    }
}


void searchProducts() {
    char searchName[MAX_NAME_LENGTH];
    float minPrice, maxPrice;
    int found = 0;

    printf("Enter the name of the product you want to search for (leave blank for any): ");
    scanf("%s", searchName);

    printf("Enter the minimum price (leave blank for any): ");
    scanf("%f", &minPrice);

    printf("Enter the maximum price (leave blank for any): ");
    scanf("%f", &maxPrice);

    printf("\n======= Search Results =======\n");

    for (int i = 0; i < numProducts; i++) {
        // Check if the product name contains the search name as a substring
        if (searchName[0] == '\0' || strstr(products[i].name, searchName) != NULL) {
            // Check if the product price is within the specified range (if provided)
            if ((minPrice == 0 || products[i].price >= minPrice) && (maxPrice == 0 || products[i].price <= maxPrice)) {
                printf("Product Name: %s\n", products[i].name);
                printf("Quantity: %d\n", products[i].quantity);
                printf("Price: $%.2f\n", products[i].price);
                printf("--------------------------------\n");
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No products found with the given criteria.\n");
    }
}


int authenticateUser(const char *username, const char *password) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1; // User authenticated
        }
    }
    return 0; // Authentication failed
}


void loadUsers() {
    FILE *file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("Error opening users file!\n");
        exit(1);
    }

    // Read user data from the file and store it in the array of users
    while (fscanf(file, "%s %s", users[numUsers].username, users[numUsers].password) != EOF) {
        numUsers++;
        if (numUsers >= MAX_USERS) {
            printf("Maximum number of users reached.\n");
            break;
        }
    }

    fclose(file);
}

void saveUsers() {
    FILE *file = fopen("users.csv", "w");
    if (file == NULL) {
        printf("Error opening users file for writing!\n");
        exit(1);
    }

    // Write user data to the file
    for (int i = 0; i < numUsers; i++) {
        fprintf(file, "%s %s\n", users[i].username, users[i].password);
    }

    fclose(file);
}


void loadProducts() {
    FILE *file = fopen("products.csv", "r");
    if (file == NULL) {
        printf("Error opening products file!\n");
        return;
    }

    // Read product data from the file and store it in the array of products
    while (numProducts < MAX_PRODUCTS && fscanf(file, "%s %d %f", products[numProducts].name, &products[numProducts].quantity, &products[numProducts].price) == 3) {
        numProducts++;
    }

    if (numProducts >= MAX_PRODUCTS) {
        printf("Maximum number of products reached.\n");
    }

    fclose(file);
}


void saveProducts() {
    FILE *file = fopen("products.csv", "w");
    if (file == NULL) {
        printf("Error opening products file for writing!\n");
        return;
    }

    // Write product data to the file
    for (int i = 0; i < numProducts; i++) {
        // Check if writing to the file was successful
        if (fprintf(file, "%s %d %.2f\n", products[i].name, products[i].quantity, products[i].price) < 0) {
            printf("Error writing product data to the file!\n");
            fclose(file);
            return;
        }
    }

    fclose(file);
}



void saveOrders() {
    FILE *file = fopen("orders.csv", "w");
    if (file == NULL) {
        printf("Error opening orders file for writing!\n");
        return;
    }

    // Write order data to the file
    for (int i = 0; i < numOrders; i++) {
        // Check if writing to the file was successful
        if (fprintf(file, "%s %d\n", orders[i].productName, orders[i].quantity) < 0) {
            printf("Error writing order data to the file!\n");
            fclose(file);
            return;
        }
    }

    fclose(file);
}
