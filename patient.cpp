#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

class Patient {
private:
    string fullName;
    string password;
    string email;
    string mobileNumber;
    string icPassport;

public:
    Patient(string name, string pwd, string eml, string mobile, string ic)
        : fullName(name), password(pwd), email(eml), mobileNumber(mobile), icPassport(ic) {}

    string getFullName() const { return fullName; }
    string getIcPassport() const { return icPassport; }

    static void saveToFile(const vector<Patient>& patients) {
        ofstream outFile("patients.txt");
        if (!outFile) {
            cout << "Error opening patients.txt for writing.\n";
            return;
        }
        for (const auto& p : patients) {
            outFile << p.fullName << "|" << p.password << "|" << p.email << "|"
                    << p.mobileNumber << "|" << p.icPassport << "\n";
        }
        outFile.close();
    }

    static vector<Patient> loadFromFile() {
        vector<Patient> patients;
        ifstream inFile("patients.txt");
        if (!inFile) return patients;

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string name, pwd, eml, mobile, ic;
            getline(ss, name, '|');
            getline(ss, pwd, '|');
            getline(ss, eml, '|');
            getline(ss, mobile, '|');
            getline(ss, ic, '|');
            patients.emplace_back(name, pwd, eml, mobile, ic);
        }
        inFile.close();
        return patients;
    }
};

class BillItem {
public:
    string service;
    int quantity;
    double unitPrice;

    BillItem(string svc, int qty, double price) : service(svc), quantity(qty), unitPrice(price) {}
};

class PatientBill {
public:
    string patientName;
    vector<BillItem> items;
    string dateTime;

    PatientBill(string name, string dt) : patientName(name), dateTime(dt) {}

    double getTotal() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item.quantity * item.unitPrice;
        }
        return total;
    }

    static void saveToFile(const vector<PatientBill>& bills) {
        ofstream outFile("bills.txt");
        if (!outFile) {
            cout << "Error opening bills.txt for writing.\n";
            return;
        }
        for (const auto& bill : bills) {
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

    static vector<PatientBill> loadFromFile() {
        vector<PatientBill> bills;
        ifstream inFile("bills.txt");
        if (!inFile) return bills;

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string name, dt, itemsStr;
            getline(ss, name, '|');
            getline(ss, dt, '|');
            getline(ss, itemsStr);
            
            PatientBill bill(name, dt);
            stringstream itemsSS(itemsStr);
            string item;
            while (getline(itemsSS, item, ';')) {
                stringstream itemSS(item);
                string svc;
                int qty;
                double price;
                getline(itemSS, svc, ',');
                itemSS >> qty;
                itemSS.ignore(1);
                itemSS >> price;
                bill.items.emplace_back(svc, qty, price);
            }
            bills.push_back(bill);
        }
        inFile.close();
        return bills;
    }
};

class Consultation {
public:
    string patientName;
    string details;
    bool isCritical;

    Consultation(string name, string det, bool crit) : patientName(name), details(det), isCritical(crit) {}

    static void saveToFile(const vector<Consultation>& consultations) {
        ofstream outFile("consultations.txt");
        if (!outFile) {
            cout << "Error opening consultations.txt for writing.\n";
            return;
        }
        for (const auto& c : consultations) {
            outFile << c.patientName << "|" << c.details << "|" << c.isCritical << "\n";
        }
        outFile.close();
    }

    static vector<Consultation> loadFromFile() {
        vector<Consultation> consultations;
        ifstream inFile("consultations.txt");
        if (!inFile) return consultations;

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string name, details;
            bool crit;
            getline(ss, name, '|');
            getline(ss, details, '|');
            ss >> crit;
            consultations.emplace_back(name, details, crit);
        }
        inFile.close();
        return consultations;
    }
};

