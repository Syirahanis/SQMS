#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>

using namespace std;

vector<string> full_names;
vector<string> passwords;
vector<string> emails;
vector<string> mobile_numbers;
vector<string> ic_passports;

vector<string> consultation_patients;
vector<string> consultation_details;
vector<bool> consultation_critical;
vector<string> checked_in_patients;
vector<string> queue_numbers;
vector<bool> is_critical;

int current_queue_number = 1;
int current_critical_queue_number = 1;

string formatQueueNumber(int number, bool critical) {
    string prefix = critical ? "C" : "";
    stringstream ss;
    ss << prefix << setw(4) << setfill('0') << number;
    return ss.str();
}

int calculateQueuePosition(const string& fullName, bool& isCritical, int& position) {
    position = -1;
    int totalAhead = 0;
    for (size_t i = 0; i < checked_in_patients.size(); i++) {
        if (checked_in_patients[i] == fullName) {
            position = i;
            isCritical = is_critical[i];
            break;
        }
        if (!is_critical[i]) {
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

            if (hasCriticalSymptoms) {
                string queueNumber = formatQueueNumber(current_critical_queue_number++, true);
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

                checked_in_patients.push_back(fullName);
                queue_numbers.push_back(queueNumber);
                is_critical.push_back(true);

                consultation_patients.push_back(fullName);
                consultation_details.push_back(healthIssue);
                consultation_critical.push_back(true);
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

                bool isCritical;
                int position;
                string queueNumber = formatQueueNumber(current_queue_number++, false);
                int peopleAhead = calculateQueuePosition(fullName, isCritical, position);
                if (peopleAhead == -1) peopleAhead = max(0, (int)checked_in_patients.size());
                int estimatedMinutes = peopleAhead * 15;
                cout << "\nConsultation booked successfully for " << timeSlot << ".\n";
                cout << "QUEUE NO: " << queueNumber << "\n";
                cout << peopleAhead << " PEOPLE AHEAD\n";
                cout << "Estimated waiting time: " << estimatedMinutes << " minutes\n";
                cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
                cout << "16:04\n";
                cout << "CLINIC STATUS: OPEN\n";

                checked_in_patients.push_back(fullName);
                queue_numbers.push_back(queueNumber);
                is_critical.push_back(false);

                consultation_patients.push_back(fullName);
                consultation_details.push_back(healthIssue);
                consultation_critical.push_back(false);
            }
        } else if (patientChoice == 2) {
            bool alreadyCheckedIn = false;
            for (const auto& patient : checked_in_patients) {
                if (patient == fullName) {
                    alreadyCheckedIn = true;
                    break;
                }
            }
            if (alreadyCheckedIn) {
                cout << "You have already checked in for a consultation or medical check-up.\n";
                continue;
            }

            bool isCritical;
            int position;
            string queueNumber = formatQueueNumber(current_queue_number++, false);
            int peopleAhead = calculateQueuePosition(fullName, isCritical, position);
            if (peopleAhead == -1) peopleAhead = max(0, (int)checked_in_patients.size());
            int estimatedMinutes = peopleAhead * 15;
            cout << "\nMedical Check-up registered successfully.\n";
            cout << "QUEUE NO: " << queueNumber << "\n";
            cout << peopleAhead << " PEOPLE AHEAD\n";
            cout << "Estimated waiting time: " << estimatedMinutes << " minutes\n";
            cout << "PLEASE BE PATIENT WHILE WAITING YOUR QUEUE NUMBER\n";
            cout << "16:04\n";
            cout << "CLINIC STATUS: OPEN\n";

            checked_in_patients.push_back(fullName);
            queue_numbers.push_back(queueNumber);
            is_critical.push_back(false);

            consultation_patients.push_back(fullName);
            consultation_details.push_back("Medical Check-up");
            consultation_critical.push_back(false);
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
            bool isCritical;
            int position;
            string queueNumber = "";
            int totalAhead = calculateQueuePosition(fullName, isCritical, position);

            if (position == -1) {
                cout << "\nPlease Check-In First.\n";
            } else {
                queueNumber = queue_numbers[position];
                int estimatedMinutes = isCritical ? 0 : totalAhead * 15;
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
            cout << "\nBill is: 0.00\n";
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
    cin >> password;
    cin.ignore();
    
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
    for (const auto& ic : ic_passports) {
        if (ic == icPassport) {
            icExists = true;
            break;
        }
    }
    if (icExists) {
        cout << "IC/Passport Number already exists. Registration failed.\n";
        return;
    }
    
    full_names.push_back(fullName);
    passwords.push_back(password);
    emails.push_back(email);
    mobile_numbers.push_back(mobileNumber);
    ic_passports.push_back(icPassport);
    
    cout << "Registration successful.\n";
    
    showHomePage(fullName);
}

int main() {
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