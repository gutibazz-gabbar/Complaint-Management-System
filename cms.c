#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

// Color codes for terminal output
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"

// File paths
#define ADMIN_FILE "admin.dat"
#define COMPLAINTS_FILE "complaints.dat"
#define REPORT_TXT "complaints_report.txt"
#define REPORT_CSV "complaints_report.csv"

// Constants
#define MAX_STRING 256
#define MAX_COMPLAINTS 1000
#define DEFAULT_ADMIN_USER "GutibazzGabbar"
#define DEFAULT_ADMIN_PASS "01788814222"

// Structures
typedef struct
{
    char username[50];
    char password[50];
} Admin;

typedef struct
{
    int id;
    char name[100];
    char email[100];
    char phone[20];
    char complaint[500];
    char status[20]; // "Pending", "In Progress", "Resolved", "Closed"
    char date[20];
    char assigned_to[50];
    int priority; // 1-High, 2-Medium, 3-Low
} Complaint;

// Global variables
Admin admin;
Complaint complaints[MAX_COMPLAINTS];
int complaint_count = 0;
int next_id = 1;

// Function prototypes
void clearScreen();
void printHeader();
void printLine(char c, int length);
void loadAdminData();
void saveAdminData();
void loadComplaints();
void saveComplaints();
void initializeSystem();
int authenticateAdmin();
void changeAdminPassword();
void mainMenu();
void userMenu();
void adminMenu();
void fileComplaint();
void checkComplaintStatus();
void viewAllComplaints();
void updateComplaintStatus();
void deleteComplaint();
void generateReports();
void deleteReports();
void viewStatistics();
void displayComplaint(Complaint c, int index);
char *getCurrentDate();
void encryptString(char *str);
void decryptString(char *str);
int getValidInteger(const char *prompt, int min, int max);
void getValidString(const char *prompt, char *str, int maxLen);
void pauseScreen();

// Utility functions
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader()
{
    printf(BOLD CYAN);
    printLine('=', 60);
    printf("        COMPLAINT MANAGEMENT SYSTEM\n");
    printLine('=', 60);
    printf(RESET);
}

void printLine(char c, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%c", c);
    }
    printf("\n");
}

char *getCurrentDate()
{
    static char date[20];
    time_t now = time(0);
    struct tm *timeinfo = localtime(&now);
    strftime(date, sizeof(date), "%Y-%m-%d", timeinfo);
    return date;
}

void encryptString(char *str)
{
    // Simple XOR encryption with key
    char key = 'K';
    for (int i = 0; str[i]; i++)
    {
        str[i] ^= key;
    }
}

void decryptString(char *str)
{
    // XOR decryption (same as encryption with XOR)
    encryptString(str);
}

int getValidInteger(const char *prompt, int min, int max)
{
    int value;
    char input[100];

    while (1)
    {
        printf(YELLOW "%s (%d-%d): " RESET, prompt, min, max);
        if (fgets(input, sizeof(input), stdin))
        {
            if (sscanf(input, "%d", &value) == 1 && value >= min && value <= max)
            {
                return value;
            }
        }
        printf(RED "Invalid input. Please enter a number between %d and %d.\n" RESET, min, max);
    }
}

void getValidString(const char *prompt, char *str, int maxLen)
{
    while (1)
    {
        printf(YELLOW "%s: " RESET, prompt);
        if (fgets(str, maxLen, stdin))
        {
            // Remove newline character
            str[strcspn(str, "\n")] = 0;
            if (strlen(str) > 0)
            {
                return;
            }
        }
        printf(RED "Input cannot be empty. Please try again.\n" RESET);
    }
}

void pauseScreen()
{
    printf(CYAN "\nPress any key to continue..." RESET);
    getch();
}

