#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

vector<string> fullNames;
vector<string> passwords;
vector<string> emails;
vector<string> mobileNumbers;
vector<string> icPassports;

vector<string> consultationPatients;
vector<string> consultationDetails;
vector<bool> consultationCritical;
vector<string> checkedInPatients;
vector<string> queueNumbers;
vector<bool> isCritical;

struct BillItem {
    string service;
    int quantity;
    double unitPrice;
};

struct PatientBill {
    string patientName;
    vector<BillItem> items;
    string dateTime;
};

vector<PatientBill> patientBills;

int currentQueueNumber = 1;
int currentCriticalQueueNumber = 1;

string formatQueueNumber(int number, bool critical) {
    string prefix = critical ? "C" : "";
    stringstream ss;
    ss << prefix << setw(4) << setfill('0') << number;
    return ss.str();
}

int calculateQueuePosition(const string& fullName, bool& isPatientCritical, int& position) {
    position = -1;
    int totalAhead = 0;
    for (size_t i = 0; i < checkedInPatients.size(); i++) {
        if (checkedInPatients[i] == fullName) {
            position = i;
            isPatientCritical = isCritical[i];
            break;
        }
        if (!isCritical[i]) {
            totalAhead++;
        }
    }
    return position == -1 ? -1 : totalAhead;
}

void viewQueueDetails(const string& queueNumber, int queuePosition, int estimatedWaitingTime) {
    cout << "\n=== Queue Details ===\n";
    cout << "Queue Number: " << queueNumber << "\n";
    cout << "Queue Position: " << queuePosition << "\n";
    cout << "Estimated Waiting Time: " << estimatedWaitingTime << " minutes\n";
}

string getCurrentDateTime() {
    return "19 February, 2025 / 03:00 PM";
}

void savePatientsToFile() {
    ofstream outFile("patients.txt");
    for (size_t i = 0; i < fullNames.size(); i++) {
        outFile << fullNames[i] << "|" << passwords[i] << "|" << emails[i] << "|"
                << mobileNumbers[i] << "|" << icPassports[i] << "\n";
    }
    outFile.close();
}

void loadPatientsFromFile() {
    ifstream inFile("patients.txt");
    if (!inFile) return;
    
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string fullName, password, email, mobileNumber, icPassport;
        getline(ss, fullName, '|');
        getline(ss, password, '|');
        getline(ss, email, '|');
        getline(ss, mobileNumber, '|');
        getline(ss, icPassport, '|');
        
        fullNames.push_back(fullName);
        passwords.push_back(password);
        emails.push_back(email);
        mobileNumbers.push_back(mobileNumber);
        icPassports.push_back(icPassport);
    }
    inFile.close();
}

void saveQueueToFile() {
    ofstream outFile("queue.txt");
    for (size_t i = 0; i < checkedInPatients.size(); i++) {
        outFile << checkedInPatients[i] << "|" << queueNumbers[i] << "|" << isCritical[i] << "\n";
    }
    outFile.close();
}

void loadQueueFromFile() {
    ifstream inFile("queue.txt");
    if (!inFile) return;
    
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string patientName, queueNumber;
        bool critical;
        getline(ss, patientName, '|');
        getline(ss, queueNumber, '|');
        ss >> critical;
        
        checkedInPatients.push_back(patientName);
        queueNumbers.push_back(queueNumber);
        isCritical.push_back(critical);
        
        
        if (queueNumber[0] == 'C') {
            int num = stoi(queueNumber.substr(1));
            currentCriticalQueueNumber = max(currentCriticalQueueNumber, num + 1);
        } else {
            int num = stoi(queueNumber);
            currentQueueNumber = max(currentQueueNumber, num + 1);
        }
    }
    inFile.close();
}

void saveConsultationsToFile() {
    ofstream outFile("consultations.txt");
    for (size_t i = 0; i < consultationPatients.size(); i++) {
        outFile << consultationPatients[i] << "|" << consultationDetails[i] << "|" 
                << consultationCritical[i] << "\n";
    }
    outFile.close();
}

