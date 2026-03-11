#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME 50
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define TABLE_SIZE 100

typedef struct Contact {
    char name[MAX_NAME];
    char phone[3][MAX_PHONE];
    char email[MAX_EMAIL];
    char type[10]; // Private / Business
} Contact;

typedef struct Node {
    Contact contact;
    struct Node* next;
} Node;

typedef struct BSTNode {
    Contact contact;
    struct BSTNode *left, *right;
} BSTNode;

Node *head = NULL;
Node *hashTable[TABLE_SIZE] = { NULL };
int contactCount = 0;

// Hash function
int hashFunction(const char *phone) {
    int sum = 0;
    for (int i = 0; phone[i]; i++) sum += phone[i];
    return sum % TABLE_SIZE;
}

// Duplicate check
bool isDuplicate(const char *name, const char *phone) {
    Node *temp = head;
    while (temp) {
        if (strcmp(temp->contact.name, name) == 0 || strcmp(temp->contact.phone[0], phone) == 0)
            return true;
        temp = temp->next;
    }
    return false;
}

// Add contact
void addContact(const char *name, const char *phone, const char *email, const char *type) {
    if (isDuplicate(name, phone)) {
        printf("Duplicate contact! Not added.\n");
        return;
    }

    Node *newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->contact.name, name);
    strcpy(newNode->contact.phone[0], phone);
    strcpy(newNode->contact.email, email);
    strcpy(newNode->contact.type, type);
    newNode->next = head;
    head = newNode;

    int index = hashFunction(phone);
    Node *hashNode = (Node*)malloc(sizeof(Node));
    hashNode->contact = newNode->contact;
    hashNode->next = hashTable[index];
    hashTable[index] = hashNode;

    contactCount++;
    printf("Contact added.\n");
}

// Search contact
void searchContact() {
    char query[MAX_NAME];
    printf("Enter name or part of name to search: ");
    fgets(query, MAX_NAME, stdin);
    query[strcspn(query, "\n")] = 0;

    Node *temp = head;
    int found = 0;
    while (temp) {
        if (strstr(temp->contact.name, query)) {
            printf("%-20s %-15s %-30s %-10s\n", temp->contact.name, temp->contact.phone[0], temp->contact.email, temp->contact.type);
            found++;
        }
        temp = temp->next;
    }
    if (!found) printf("No contact found.\n");
}