// File operations
void loadAdminData()
{
    FILE *file = fopen(ADMIN_FILE, "rb");
    if (file)
    {
        fread(&admin, sizeof(Admin), 1, file);
        fclose(file);
        // Decrypt password
        decryptString(admin.password);
    }
    else
    {
        // Create default admin account
        strcpy(admin.username, DEFAULT_ADMIN_USER);
        strcpy(admin.password, DEFAULT_ADMIN_PASS);
        saveAdminData();
        printf(GREEN "Default admin account created.\n" RESET);
        printf("Username: %s\n", DEFAULT_ADMIN_USER);
        printf("Password: %s\n", DEFAULT_ADMIN_PASS);
        pauseScreen();
    }
}

void saveAdminData()
{
    FILE *file = fopen(ADMIN_FILE, "wb");
    if (file)
    {
        Admin temp = admin;
        // Encrypt password before saving
        encryptString(temp.password);
        fwrite(&temp, sizeof(Admin), 1, file);
        fclose(file);
    }
    else
    {
        printf(RED "Error saving admin data!\n" RESET);
    }
}

void loadComplaints()
{
    FILE *file = fopen(COMPLAINTS_FILE, "rb");
    if (file)
    {
        fread(&complaint_count, sizeof(int), 1, file);
        fread(&next_id, sizeof(int), 1, file);
        fread(complaints, sizeof(Complaint), complaint_count, file);
        fclose(file);
    }
}

void saveComplaints()
{
    FILE *file = fopen(COMPLAINTS_FILE, "wb");
    if (file)
    {
        fwrite(&complaint_count, sizeof(int), 1, file);
        fwrite(&next_id, sizeof(int), 1, file);
        fwrite(complaints, sizeof(Complaint), complaint_count, file);
        fclose(file);
    }
    else
    {
        printf(RED "Error saving complaints data!\n" RESET);
    }
}

void initializeSystem()
{
    loadAdminData();
    loadComplaints();
    printf(GREEN "System initialized successfully!\n" RESET);
    printf("Loaded %d complaint(s).\n", complaint_count);
}

