#include <iostream>
#include <string>
#include <vector>

using namespace std;

const string STAFF_USERNAME = "admin";
const string STAFF_PASSWORD = "admin123";

struct Patient {
    string queueNumber;
    string name;
    int estimatedWaitTime;
    string status;
    string action;
};

bool staffLogin() {
    string username, password;
    cout << "\nEnter staff username: ";
    getline(cin, username);
    cout << "Enter staff password: ";
    getline(cin, password);
    
    if (username == STAFF_USERNAME && password == STAFF_PASSWORD) {
        cout << "\nLogin Successfully.\n";
        return true;
    }
    cout << "\nInvalid Username or Password, Please re-enter the correct credentials to login.\n";
    return false;
}

int displayMenu() {
    int choice;
    cout << "\n=== Staff Portal ===\n";
    cout << "1. Login\n";
    cout << "2. Exit\n";
    cout << "Choose an option: ";
    cin >> choice;
    cin.ignore();
    return choice;
}

int displayStaffMainPage() {
    int choice;
    cout << "\n=== Staff Main Page ===\n";
    cout << "1. Monitor Real-Time Queue Data\n";
    cout << "2. Generate Patient Bill\n";
    cout << "3. Queue Analytics & Reporting\n";
    cout << "4. Logout\n";
    cout << "Choose an option: ";
    cin >> choice;
    cin.ignore();
    return choice;
}

int displayQueueDashboard() {
    int choice;
    cout << "\n=== Queue Dashboard ===\n";
    cout << "Available Queues:\n";
    cout << "1. Doctor Consultation\n";
    cout << "2. Medical Checkup\n";
    cout << "3. Back\n";
    cout << "Select a queue: ";
    cin >> choice;
    cin.ignore();
    return choice;
}

void showDoctorConsultation() {
    vector<Patient> consultationQueue = {
        {"0001", "Patient A", 10, "Waiting", "Call Patient"},
        {"0002", "Patient B", 20, "In Progress", "Monitor"},
        {"0003", "Patient C", 30, "Waiting", "Call Patient"}
    };

    cout << "\n=== Real-Time Doctor Consultation Queue ===\n";
    cout << "Queue No. | Patient Name | Est. Wait Time | Status | Action\n";
    cout << "-----------------------------------------------------\n";
    for (const auto& patient : consultationQueue) {
        cout << patient.queueNumber << " | " 
             << patient.name << " | " 
             << patient.estimatedWaitTime << " mins | " 
             << patient.status << " | " 
             << patient.action << "\n";
    }
}

void showMedicalCheckup() {
    vector<Patient> checkupQueue = {
        {"0101", "Patient X", 15, "Waiting", "Prepare Equipment"},
        {"0102", "Patient Y", 25, "Waiting", "Prepare Equipment"},
        {"0103", "Patient Z", 5, "In Progress", "Complete Test"}
    };

    cout << "\n=== Real-Time Medical Checkup Queue ===\n";
    cout << "Queue No. | Patient Name | Est. Wait Time | Status | Action\n";
    cout << "-----------------------------------------------------\n";
    for (const auto& patient : checkupQueue) {
        cout << patient.queueNumber << " | " 
             << patient.name << " | " 
             << patient.estimatedWaitTime << " mins | " 
             << patient.status << " | " 
             << patient.action << "\n";
    }
}

void monitorQueueData() {
    while (true) {
        int dashboardChoice = displayQueueDashboard();
        if (dashboardChoice == 1) {
            showDoctorConsultation();
        } else if (dashboardChoice == 2) {
            showMedicalCheckup();
        } else if (dashboardChoice == 3) {
            cout << "Returning to staff main page...\n";
            break;
        } else {
            cout << "Invalid choice. Please select 1, 2, or 3.\n";
        }
    }
}

void queueAnalyticsReporting() {
    cout << "\nQueue Analytics & Reporting...\n";
    cout << "Report Summary:\n";
    cout << "- Total Patients in Queue: 6\n";
    cout << "- Average Wait Time: 15 minutes\n";
    cout << "- Doctor Consultation Queue: 3 patients\n";
    cout << "- Medical Checkup Queue: 3 patients\n";
}

void generatePatientBill() {
    cout << "\nGenerating Patient Bill...\n";
    cout << "Patient Name: John Doe\n";
    cout << "Total Amount: $150.00\n";
}

int main() {
    int choice;
    do {
        choice = displayMenu();
        
        if (choice == 1) {
            if (staffLogin()) {
                cout << "Welcome to the staff main page.\n";
                while (true) {
                    int staffChoice = displayStaffMainPage();
                    if (staffChoice == 1) {
                        monitorQueueData();
                    } else if (staffChoice == 2) {
                        generatePatientBill();
                    } else if (staffChoice == 3) {
                        queueAnalyticsReporting();
                    } else if (staffChoice == 4) {
                        cout << "Logging out...\n";
                        break;
                    } else {
                        cout << "Invalid choice. Please select 1, 2, 3, or 4.\n";
                    }
                }
            } else {
                cout << "Login failed. Returning to menu.\n";
            }
        } else if (choice == 2) {
            cout << "Exiting program.\n";
            return 0;
        } else {
            cout << "Invalid choice. Please select 1 or 2.\n";
        }
    } while (choice != 2);
    return 0;
}