void loadConsultationsFromFile() {
    ifstream inFile("consultations.txt");
    if (!inFile) return;
    
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string patientName, details;
        bool critical;
        getline(ss, patientName, '|');
        getline(ss, details, '|');
        ss >> critical;
        
        consultationPatients.push_back(patientName);
        consultationDetails.push_back(details);
        consultationCritical.push_back(critical);
    }
    inFile.close();
}

void saveBillsToFile() {
    ofstream outFile("bills.txt");
    for (const auto& bill : patientBills) {
        outFile << bill.patientName << "|" << bill.dateTime << "|";
        for (size_t i = 0; i < bill.items.size(); i++) {
            outFile << bill.items[i].service << "," << bill.items[i].quantity << ","
                    << bill.items[i].unitPrice;
            if (i < bill.items.size() - 1) outFile << ";";
        }
        outFile << "\n";
    }
    outFile.close();
}

void loadBillsFromFile() {
    ifstream inFile("bills.txt");
    if (!inFile) return;
    
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        PatientBill bill;
        getline(ss, bill.patientName, '|');
        getline(ss, bill.dateTime, '|');
        
        string itemsStr;
        getline(ss, itemsStr);
        stringstream itemsSS(itemsStr);
        string item;
        while (getline(itemsSS, item, ';')) {
            stringstream itemSS(item);
            BillItem billItem;
            getline(itemSS, billItem.service, ',');
            itemSS >> billItem.quantity;
            itemSS.ignore(1);
            itemSS >> billItem.unitPrice;
            bill.items.push_back(billItem);
        }
        
        patientBills.push_back(bill);
    }
    inFile.close();
}

void viewBills(const string& fullName) {
    bool billFound = false;

    for (const auto& bill : patientBills) {
        if (bill.patientName == fullName) {
            billFound = true;
            double totalAmount = 0.0;
            for (const auto& item : bill.items) {
                totalAmount += item.quantity * item.unitPrice;
            }

            cout << "\n=== Clinic Bill ===\n";
            cout << "RM " << fixed << setprecision(2) << totalAmount << "\n";
            cout << "Date / Hour: " << bill.dateTime << "\n";
            cout << "Mr./Ms.: " << fullName << "\n";
            cout << "Billing Details: Please Pay At Counter\n";
            cout << "\n";
            cout << "--------------------------------------------------\n";
            cout << "| No | Items/Services         | Qty | Unit Price | Amount |\n";
            cout << "--------------------------------------------------\n";

            int itemNo = 1;
            for (const auto& item : bill.items) {
                double amount = item.quantity * item.unitPrice;
                cout << "| " << left << setw(2) << itemNo++ << " | "
                     << setw(22) << item.service << " | "
                     << setw(3) << item.quantity << " | "
                     << right << setw(10) << fixed << setprecision(2) << item.unitPrice << " | "
                     << setw(6) << amount << " |\n";
            }
            cout << "--------------------------------------------------\n";
            cout << "Sub Total: RM " << totalAmount << "\n";
            cout << "Total Amount Due: RM " << totalAmount << "\n";
        }
    }

    if (!billFound) {
        cout << "\nNo bills found for " << fullName << ".\n";
    }

    int returnChoice;
    cout << "\n1. Back to Home\nChoose an option: ";
    if (!(cin >> returnChoice)) {
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        returnChoice = 1;
    }
    cin.ignore();
    if (returnChoice == 1) {
        cout << "Returning to Home.\n";
    } else {
        cout << "Invalid choice. Returning to Home.\n";
    }
}