// Authentication
int authenticateAdmin()
{
    char username[50], password[50];
    int attempts = 0;

    while (attempts < 3)
    {
        clearScreen();
        printHeader();
        printf(BOLD YELLOW "\n        ADMIN LOGIN\n" RESET);
        printLine('-', 30);

        getValidString("Username", username, sizeof(username));
        printf(YELLOW "Password: " RESET);

        // Hide password input
        int i = 0;
        char ch;
        while ((ch = getch()) != '\r' && i < sizeof(password) - 1)
        {
            if (ch == '\b' && i > 0)
            {
                printf("\b \b");
                i--;
            }
            else if (ch != '\b')
            {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0';
        printf("\n");

        if (strcmp(username, admin.username) == 0 && strcmp(password, admin.password) == 0)
        {
            printf(GREEN "\nLogin successful!\n" RESET);
            pauseScreen();
            return 1;
        }

        attempts++;
        printf(RED "\nInvalid credentials! Attempts remaining: %d\n" RESET, 3 - attempts);
        if (attempts < 3)
            pauseScreen();
    }

    printf(RED "Maximum login attempts exceeded!\n" RESET);
    return 0;
}

void changeAdminPassword()
{
    char oldPass[50], newPass[50], confirmPass[50];

    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n    CHANGE ADMIN PASSWORD\n" RESET);
    printLine('-', 30);

    printf(YELLOW "Current Password: " RESET);
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r' && i < sizeof(oldPass) - 1)
    {
        if (ch == '\b' && i > 0)
        {
            printf("\b \b");
            i--;
        }
        else if (ch != '\b')
        {
            oldPass[i++] = ch;
            printf("*");
        }
    }
    oldPass[i] = '\0';
    printf("\n");

    if (strcmp(oldPass, admin.password) != 0)
    {
        printf(RED "Incorrect current password!\n" RESET);
        pauseScreen();
        return;
    }

    getValidString("New Password", newPass, sizeof(newPass));
    getValidString("Confirm Password", confirmPass, sizeof(confirmPass));

    if (strcmp(newPass, confirmPass) != 0)
    {
        printf(RED "Passwords don't match!\n" RESET);
        pauseScreen();
        return;
    }

    strcpy(admin.password, newPass);
    saveAdminData();
    printf(GREEN "Password changed successfully!\n" RESET);
    pauseScreen();
}

// User functions
void fileComplaint()
{
    if (complaint_count >= MAX_COMPLAINTS)
    {
        printf(RED "Maximum complaints limit reached!\n" RESET);
        pauseScreen();
        return;
    }

    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n        FILE NEW COMPLAINT\n" RESET);
    printLine('-', 30);

    Complaint newComplaint;
    newComplaint.id = next_id++;

    getValidString("Your Name", newComplaint.name, sizeof(newComplaint.name));
    getValidString("Email Address", newComplaint.email, sizeof(newComplaint.email));
    getValidString("Phone Number", newComplaint.phone, sizeof(newComplaint.phone));

    printf(YELLOW "Complaint Description (max 500 chars): " RESET);
    fgets(newComplaint.complaint, sizeof(newComplaint.complaint), stdin);
    newComplaint.complaint[strcspn(newComplaint.complaint, "\n")] = 0;

    newComplaint.priority = getValidInteger("Priority", 1, 3);

    strcpy(newComplaint.status, "Pending");
    strcpy(newComplaint.date, getCurrentDate());
    strcpy(newComplaint.assigned_to, "Not Assigned");

    complaints[complaint_count++] = newComplaint;
    saveComplaints();

    printf(GREEN "\nComplaint filed successfully!\n" RESET);
    printf(BOLD "Your Complaint ID: %d\n" RESET, newComplaint.id);
    printf("Please note this ID for future reference.\n");
    pauseScreen();
}

void checkComplaintStatus()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n      CHECK COMPLAINT STATUS\n" RESET);
    printLine('-', 30);

    int id = getValidInteger("Enter Complaint ID", 1, 999999);

    int found = 0;
    for (int i = 0; i < complaint_count; i++)
    {
        if (complaints[i].id == id)
        {
            found = 1;
            printf(GREEN "\nComplaint Found!\n" RESET);
            displayComplaint(complaints[i], i + 1);
            break;
        }
    }

    if (!found)
    {
        printf(RED "Complaint ID not found!\n" RESET);
    }

    pauseScreen();
}

// Admin functions
void viewAllComplaints()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n        ALL COMPLAINTS\n" RESET);
    printLine('-', 30);

    if (complaint_count == 0)
    {
        printf(YELLOW "No complaints found.\n" RESET);
        pauseScreen();
        return;
    }

    for (int i = 0; i < complaint_count; i++)
    {
        displayComplaint(complaints[i], i + 1);
        if ((i + 1) % 3 == 0 && i < complaint_count - 1)
        {
            printf(CYAN "Press any key to see more..." RESET);
            getch();
        }
    }

    pauseScreen();
}

void displayComplaint(Complaint c, int index)
{
    printf(CYAN "\n[%d] " RESET, index);
    printf(BOLD "ID: %d" RESET " | ", c.id);

    // Color code status
    if (strcmp(c.status, "Pending") == 0)
        printf(YELLOW);
    else if (strcmp(c.status, "In Progress") == 0)
        printf(BLUE);
    else if (strcmp(c.status, "Resolved") == 0)
        printf(GREEN);
    else
        printf(MAGENTA);

    printf("Status: %s" RESET "\n", c.status);
    printf("Name: %s | Email: %s | Phone: %s\n", c.name, c.email, c.phone);
    printf("Date: %s | Priority: %s | Assigned: %s\n",
           c.date,
           c.priority == 1 ? "High" : c.priority == 2 ? "Medium"
                                                      : "Low",
           c.assigned_to);
    printf("Complaint: %.100s%s\n", c.complaint, strlen(c.complaint) > 100 ? "..." : "");
    printLine('-', 50);
}

