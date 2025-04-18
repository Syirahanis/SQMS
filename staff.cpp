#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

const string STAFF_USERNAME = "admin";
const string STAFF_PASSWORD = "admin123";

struct Patient {
    string queueNumber;
    int estimatedWaitTime;
    string status;
    string action;
};

struct BillItem {
    string serviceName;
    int quantity;
    double unitPrice;
    double amount;
};

struct OperatingHours {
    string date;
    string startTime;
    string endTime;
};

bool staffLogin() {
    string username, password;
    cout << "\nEnter Staff ID: ";
    getline(cin, username);
    cout << "Enter Password: ";
    getline(cin, password);
    
    if (username == STAFF_USERNAME && password == STAFF_PASSWORD) {
        cout << "\nLogin Successfully.\n";
        return true;
    }
    cout << "\nInvalid Staff ID or Password.\n";
    return false;
}

int displayMenu() {
    int choice;
    cout << "\n=== Staff Portal ===\n";
    cout << "1. Login\n";
    cout << "2. Exit\n";
    cout << "Choose an option: ";
    while (!(cin >> choice) || choice < 1 || choice > 2) {
        cout << "Invalid input. Please enter 1 or 2: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    return choice;
}

int displayStaffMainPage() {
    int choice;
    cout << "\n=== Staff Main Page ===\n";
    cout << "1. Monitor Real-Time Queue Data\n";
    cout << "2. Generate Patient Bill\n";
    cout << "3. Update Operating Hours\n";
    cout << "4. Queue Performance Report\n";
    cout << "5. Logout\n";
    cout << "Choose an option: ";
    while (!(cin >> choice) || choice < 1 || choice > 5) {
        cout << "Invalid input. Please enter 1-5: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    return choice;
}

int displayQueueDashboard() {
    int choice;
    cout << "\n=== Queue Dashboard ===\n";
    cout << "Available Queues:\n";
    cout << "1. Consultation\n";
    cout << "2. Medical Checkup\n";
    cout << "Select a queue (1-2, or 0 to go Back): ";
    while (!(cin >> choice) || choice < 0 || choice > 2) {
        cout << "Invalid input. Please enter 0-2: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    return choice;
}

void advanceQueue(vector<Patient>& queue) {
    if (!queue.empty()) {
        queue.erase(queue.begin());
        if (!queue.empty()) {
            queue[0].status = "Calling";
            queue[0].action = "Start";
        }
    }
}

void showQueue(vector<Patient>& queue, const string& queueType) {
    cout << "\n=== Real-Time " << queueType << " Queue ===\n";
    cout << left << setw(12) << "Queue No"
         << setw(24) << "Estimated Waiting Time"
         << setw(12) << "Status"
         << setw(12) << "Action" << "\n";
    cout << string(60, '-') << "\n";

    for (const auto& patient : queue) {
        string waitTime = (patient.estimatedWaitTime == 0) ? "-" : to_string(patient.estimatedWaitTime) + " minutes";
        cout << left << setw(12) << patient.queueNumber
             << setw(24) << waitTime
             << setw(12) << patient.status
             << setw(12) << patient.action << "\n";
    }
}

bool setStatus(vector<Patient>& queue, const string& queueNumber) {
    for (auto& patient : queue) {
        if (patient.queueNumber == queueNumber && patient.status == "Calling") {
            patient.status = "In Session";
            patient.estimatedWaitTime = 0;
            patient.action = "Complete";
            return true;
        }
    }
    return false;
}

void manageQueue(vector<Patient>& queue, const string& queueType) {
    if (queue.empty()) {
        cout << "\n=== Real-Time " << queueType << " Queue ===\n";
        cout << "Queue is empty.\n";
        cout << "Returning to staff main page...\n";
        return;
    }
    while (true) {
        showQueue(queue, queueType);
        cout << "\nOptions:\n";
        cout << "1. Start (for Calling patient)\n";
        cout << "2. Complete (for In Session patient)\n";
        cout << "3. Expired (for no-show/late patient)\n";
        cout << "4. Done (return to main page)\n";
        cout << "Choose an option: ";
        
        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cout << "Invalid input. Please enter 1-4: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        if (choice == 1 && !queue.empty() && queue[0].status == "Calling") {
            if (setStatus(queue, queue[0].queueNumber)) {
                cout << "Patient " << queue[0].queueNumber << " status updated to In Session.\n";
            } else {
                cout << "Failed to update status for patient " << queue[0].queueNumber << ".\n";
            }
        }
        else if (choice == 2 && !queue.empty() && queue[0].status == "In Session") {
            advanceQueue(queue);
            cout << "Patient session completed. Queue advanced.\n";
        }
        else if (choice == 3 && !queue.empty()) {
            advanceQueue(queue);
            cout << "Patient marked as expired. Queue advanced.\n";
        }
        else if (choice == 4) {
            cout << "Returning to staff main page...\n";
            break;
        }
        else {
            cout << "Invalid choice or action not applicable. Please try again.\n";
        }
    }
}

void monitorQueueData(vector<Patient>& consultationQueue, vector<Patient>& checkupQueue) {
    while (true) {
        int dashboardChoice = displayQueueDashboard();
        
        if (dashboardChoice == 0) {
            cout << "Returning to staff main page...\n";
            break;
        }
        else if (dashboardChoice == 1 || dashboardChoice == 2) {
            cout << "Press 1 to Confirm selection, 0 to Cancel: ";
            int confirm;
            while (!(cin >> confirm) || (confirm != 0 && confirm != 1)) {
                cout << "Invalid input. Please enter 1 or 0: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore();
            
            if (confirm == 1) {
                if (dashboardChoice == 1) {
                    manageQueue(consultationQueue, "Consultation");
                    break;
                }
                else {
                    manageQueue(checkupQueue, "Medical Checkup");
                    break;
                }
            }
            else {
                cout << "Selection cancelled. Please select a queue.\n";
            }
        }
        else {
            cout << "Invalid choice. Please select 0, 1, or 2.\n";
        }
    }
}

void updateOperatingHours(vector<OperatingHours>& opHours) {
    int choice;
    do {
        cout << "\n=== Operating Hours Management ===\n";
        cout << "1. View Default Operating Hours\n";
        cout << "2. Update Operating Hours for a Specific Date\n";
        cout << "3. View Updated Operating Hours\n";
        cout << "4. Back to Main Menu\n";
        cout << "Choose an option: ";
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cout << "Invalid input. Please enter 1-4: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        if (choice == 1) {
            cout << "\nDefault Operating Hours:\n";
            cout << "Monday to Friday: 9:00 AM - 5:00 PM\n";
            cout << "Saturday: 9:00 AM - 1:00 PM\n";
            cout << "Sunday: Closed\n";
        } else if (choice == 2) {
            string date, startTime, endTime;
            cout << "\n=== Update Operating Hours ===\n";
            cout << "Choose a date (DD/MM/YYYY): ";
            getline(cin, date);
            cout << "New Operating Hours:\n";
            cout << "START (HH:MM, e.g., 09:00): ";
            getline(cin, startTime);
            cout << "ENDS (HH:MM, e.g., 17:00): ";
            getline(cin, endTime);

            if (date.length() != 10 || startTime.length() != 5 || endTime.length() != 5) {
                cout << "Invalid date or time format. Update cancelled.\n";
                continue;
            }

            OperatingHours newHours = {date, startTime, endTime};
            opHours.push_back(newHours);

            cout << "\nOperating hours updated successfully!\n";
            cout << "Date: " << date << "\n";
            cout << "New Hours: " << startTime << " - " << endTime << "\n";
            cout << "Updates confirmed.\n";
        } else if (choice == 3) {
            if (opHours.empty()) {
                cout << "\nNo custom operating hours have been set.\n";
            } else {
                cout << "\n=== Custom Operating Hours ===\n";
                for (const auto& hours : opHours) {
                    cout << "Date: " << hours.date << "\n";
                    cout << "Hours: " << hours.startTime << " - " << hours.endTime << "\n\n";
                }
            }
        } else if (choice == 4) {
            cout << "Returning to main menu...\n";
        } else {
            cout << "Invalid choice. Please select 1, 2, 3, or 4.\n";
        }
    } while (choice != 4);
}

void performanceReport() {
    cout << "\nPerformance Report...\n";
    cout << "Report Summary:\n";
    cout << "- Total Patients Served: 50\n";
    cout << "- Average Consultation Time: 15 minutes\n";
    cout << "- Patient Satisfaction Rate: 92%\n";
}

void generatePatientBill() {
    string patientName, icNo;
    vector<BillItem> billItems;
    double subTotal = 0.0;

    cout << "\n=== CLINIC BILL GENERATION ===\n";
    cout << "Staff, please enter patient details:\n";
    cout << "Patient Name: ";
    getline(cin, patientName);
    cout << "IC No: ";
    getline(cin, icNo);

    int serviceChoice;
    do {
        cout << "\nSelect Clinic Services:\n";
        cout << "1. Doctor Consultation\n";
        cout << "2. Blood Test\n";
        cout << "3. Fever Medication\n";
        cout << "4. Finish and Generate Bill\n";
        cout << "Choose a service (1-4): ";
        while (!(cin >> serviceChoice) || serviceChoice < 1 || serviceChoice > 4) {
            cout << "Invalid input. Please enter 1-4: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        if (serviceChoice == 4) {
            if (billItems.empty()) {
                cout << "No services selected. Bill generation cancelled.\n";
                return;
            }
        } else if (serviceChoice >= 1 && serviceChoice <= 3) {
            BillItem item;
            switch (serviceChoice) {
                case 1:
                    item.serviceName = "Doctor Consultation";
                    item.unitPrice = 30.00;
                    break;
                case 2:
                    item.serviceName = "Blood Test";
                    item.unitPrice = 50.00;
                    break;
                case 3:
                    item.serviceName = "Fever Medication";
                    item.unitPrice = 10.00;
                    break;
            }
            item.quantity = 1;
            item.amount = item.unitPrice * item.quantity;
            billItems.push_back(item);
            subTotal += item.amount;
        } else {
            cout << "Invalid choice. Please select 1, 2, 3, or 4.\n";
            continue;
        }
    } while (serviceChoice != 4);

    cout << "\n=== CLINIC BILL ===\n";
    cout << left << setw(20) << "Patient Name:" << patientName << "\n";
    cout << left << setw(20) << "IC No:" << icNo << "\n\n";

    cout << left << setw(5) << "No"
         << setw(25) << "Item/Services"
         << setw(10) << "Qty"
         << setw(15) << "Unit Price (RM)"
         << setw(15) << "Amount (RM)" << "\n";
    cout << string(70, '-') << "\n";

    for (size_t i = 0; i < billItems.size(); ++i) {
        cout << left << setw(5) << (i + 1)
             << setw(25) << billItems[i].serviceName
             << setw(10) << billItems[i].quantity
             << fixed << setprecision(2)
             << setw(15) << billItems[i].unitPrice
             << setw(15) << billItems[i].amount << "\n";
    }

    cout << "\n" << left << setw(55) << "Sub Total"
         << fixed << setprecision(2) << subTotal << "\n";
}

int main() {
    vector<Patient> consultationQueue = {
        {"0020", 5, "Calling", "Start"},
        {"0021", 12, "Waiting", "-"},
        {"0022", 20, "Waiting", "-"}
    };
    vector<Patient> checkupQueue = {
        {"0020", 0, "In Session", "Complete"},
        {"0021", 12, "Waiting", "-"},
        {"0022", 20, "Waiting", "-"}
    };
    vector<OperatingHours> customHours;

    int choice;
    do {
        choice = displayMenu();
        
        if (choice == 1) {
            if (staffLogin()) {
                cout << "Welcome to the staff main page.\n";
                int staffChoice;
                do {
                    staffChoice = displayStaffMainPage();
                    if (staffChoice == 1) {
                        monitorQueueData(consultationQueue, checkupQueue);
                    } else if (staffChoice == 2) {
                        generatePatientBill();
                    } else if (staffChoice == 3) {
                        updateOperatingHours(customHours);
                    } else if (staffChoice == 4) {
                        performanceReport();
                    } else if (staffChoice == 5) {
                        cout << "Logging out...\n";
                    } else {
                        cout << "Invalid choice. Please select 1, 2, 3, 4, or 5.\n";
                    }
                } while (staffChoice != 5);
            } else {
                cout << "Login failed. Returning to menu.\n";
            }
        } else if (choice == 2) {
            cout << "Exiting program.\n";
        } else {
            cout << "Invalid choice. Please select 1 or 2.\n";
        }
    } while (choice != 2);
    return 0;
}