void showPatientMenu(const string& fullName) {
    int patientChoice;
    do {
        cout << "\nPatient Menu:\n";
        cout << "1. Consultation\n2. Medical Check-up\n3. Back to Home\nChoose an option: ";
        if (!(cin >> patientChoice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();
        if (patientChoice == 1) {
            cout << "\nSelf-Assessment for Consultation\n";
            cout << "Answer all the following questions (Yes = Y, No = N) - CRITICAL\n";
            
            char answer1, answer2, answer3, answer4;
            string healthIssue = "Critical symptoms: ";
            bool hasCriticalSymptoms = false;

            cout << "1. Are You Experiencing Pain Or Tightness In Your Chest? (Y/N): ";
            cin >> answer1;
            cin.ignore();
            answer1 = toupper(answer1);
            while (answer1 != 'Y' && answer1 != 'N') {
                cout << "Invalid input. Please enter Y or N: ";
                cin >> answer1;
                cin.ignore();
                answer1 = toupper(answer1);
            }
            if (answer1 == 'Y') {
                healthIssue += "Chest Pain/Tightness, ";
                hasCriticalSymptoms = true;
            }

            cout << "2. Are You Having Difficulty Breathing Or Shortness Of Breath? (Y/N): ";
            cin >> answer2;
            cin.ignore();
            answer2 = toupper(answer2);
            while (answer2 != 'Y' && answer2 != 'N') {
                cout << "Invalid input. Please enter Y or N: ";
                cin >> answer2;
                cin.ignore();
                answer2 = toupper(answer2);
            }
            if (answer2 == 'Y') {
                healthIssue += "Difficulty Breathing, ";
                hasCriticalSymptoms = true;
            }

            cout << "3. Have You Experienced Sudden Numbness, Weakness, Slurred Speech, Or Confusion? (Y/N): ";
            cin >> answer3;
            cin.ignore();
            answer3 = toupper(answer3);
            while (answer3 != 'Y' && answer3 != 'N') {
                cout << "Invalid input. Please enter Y or N: ";
                cin >> answer3;
                cin.ignore();
                answer3 = toupper(answer3);
            }
            if (answer3 == 'Y') {
                healthIssue += "Numbness/Weakness/Slurred Speech/Confusion, ";
                hasCriticalSymptoms = true;
            }

            cout << "4. Are You Bleeding Heavily And Unable To Stop It? (Y/N): ";
            cin >> answer4;
            cin.ignore();
            answer4 = toupper(answer4);
            while (answer4 != 'Y' && answer4 != 'N') {
                cout << "Invalid input. Please enter Y or N: ";
                cin >> answer4;
                cin.ignore();
                answer4 = toupper(answer4);
            }
            if (answer4 == 'Y') {
                healthIssue += "Heavy Bleeding, ";
                hasCriticalSymptoms = true;
            }

            PatientBill newBill;
            newBill.patientName = fullName;
            newBill.dateTime = getCurrentDateTime();

            if (hasCriticalSymptoms) {
                string queueNumber = formatQueueNumber(currentCriticalQueueNumber++, true);
                int peopleAhead = 0;
                int estimatedMinutes = 0;
                cout << "\nCritical symptoms detected. Immediate attention required.\n";
                cout << "CRITICAL\n";
                cout << "\"PLEASE COME TO CLINIC IMMEDIATELY\"\n";
                cout << "QUEUE NO: " << queueNumber << "\n";
                cout << peopleAhead << " PEOPLE AHEAD\n";
                cout << "Estimated waiting time: " << estimatedMinutes << " minutes\n";
                cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
                cout << "16:04\n";
                cout << "CLINIC STATUS: OPEN\n";

                checkedInPatients.push_back(fullName);
                queueNumbers.push_back(queueNumber);
                isCritical.push_back(true);

                consultationPatients.push_back(fullName);
                consultationDetails.push_back(healthIssue);
                consultationCritical.push_back(true);

                newBill.items.push_back({"Doctor Consultation", 1, 30.00});
                if (answer2 == 'Y') {
                    newBill.items.push_back({"Blood Test", 1, 50.00});
                }
            } else {
                string timeSlot;
                healthIssue = "No critical symptoms reported.";
                cout << "\nNo critical symptoms reported.\n";

                cout << "\nAvailable time slots:\n";
                cout << "1. 08:00-09:00\n2. 09:00-10:00\n3. 10:00-11:00\n4. 11:00-12:00\n";
                cout << "5. 12:00-01:00\n6. 01:00-02:00\n7. 02:00-03:00\n8. 03:00-04:00\n9. 04:00-05:00\n";
                cout << "Choose a time slot (1-9): ";
                int slotChoice;
                while (!(cin >> slotChoice) || slotChoice < 1 || slotChoice > 9) {
                    cout << "Please choose your preferred slot: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                cin.ignore();
                switch (slotChoice) {
                    case 1: timeSlot = "08:00-09:00"; break;
                    case 2: timeSlot = "09:00-10:00"; break;
                    case 3: timeSlot = "10:00-11:00"; break;
                    case 4: timeSlot = "11:00-12:00"; break;
                    case 5: timeSlot = "12:00-01:00"; break;
                    case 6: timeSlot = "01:00-02:00"; break;
                    case 7: timeSlot = "02:00-03:00"; break;
                    case 8: timeSlot = "03:00-04:00"; break;
                    case 9: timeSlot = "04:00-05:00"; break;
                }

                bool isPatientCritical;
                int position;
                string queueNumber = formatQueueNumber(currentQueueNumber++, false);
                int peopleAhead = calculateQueuePosition(fullName, isPatientCritical, position);
                if (peopleAhead == -1) peopleAhead = max(0, (int)checkedInPatients.size());
                int estimatedMinutes = peopleAhead * 15;
                cout << "\nConsultation booked successfully for " << timeSlot << ".\n";
                cout << "QUEUE NO: " << queueNumber << "\n";
                cout << peopleAhead << " PEOPLE AHEAD\n";
                cout << "Estimated waiting time: " << estimatedMinutes << " minutes\n";
                cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
                cout << "16:04\n";
                cout << "CLINIC STATUS: OPEN\n";

                checkedInPatients.push_back(fullName);
                queueNumbers.push_back(queueNumber);
                isCritical.push_back(false);

                consultationPatients.push_back(fullName);
                consultationDetails.push_back(healthIssue);
                consultationCritical.push_back(false);

                newBill.items.push_back({"Doctor Consultation", 1, 30.00});
                newBill.items.push_back({"Fever Medication", 2, 10.00});
            }
            patientBills.push_back(newBill);
            saveQueueToFile();
            saveConsultationsToFile();
            saveBillsToFile();
        } else if (patientChoice == 2) {
            bool alreadyCheckedIn = false;
            for (const auto& patient : checkedInPatients) {
                if (patient == fullName) {
                    alreadyCheckedIn = true;
                    break;
                }
            }
            if (alreadyCheckedIn) {
                cout << "You have already checked in for a consultation or medical check-up.\n";
                continue;
            }

            bool isPatientCritical;
            int position;
            string queueNumber = formatQueueNumber(currentQueueNumber++, false);
            int peopleAhead = calculateQueuePosition(fullName, isPatientCritical, position);
            if (peopleAhead == -1) peopleAhead = max(0, (int)checkedInPatients.size());
            int estimatedMinutes = peopleAhead * 15;
            cout << "\nMedical Check-up registered successfully.\n";
            cout << "QUEUE NO: " << queueNumber << "\n";
            cout << peopleAhead << " PEOPLE AHEAD\n";
            cout << "Estimated waiting time: " << estimatedMinutes << " minutes\n";
            cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
            cout << "16:04\n";
            cout << "CLINIC STATUS: OPEN\n";

            checkedInPatients.push_back(fullName);
            queueNumbers.push_back(queueNumber);
            isCritical.push_back(false);

            consultationPatients.push_back(fullName);
            consultationDetails.push_back("Medical Check-up");
            consultationCritical.push_back(false);

            PatientBill newBill;
            newBill.patientName = fullName;
            newBill.dateTime = getCurrentDateTime();
            newBill.items.push_back({"Blood Test", 1, 50.00});
            patientBills.push_back(newBill);

            saveQueueToFile();
            saveConsultationsToFile();
            saveBillsToFile();
        } else if (patientChoice == 3) {
            cout << "Returning to Home.\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    } while (patientChoice != 3);
}

void showHomePage(const string& fullName) {
    int homeChoice;
    do {
        cout << "\n16:04\n";
        cout << "Clinic Operating Hours: 8:00 AM - 5:00 PM\n";
        cout << "HAI, WELCOME\n";
        cout << "1. Check-In\n";
        cout << "2. View Queue Details\n";
        cout << "3. View My Bills\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
        if (!(cin >> homeChoice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();
        if (homeChoice == 1) {
            showPatientMenu(fullName);
        } else if (homeChoice == 2) {
            bool isPatientCritical;
            int position;
            string queueNumber = "";
            int totalAhead = calculateQueuePosition(fullName, isPatientCritical, position);

            if (position == -1) {
                cout << "\nPlease Check-In First.\n";
            } else {
                queueNumber = queueNumbers[position];
                int estimatedMinutes = isPatientCritical ? 0 : totalAhead * 15;
                viewQueueDetails(queueNumber, position + 1, estimatedMinutes);

                int returnChoice;
                cout << "\n1. Back to Home\nChoose an option: ";
                if (!(cin >> returnChoice)) {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    returnChoice = 1;
                }
                cin.ignore();
                if (returnChoice == 1) {
                    cout << "Returning to Home.\n";
                } else {
                    cout << "Invalid choice. Returning to Home.\n";
                }
            }
        } else if (homeChoice == 3) {
            viewBills(fullName);
        } else if (homeChoice == 4) {
            cout << "Exiting program.\n";
            exit(0);
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    } while (homeChoice != 4);
}

void registerPatient() {
    string fullName, password, email, mobileNumber, icPassport;
    
    cout << "Enter Full Name: ";
    getline(cin, fullName);
    
    cout << "Enter Password: ";
    getline(cin, password);
    
    cout << "Enter Email: ";
    getline(cin, email);
    
    if (email.find('@') == string::npos) {
        cout << "Invalid email format. Registration failed.\n";
        return;
    }
    
    cout << "Enter Mobile Number: ";
    getline(cin, mobileNumber);
    
    bool isDigit = true;
    for (char c : mobileNumber) {
        if (!isdigit(c)) {
            isDigit = false;
            break;
        }
    }
    if (!isDigit || mobileNumber.length() < 10) {
        cout << "Invalid mobile number. It should contain only digits and be at least 10 digits long. Registration failed.\n";
        return;
    }
    
    cout << "Enter IC/Passport Number: ";
    getline(cin, icPassport);
    
    bool icExists = false;
    for (const auto& ic : icPassports) {
        if (ic == icPassport) {
            icExists = true;
            break;
        }
    }
    if (icExists) {
        cout << "IC/Passport Number already exists. Registration failed.\n";
        return;
    }
    
    fullNames.push_back(fullName);
    passwords.push_back(password);
    emails.push_back(email);
    mobileNumbers.push_back(mobileNumber);
    icPassports.push_back(icPassport);
    
    savePatientsToFile();
    
    cout << "Registration successful.\n";
    
    showHomePage(fullName);
}

int main() {
    
    loadPatientsFromFile();
    loadQueueFromFile();
    loadConsultationsFromFile();
    loadBillsFromFile();
    
    int choice;
    do {
        cout << "\n1. Register\n2. Exit\nChoose an option: ";
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();
        if (choice == 1) {
            registerPatient();
        } else if (choice == 2) {
            cout << "Exiting program.\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 2);
    return 0;
}