void updateComplaintStatus()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n    UPDATE COMPLAINT STATUS\n" RESET);
    printLine('-', 30);

    int id = getValidInteger("Enter Complaint ID to update", 1, 999999);

    int found = -1;
    for (int i = 0; i < complaint_count; i++)
    {
        if (complaints[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf(RED "Complaint ID not found!\n" RESET);
        pauseScreen();
        return;
    }

    printf("\nCurrent complaint details:\n");
    displayComplaint(complaints[found], found + 1);

    printf(YELLOW "\nSelect new status:\n" RESET);
    printf("1. Pending\n2. In Progress\n3. Resolved\n4. Closed\n");

    int choice = getValidInteger("Enter choice", 1, 4);

    switch (choice)
    {
    case 1:
        strcpy(complaints[found].status, "Pending");
        break;
    case 2:
        strcpy(complaints[found].status, "In Progress");
        break;
    case 3:
        strcpy(complaints[found].status, "Resolved");
        break;
    case 4:
        strcpy(complaints[found].status, "Closed");
        break;
    }

    getValidString("Assign to (person/department)", complaints[found].assigned_to,
                   sizeof(complaints[found].assigned_to));

    saveComplaints();
    printf(GREEN "Complaint status updated successfully!\n" RESET);
    pauseScreen();
}

void deleteComplaint()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n      DELETE COMPLAINT\n" RESET);
    printLine('-', 30);

    int id = getValidInteger("Enter Complaint ID to delete", 1, 999999);

    int found = -1;
    for (int i = 0; i < complaint_count; i++)
    {
        if (complaints[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf(RED "Complaint ID not found!\n" RESET);
        pauseScreen();
        return;
    }

    printf("\nComplaint to be deleted:\n");
    displayComplaint(complaints[found], found + 1);

    printf(YELLOW "Are you sure you want to delete this complaint? (y/n): " RESET);
    char confirm;
    scanf(" %c", &confirm);
    getchar(); // consume newline

    if (confirm == 'y' || confirm == 'Y')
    {
        // Shift all complaints after the deleted one
        for (int i = found; i < complaint_count - 1; i++)
        {
            complaints[i] = complaints[i + 1];
        }
        complaint_count--;
        saveComplaints();
        printf(GREEN "Complaint deleted successfully!\n" RESET);
    }
    else
    {
        printf(YELLOW "Delete operation cancelled.\n" RESET);
    }

    pauseScreen();
}

void generateReports()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n      GENERATE REPORTS\n" RESET);
    printLine('-', 30);

    if (complaint_count == 0)
    {
        printf(YELLOW "No complaints to report.\n" RESET);
        pauseScreen();
        return;
    }

    // Generate text report
    FILE *txtFile = fopen(REPORT_TXT, "w");
    FILE *csvFile = fopen(REPORT_CSV, "w");

    if (!txtFile || !csvFile)
    {
        printf(RED "Error creating report files!\n" RESET);
        pauseScreen();
        return;
    }

    // Text report
    fprintf(txtFile, "COMPLAINT MANAGEMENT SYSTEM REPORT\n");
    fprintf(txtFile, "Generated on: %s\n", getCurrentDate());
    fprintf(txtFile, "Total Complaints: %d\n\n", complaint_count);

    // CSV header
    fprintf(csvFile, "ID,Name,Email,Phone,Status,Date,Priority,Assigned To,Complaint\n");

    int pending = 0, inProgress = 0, resolved = 0, closed = 0;
    int high = 0, medium = 0, low = 0;

    for (int i = 0; i < complaint_count; i++)
    {
        Complaint c = complaints[i];

        // Count statistics
        if (strcmp(c.status, "Pending") == 0)
            pending++;
        else if (strcmp(c.status, "In Progress") == 0)
            inProgress++;
        else if (strcmp(c.status, "Resolved") == 0)
            resolved++;
        else if (strcmp(c.status, "Closed") == 0)
            closed++;

        if (c.priority == 1)
            high++;
        else if (c.priority == 2)
            medium++;
        else
            low++;

        // Text report entry
        fprintf(txtFile, "[%d] ID: %d | Status: %s\n", i + 1, c.id, c.status);
        fprintf(txtFile, "Name: %s | Email: %s | Phone: %s\n", c.name, c.email, c.phone);
        fprintf(txtFile, "Date: %s | Priority: %s | Assigned: %s\n",
                c.date,
                c.priority == 1 ? "High" : c.priority == 2 ? "Medium"
                                                           : "Low",
                c.assigned_to);
        fprintf(txtFile, "Complaint: %s\n", c.complaint);
        fprintf(txtFile, "----------------------------------------\n\n");

        // CSV entry
        fprintf(csvFile, "%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                c.id, c.name, c.email, c.phone, c.status, c.date,
                c.priority == 1 ? "High" : c.priority == 2 ? "Medium"
                                                           : "Low",
                c.assigned_to, c.complaint);
    }

    // Statistics in text report
    fprintf(txtFile, "\nSTATISTICS:\n");
    fprintf(txtFile, "By Status:\n");
    fprintf(txtFile, "  Pending: %d\n", pending);
    fprintf(txtFile, "  In Progress: %d\n", inProgress);
    fprintf(txtFile, "  Resolved: %d\n", resolved);
    fprintf(txtFile, "  Closed: %d\n", closed);
    fprintf(txtFile, "\nBy Priority:\n");
    fprintf(txtFile, "  High: %d\n", high);
    fprintf(txtFile, "  Medium: %d\n", medium);
    fprintf(txtFile, "  Low: %d\n", low);

    fclose(txtFile);
    fclose(csvFile);

    printf(GREEN "Reports generated successfully!\n" RESET);
    printf("Files created:\n");
    printf("- %s (Text format)\n", REPORT_TXT);
    printf("- %s (CSV format)\n", REPORT_CSV);
    printf("\nStatistics:\n");
    printf("Total: %d | Pending: %d | In Progress: %d | Resolved: %d | Closed: %d\n",
           complaint_count, pending, inProgress, resolved, closed);
    printf("High Priority: %d | Medium: %d | Low: %d\n", high, medium, low);

    pauseScreen();
}

void deleteReports()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n       DELETE REPORTS\n" RESET);
    printLine('-', 30);

    printf(YELLOW "Are you sure you want to delete all report files? (y/n): " RESET);
    char confirm;
    scanf(" %c", &confirm);
    getchar();

    if (confirm == 'y' || confirm == 'Y')
    {
        int deleted = 0;

        if (remove(REPORT_TXT) == 0)
        {
            printf(GREEN "Deleted: %s\n" RESET, REPORT_TXT);
            deleted++;
        }

        if (remove(REPORT_CSV) == 0)
        {
            printf(GREEN "Deleted: %s\n" RESET, REPORT_CSV);
            deleted++;
        }

        if (deleted > 0)
        {
            printf(GREEN "Report files deleted successfully!\n" RESET);
        }
        else
        {
            printf(YELLOW "No report files found to delete.\n" RESET);
        }
    }
    else
    {
        printf(YELLOW "Delete operation cancelled.\n" RESET);
    }

    pauseScreen();
}