class PatientQueueManager {
private:
    vector<string> checkedInPatients;
    vector<string> queueNumbers;
    vector<bool> isCritical;
    vector<string> checkInTimes; 
    vector<bool> isExpired;      
    int currentQueueNumber = 1;
    int currentCriticalQueueNumber = 1;

public:
    PatientQueueManager() {
        loadFromFile();
    }

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
            if (!isCritical[i] && !isExpired[i]) {
                totalAhead++;
            }
        }
        return position == -1 ? -1 : totalAhead;
    }

    void checkInPatient(const string& fullName, bool critical) {
        string queueNumber = formatQueueNumber(critical ? currentCriticalQueueNumber++ : currentQueueNumber++, critical);
        checkedInPatients.push_back(fullName);
        queueNumbers.push_back(queueNumber);
        isCritical.push_back(critical);

        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        stringstream ss;
        ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
        checkInTimes.push_back(ss.str());

        isExpired.push_back(false);
        saveToFile();
    }

    bool isQueueExpired(const string& fullName) {
        for (size_t i = 0; i < checkedInPatients.size(); i++) {
            if (checkedInPatients[i] == fullName) {
                int checkInHour, checkInMinute;
                sscanf(checkInTimes[i].c_str(), "%d:%d", &checkInHour, &checkInMinute);
                int checkInTotalMinutes = checkInHour * 60 + checkInMinute;

                auto now = chrono::system_clock::now();
                time_t currentTime = chrono::system_clock::to_time_t(now);
                tm* localTime = localtime(&currentTime);
                int currentHour = localTime->tm_hour;
                int currentMinute = localTime->tm_min;
                int currentTotalMinutes = currentHour * 60 + currentMinute;
 
                //can change the 15 to 1 for testing
                if (currentTotalMinutes - checkInTotalMinutes > 15) {
                    isExpired[i] = true;
                    saveToFile();
                }
                return isExpired[i];
            }
        }
        return false;
    }

    void requestNewQueueNumber(const string& fullName) {
        for (size_t i = 0; i < checkedInPatients.size(); i++) {
            if (checkedInPatients[i] == fullName) {
                checkedInPatients.erase(checkedInPatients.begin() + i);
                queueNumbers.erase(queueNumbers.begin() + i);
                isCritical.erase(isCritical.begin() + i);
                checkInTimes.erase(checkInTimes.begin() + i);
                isExpired.erase(isExpired.begin() + i);
                break;
            }
        }
        
        bool critical = false; 
        string queueNumber = formatQueueNumber(currentQueueNumber++, critical);
        checkedInPatients.push_back(fullName);
        queueNumbers.push_back(queueNumber);
        isCritical.push_back(critical);

        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        stringstream ss;
        ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
        checkInTimes.push_back(ss.str());

        isExpired.push_back(false);
        saveToFile();

        bool isPatientCritical;
        int position;
        int totalAhead = calculateQueuePosition(fullName, isPatientCritical, position);
        int estimatedMinutes = totalAhead * 15;

        cout << "\nYOU HAVE SUCCESSFULLY CHECKED-IN\n";
        viewQueueDetails(queueNumber, position + 1, estimatedMinutes);
        cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
    }

    void cancelAppointment(const string& fullName) {
        for (size_t i = 0; i < checkedInPatients.size(); i++) {
            if (checkedInPatients[i] == fullName) {
                checkedInPatients.erase(checkedInPatients.begin() + i);
                queueNumbers.erase(queueNumbers.begin() + i);
                isCritical.erase(isCritical.begin() + i);
                checkInTimes.erase(checkInTimes.begin() + i);
                isExpired.erase(isExpired.begin() + i);
                break;
            }
        }
        saveToFile();
        cout << "\nYour appoinment has been canceled successfully.\n";
    }

    void saveToFile() {
        ofstream outFile("queue.txt");
        if (!outFile) {
            cout << "Error opening queue.txt for writing.\n";
            return;
        }
        for (size_t i = 0; i < checkedInPatients.size(); i++) {
            outFile << checkedInPatients[i] << "|" << queueNumbers[i] << "|" << isCritical[i] << "|"
                    << checkInTimes[i] << "|" << isExpired[i] << "\n";
        }
        outFile.close();
    }

    void loadFromFile() {
        ifstream inFile("queue.txt");
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string patientName, queueNumber, checkInTime;
            bool critical, expired;
            getline(ss, patientName, '|');
            getline(ss, queueNumber, '|');
            ss >> critical;
            ss.ignore(1);
            getline(ss, checkInTime, '|');
            ss >> expired;

            checkedInPatients.push_back(patientName);
            queueNumbers.push_back(queueNumber);
            isCritical.push_back(critical);
            checkInTimes.push_back(checkInTime);
            isExpired.push_back(expired);

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

    bool isCheckedIn(const string& fullName) {
        return find(checkedInPatients.begin(), checkedInPatients.end(), fullName) != checkedInPatients.end();
    }

    string getQueueNumber(const string& fullName) {
        for (size_t i = 0; i < checkedInPatients.size(); i++) {
            if (checkedInPatients[i] == fullName) return queueNumbers[i];
        }
        return "";
    }

private:
    void viewQueueDetails(const string& queueNumber, int queuePosition, int estimatedWaitingTime) {
        cout << "=== Queue Details ===\n";
        cout << "Queue Number: " << queueNumber << "\n";
        cout << "Queue Position: " << queuePosition << "\n";
        cout << "Estimated Waiting Time: " << estimatedWaitingTime << " minutes\n";
    }
};