// BST insert
BSTNode* insertBST(BSTNode *root, Contact contact) {
    if (!root) {
        BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
        newNode->contact = contact;
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (strcmp(contact.name, root->contact.name) < 0)
        root->left = insertBST(root->left, contact);
    else
        root->right = insertBST(root->right, contact);
    return root;
}

// Build BST from linked list
BSTNode* buildBSTFromList(Node *list) {
    BSTNode *root = NULL;
    Node *temp = list;
    while (temp) {
        root = insertBST(root, temp->contact);
        temp = temp->next;
    }
    return root;
}

// Display sorted contacts (Private / Business separated)
void displaySortedSeparated(BSTNode *root, const char *type) {
    if (!root) return;
    displaySortedSeparated(root->left, type);
    if (strcmp(root->contact.type, type) == 0)
        printf("%-20s %-15s %-30s %-10s\n", root->contact.name, root->contact.phone[0], root->contact.email, root->contact.type);
    displaySortedSeparated(root->right, type);
}

// Delete contact
void deleteContact(const char *phone) {
    Node *temp = head, *prev = NULL;
    while (temp) {
        if (strcmp(temp->contact.phone[0], phone) == 0) {
            if (prev) prev->next = temp->next;
            else head = temp->next;
            free(temp);
            contactCount--;
            printf("Contact deleted.\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Contact not found.\n");
}

// Edit contact
void editContact() {
    int searchChoice;
    printf("Search by: \n1. Phone Number\n2. Name\nEnter your choice: ");
    scanf("%d", &searchChoice);
    getchar();

    Node *temp = head;
    Node *matchedContacts[10];
    int matchCount = 0;

    if (searchChoice == 1) {
        char phoneQuery[MAX_PHONE];
        printf("Enter part of phone number to search: ");
        fgets(phoneQuery, MAX_PHONE, stdin);
        phoneQuery[strcspn(phoneQuery, "\n")] = 0;

        while (temp) {
            if (strstr(temp->contact.phone[0], phoneQuery)) {
                matchedContacts[matchCount] = temp;
                matchCount++;
                printf("%d. %s - %s\n", matchCount, temp->contact.name, temp->contact.phone[0]);
            }
            temp = temp->next;
        }
    } else if (searchChoice == 2) {
        char nameQuery[MAX_NAME];
        printf("Enter part of name to search: ");
        fgets(nameQuery, MAX_NAME, stdin);
        nameQuery[strcspn(nameQuery, "\n")] = 0;

        while (temp) {
            if (strstr(temp->contact.name, nameQuery)) {
                matchedContacts[matchCount] = temp;
                matchCount++;
                printf("%d. %s - %s\n", matchCount, temp->contact.name, temp->contact.phone[0]);
            }
            temp = temp->next;
        }
    } else {
        printf("Invalid choice.\n");
        return;
    }

    if (matchCount == 0) {
        printf("No match found.\n");
        return;
    }

    printf("Select number of contact to edit (1-%d): ", matchCount);
    int choice;
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > matchCount) {
        printf("Invalid selection.\n");
        return;
    }

    Node *selected = matchedContacts[choice - 1];
    printf("Editing %s - %s\n", selected->contact.name, selected->contact.phone[0]);

    char newName[MAX_NAME], newPhone[MAX_PHONE], newEmail[MAX_EMAIL];
    int typeChoice;
    char newType[10];

    printf("Enter new name (or press Enter to keep '%s'): ", selected->contact.name);
    fgets(newName, MAX_NAME, stdin);
    newName[strcspn(newName, "\n")] = 0;

    printf("Enter new phone (or press Enter to keep '%s'): ", selected->contact.phone[0]);
    fgets(newPhone, MAX_PHONE, stdin);
    newPhone[strcspn(newPhone, "\n")] = 0;

    printf("Enter new email (or press Enter to keep '%s'): ", selected->contact.email);
    fgets(newEmail, MAX_EMAIL, stdin);
    newEmail[strcspn(newEmail, "\n")] = 0;

    printf("Contact type? 1. Private  2. Business (current: %s): ", selected->contact.type);
    scanf("%d", &typeChoice);
    getchar();

    if (typeChoice == 1)
        strcpy(newType, "Private");
    else if (typeChoice == 2)
        strcpy(newType, "Business");
    else
        strcpy(newType, selected->contact.type);

    // If blank, keep old
    if (strlen(newName) == 0) strcpy(newName, selected->contact.name);
    if (strlen(newPhone) == 0) strcpy(newPhone, selected->contact.phone[0]);
    if (strlen(newEmail) == 0) strcpy(newEmail, selected->contact.email);

    // Fix: Only check duplicate if name/phone changed
    if ((strcmp(newName, selected->contact.name) != 0 || strcmp(newPhone, selected->contact.phone[0]) != 0)
        && isDuplicate(newName, newPhone)) {
        printf("New info conflicts with existing contact.\n");
        return;
    }

    deleteContact(selected->contact.phone[0]);
    addContact(newName, newPhone, newEmail, newType);
}


int main() {
    int choice;
    char name[MAX_NAME], phone[MAX_PHONE], email[MAX_EMAIL], type[10];

    do {
        printf("\n==== Telephone Directory ====\n");
        printf("1. Add Contact\n2. Search Contact\n3. Edit Contact\n4. Display Contacts\n5. Delete Contact\n6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            printf("Enter name: ");
            fgets(name, MAX_NAME, stdin);
            name[strcspn(name, "\n")] = 0;

            printf("Enter phone: ");
            fgets(phone, MAX_PHONE, stdin);
            phone[strcspn(phone, "\n")] = 0;

            printf("Enter email: ");
            fgets(email, MAX_EMAIL, stdin);
            email[strcspn(email, "\n")] = 0;

            int typeChoice;
            printf("Contact type? 1. Private  2. Business: ");
            scanf("%d", &typeChoice);
            getchar();

            if (typeChoice == 1)
                strcpy(type, "Private");
            else if (typeChoice == 2)
                strcpy(type, "Business");
            else {
                printf("Invalid type. Defaulting to Private.\n");
                strcpy(type, "Private");
            }

            addContact(name, phone, email, type);
            break;

        case 2:
            searchContact();
            break;

        case 3:
            editContact();
            break;

        case 4: {
            BSTNode *root = buildBSTFromList(head);
            printf("\n--- Private Contacts ---\n");
            printf("%-20s %-15s %-30s %-10s\n", "Name", "Phone", "Email", "Type");
            displaySortedSeparated(root, "Private");

            printf("\n--- Business Contacts ---\n");
            printf("%-20s %-15s %-30s %-10s\n", "Name", "Phone", "Email", "Type");
            displaySortedSeparated(root, "Business");
            break;
        }

        case 5:
            printf("Enter phone to delete: ");
            fgets(phone, MAX_PHONE, stdin);
            phone[strcspn(phone, "\n")] = 0;
            deleteContact(phone);
            break;

        case 6:
            printf("Exiting...\n");
            break;

        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 6);

    return 0;
}