void viewStatistics()
{
    clearScreen();
    printHeader();
    printf(BOLD YELLOW "\n        STATISTICS\n" RESET);
    printLine('-', 30);

    if (complaint_count == 0)
    {
        printf(YELLOW "No data available for statistics.\n" RESET);
        pauseScreen();
        return;
    }

    int pending = 0, inProgress = 0, resolved = 0, closed = 0;
    int high = 0, medium = 0, low = 0;
    int thisMonth = 0;

    char currentMonth[8];
    strftime(currentMonth, sizeof(currentMonth), "%Y-%m", localtime(&(time_t){time(NULL)}));

    for (int i = 0; i < complaint_count; i++)
    {
        Complaint c = complaints[i];

        // Status count
        if (strcmp(c.status, "Pending") == 0)
            pending++;
        else if (strcmp(c.status, "In Progress") == 0)
            inProgress++;
        else if (strcmp(c.status, "Resolved") == 0)
            resolved++;
        else if (strcmp(c.status, "Closed") == 0)
            closed++;

        // Priority count
        if (c.priority == 1)
            high++;
        else if (c.priority == 2)
            medium++;
        else
            low++;

        // This month count
        if (strncmp(c.date, currentMonth, 7) == 0)
            thisMonth++;
    }

    printf(BOLD "COMPLAINT STATISTICS\n" RESET);
    printf("Total Complaints: %d\n", complaint_count);
    printf("Complaints this month: %d\n\n", thisMonth);

    printf(CYAN "Status Distribution:\n" RESET);
    printf("  - Pending:     %d (%.1f%%)\n", pending, (float)pending / complaint_count * 100);
    printf("  - In Progress: %d (%.1f%%)\n", inProgress, (float)inProgress / complaint_count * 100);
    printf("  - Resolved:    %d (%.1f%%)\n", resolved, (float)resolved / complaint_count * 100);
    printf("  - Closed:      %d (%.1f%%)\n\n", closed, (float)closed / complaint_count * 100);

    printf(MAGENTA "Priority Distribution:\n" RESET);
    printf("  - High:   %d (%.1f%%)\n", high, (float)high / complaint_count * 100);
    printf("  - Medium: %d (%.1f%%)\n", medium, (float)medium / complaint_count * 100);
    printf("  - Low:    %d (%.1f%%)\n\n", low, (float)low / complaint_count * 100);

    // Resolution rate
    float resolutionRate = (float)(resolved + closed) / complaint_count * 100;
    printf(GREEN "Resolution Rate: %.1f%%\n" RESET, resolutionRate);

    pauseScreen();
}