class SmartQueueManagementSystem {
private:
    vector<Patient> patients;
    vector<Consultation> consultations;
    vector<PatientBill> bills;
    PatientQueueManager queue;

    string getCurrentDateTime() {
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        stringstream ss;
        ss << "19 February, 2025 / " << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
        return ss.str();
    }

    void viewQueueDetails(const string& qNum, int qPosition, int ewt) {
        cout << "=== Queue Details ===\n";
        cout << "Queue Number: " << qNum << "\n";
        cout << "Queue Position: " << qPosition << "\n";
        cout << "Estimated Waiting Time: " << ewt << " minutes\n";
    }

    void viewBills(const string& fullName) {
        bool billFound = false;
        for (const auto& bill : bills) {
            if (bill.patientName == fullName) {
                billFound = true;
                cout << "\n=== Clinic Bill ===\n";
                cout << "RM " << fixed << setprecision(2) << bill.getTotal() << "\n";
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
                cout << "Sub Total: RM " << bill.getTotal() << "\n";
                cout << "Total Amount Due: RM " << bill.getTotal() << "\n";
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

                PatientBill newBill(fullName, getCurrentDateTime());
                if (hasCriticalSymptoms) {
                    queue.checkInPatient(fullName, true);
                    int peopleAhead = 0;
                    int estimatedMinutes = 0;
                    cout << "\nCritical symptoms detected. Immediate attention required.\n";
                    cout << "CRITICAL\n";
                    cout << "\"PLEASE COME TO CLINIC IMMEDIATELY\"\n";
                    viewQueueDetails(queue.getQueueNumber(fullName), 1, estimatedMinutes);
                    cout << peopleAhead << " PEOPLE AHEAD\n";

                    auto now = chrono::system_clock::now();
                    time_t currentTime = chrono::system_clock::to_time_t(now);
                    tm* localTime = localtime(&currentTime);
                    stringstream ss;
                    ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
                    cout << ss.str() << "\n";
                    cout << "CLINIC STATUS: OPEN\n";

                    consultations.emplace_back(fullName, healthIssue, true);
                    newBill.items.emplace_back("Doctor Consultation", 1, 30.00);
                    if (answer2 == 'Y') {
                        newBill.items.emplace_back("Blood Test", 1, 50.00);
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

                    queue.checkInPatient(fullName, false);
                    bool isPatientCritical;
                    int position;
                    int peopleAhead = queue.calculateQueuePosition(fullName, isPatientCritical, position);
                    if (peopleAhead == -1) peopleAhead = max(0, (int)queue.isCheckedIn(fullName));
                    int estimatedMinutes = peopleAhead * 15;
                    cout << "\nConsultation booked successfully for " << timeSlot << ".\n";
                    viewQueueDetails(queue.getQueueNumber(fullName), position + 1, estimatedMinutes);
                    cout << peopleAhead << " PEOPLE AHEAD\n";

                    auto now = chrono::system_clock::now();
                    time_t currentTime = chrono::system_clock::to_time_t(now);
                    tm* localTime = localtime(&currentTime);
                    stringstream ss;
                    ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
                    cout << ss.str() << "\n";
                    cout << "CLINIC STATUS: OPEN\n";

                    consultations.emplace_back(fullName, healthIssue, false);
                    newBill.items.emplace_back("Doctor Consultation", 1, 30.00);
                    newBill.items.emplace_back("Fever Medication", 2, 10.00);
                }
                bills.push_back(newBill);
                Consultation::saveToFile(consultations);
                PatientBill::saveToFile(bills);
            } else if (patientChoice == 2) {
                if (queue.isCheckedIn(fullName)) {
                    cout << "You have already checked in for a consultation or medical check-up.\n";
                    continue;
                }

                queue.checkInPatient(fullName, false);
                bool isPatientCritical;
                int position;
                int peopleAhead = queue.calculateQueuePosition(fullName, isPatientCritical, position);
                if (peopleAhead == -1) peopleAhead = max(0, (int)queue.isCheckedIn(fullName));
                int estimatedMinutes = peopleAhead * 15;
                cout << "\nMedical Check-up registered successfully.\n";
                viewQueueDetails(queue.getQueueNumber(fullName), position + 1, estimatedMinutes);
                cout << peopleAhead << " PEOPLE AHEAD\n";

                auto now = chrono::system_clock::now();
                time_t currentTime = chrono::system_clock::to_time_t(now);
                tm* localTime = localtime(&currentTime);
                stringstream ss;
                ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
                cout << ss.str() << "\n";
                cout << "CLINIC STATUS: OPEN\n";

                consultations.emplace_back(fullName, "Medical Check-up", false);
                PatientBill newBill(fullName, getCurrentDateTime());
                newBill.items.emplace_back("Blood Test", 1, 50.00);
                bills.push_back(newBill);

                Consultation::saveToFile(consultations);
                PatientBill::saveToFile(bills);
            } else if (patientChoice == 3) {
                cout << "Returning to Home.\n";
                break;
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        } while (patientChoice != 3);
    }

public:
    SmartQueueManagementSystem() {
        patients = Patient::loadFromFile();
        consultations = Consultation::loadFromFile();
        bills = PatientBill::loadFromFile();
        queue.loadFromFile();
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

        for (const auto& p : patients) {
            if (p.getIcPassport() == icPassport) {
                cout << "IC/Passport Number already exists. Registration failed.\n";
                return;
            }
        }

        patients.emplace_back(fullName, password, email, mobileNumber, icPassport);
        Patient::saveToFile(patients);
        cout << "Registration successful.\n";
        showHomePage(fullName);
    }

    void showHomePage(const string& fullName) {
        int homeChoice;
        do {
            auto now = chrono::system_clock::now();
            time_t currentTime = chrono::system_clock::to_time_t(now);
            tm* localTime = localtime(&currentTime);
            stringstream ss;
            ss << setfill('0') << setw(2) << localTime->tm_hour << ":" << setw(2) << localTime->tm_min;
            cout << "\n" << ss.str() << "\n";
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
                int totalAhead = queue.calculateQueuePosition(fullName, isPatientCritical, position);

                if (position == -1) {
                    cout << "\nPlease Check-In First.\n";
                } else if (queue.isQueueExpired(fullName)) {
                    cout << "\nLATE ARRIVAL & QUEUE EXPIRED\n";
                    cout << "\"Your Queue Number Has Expired. Please Click The Button Below To Get New Queue Number\"\n";
                    cout << "1. REQUEST NEW QUEUE NUMBER\n";
                    cout << "2. CANCEL APPOINTMENT\n";
                    cout << "Choose an option: ";
                    int expiredChoice;
                    if (!(cin >> expiredChoice)) {
                        cout << "Invalid input. Please enter a number.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    cin.ignore();
                    if (expiredChoice == 1) {
                        queue.requestNewQueueNumber(fullName);
                    } else if (expiredChoice == 2) {
                        queue.cancelAppointment(fullName);
                    } else {
                        cout << "Invalid choice. Returning to Home.\n";
                    }
                } else {
                    string queueNumber = queue.getQueueNumber(fullName);
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

    void run() {
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
    }
};

int main() {
    SmartQueueManagementSystem system;
    system.run();
    return 0;
}