// Menu functions
void mainMenu()
{
    int choice;

    while (1)
    {
        clearScreen();
        printHeader();
        printf(BOLD CYAN "\n          MAIN MENU\n" RESET);
        printLine('-', 25);

        printf("1. " BLUE "User Section" RESET "\n");
        printf("2. " YELLOW "Admin Section" RESET "\n");
        printf("3. " RED "Exit" RESET "\n");

        choice = getValidInteger("Select option", 1, 3);

        switch (choice)
        {
        case 1:
            userMenu();
            break;
        case 2:
            if (authenticateAdmin())
            {
                adminMenu();
            }
            break;
        case 3:
            printf(GREEN "Thank you for using Complaint Management System!\n" RESET);
            exit(0);
        }
    }
}

void userMenu()
{
    int choice;

    while (1)
    {
        clearScreen();
        printHeader();
        printf(BOLD BLUE "\n         USER SECTION\n" RESET);
        printLine('-', 25);

        printf("1. File New Complaint\n");
        printf("2. Check Complaint Status\n");
        printf("3. Back to Main Menu\n");

        choice = getValidInteger("Select option", 1, 3);

        switch (choice)
        {
        case 1:
            fileComplaint();
            break;
        case 2:
            checkComplaintStatus();
            break;
        case 3:
            return;
        }
    }
}

void adminMenu()
{
    int choice;

    while (1)
    {
        clearScreen();
        printHeader();
        printf(BOLD YELLOW "\n        ADMIN SECTION\n" RESET);
        printLine('-', 25);

        printf("1. View All Complaints\n");
        printf("2. Update Complaint Status\n");
        printf("3. Delete Complaint\n");
        printf("4. Generate Reports\n");
        printf("5. Delete Reports\n");
        printf("6. View Statistics\n");
        printf("7. Change Admin Password\n");
        printf("8. Back to Main Menu\n");

        choice = getValidInteger("Select option", 1, 8);

        switch (choice)
        {
        case 1:
            viewAllComplaints();
            break;
        case 2:
            updateComplaintStatus();
            break;
        case 3:
            deleteComplaint();
            break;
        case 4:
            generateReports();
            break;
        case 5:
            deleteReports();
            break;
        case 6:
            viewStatistics();
            break;
        case 7:
            changeAdminPassword();
            break;
        case 8:
            return;
        }
    }
}

// Main function
int main()
{
    printf(BOLD GREEN "Initializing Complaint Management System...\n" RESET);
    initializeSystem();
    pauseScreen();

    mainMenu();

    return 0;
}