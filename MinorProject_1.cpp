#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

const char FILE_NAME[] = "stalls.dat";

class Stall {
private:
    char stallId[15];
    char vendorName[40];
    char category[25];
    int spaceSizeSqFt;
    double rentalFee;
    bool isBooked;

    static int totalStalls;

public:
    // 1. Default Constructor
    Stall() {
        strcpy(stallId, "UNKNOWN");
        strcpy(vendorName, "None");
        strcpy(category, "General");
        spaceSizeSqFt = 0;
        rentalFee = 0.0;
        isBooked = false;
    }

    // 2. Parameterized Constructor using 'this' pointer
    Stall(string id, string vendor, string cat, int size, double fee, bool booked) {
        strncpy(this->stallId, id.c_str(), sizeof(this->stallId) - 1);
        this->stallId[sizeof(this->stallId) - 1] = '\0';

        strncpy(this->vendorName, vendor.c_str(), sizeof(this->vendorName) - 1);
        this->vendorName[sizeof(this->vendorName) - 1] = '\0';

        strncpy(this->category, cat.c_str(), sizeof(this->category) - 1);
        this->category[sizeof(this->category) - 1] = '\0';

        this->spaceSizeSqFt = size;
        this->rentalFee = fee;
        this->isBooked = booked;
        totalStalls++;
    }

    // 3. Copy Constructor
    Stall(const Stall &other) {
        strcpy(this->stallId, other.stallId);
        strcpy(this->vendorName, other.vendorName);
        strcpy(this->category, other.category);
        this->spaceSizeSqFt = other.spaceSizeSqFt;
        this->rentalFee = other.rentalFee;
        this->isBooked = other.isBooked;
        totalStalls++;
    }

    // 4. Destructor
    ~Stall() {
        if (totalStalls > 0) {
            totalStalls--;
        }
    }

    // Inline function to get Stall ID
    inline string getStallId() const {
        return string(stallId);
    }

    // Update member values for existing record
    void updateDetails(string vendor, string cat, int size, double fee, bool booked) {
        strncpy(vendorName, vendor.c_str(), sizeof(vendorName) - 1);
        vendorName[sizeof(vendorName) - 1] = '\0';

        strncpy(category, cat.c_str(), sizeof(category) - 1);
        category[sizeof(category) - 1] = '\0';

        spaceSizeSqFt = size;
        rentalFee = fee;
        isBooked = booked;
    }

    // Display Stall Details
    void displayStall() const {
        cout << "ID: " << stallId << "\n"
             << "Vendor: " << vendorName << "\n"
             << "Category: " << category << "\n"
             << "Space Size: " << spaceSizeSqFt << " Sq. Ft.\n"
             << "Rental Fee: INR " << rentalFee << "\n"
             << "Status: " << (isBooked ? "Booked" : "Available") << endl;
    }

    // Friend Function declaration
    friend void auditStall(const Stall &s);

    // Static member function
    static int getTotalStallsCount() {
        return totalStalls;
    }
};

// Initialize static member
int Stall::totalStalls = 0;

// Friend Function definition
void auditStall(const Stall &s) {
    cout << "\n[Audit] Stall ID: " << s.stallId
         << " | Vendor: " << s.vendorName
         << " | Fee: INR " << s.rentalFee << endl;
}

// Helper: Find zero-based record index in binary file (-1 if not found)
int findRecordPosition(string targetId) {
    ifstream inFile(FILE_NAME, ios::binary);
    if (!inFile) {
        return -1;
    }

    Stall temp;
    int pos = 0;
    while (inFile.read((char*)&temp, sizeof(Stall))) {
        if (temp.getStallId() == targetId) {
            inFile.close();
            return pos;
        }
        pos++;
    }
    inFile.close();
    return -1;
}

int main() {
    int choice;

    do {
        cout << "\n--- FESTIVAL STALL PLANNER (CA2 - PERSIST & SEARCH) ---\n";
        cout << "1. Add Stall (Save to Binary File)\n";
        cout << "2. Display All Stalls\n";
        cout << "3. Search Stall by ID (seekg)\n";
        cout << "4. Update Stall Details (seekp)\n";
        cout << "5. Audit Stall (Friend Function)\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string id, vendor, cat;
                int size;
                double fee;

                cout << "Enter Stall ID: ";
                cin >> id;

                if (findRecordPosition(id) != -1) {
                    cout << "Error: Duplicate Stall ID already exists in file!\n";
                    break;
                }

                cout << "Enter Vendor Name: ";
                cin.ignore();
                getline(cin, vendor);
                cout << "Enter Category: ";
                getline(cin, cat);

                do {
                    cout << "Enter Space Size (Sq. Ft > 0): ";
                    cin >> size;
                } while (size <= 0);

                do {
                    cout << "Enter Rental Fee (>= 0): ";
                    cin >> fee;
                } while (fee < 0);

                Stall newStall(id, vendor, cat, size, fee, true);

                ofstream outFile(FILE_NAME, ios::binary | ios::app);
                if (!outFile) {
                    cout << "Error opening file for writing!\n";
                    break;
                }
                outFile.write((char*)&newStall, sizeof(Stall));
                outFile.close();
                cout << "Stall saved permanently to " << FILE_NAME << "!\n";
                break;
            }

            case 2: {
                ifstream inFile(FILE_NAME, ios::binary);
                if (!inFile) {
                    cout << "No stall records found yet.\n";
                    break;
                }

                Stall temp;
                bool hasRecords = false;
                // cout << "\nID\tVendor\t\tCategory\tSize\tFee\tStatus\n";
                // cout << "--------------------------------------------------------\n";
                while (inFile.read((char*)&temp, sizeof(Stall))) {
                    temp.displayStall();
                    hasRecords = true;
                }
                if (!hasRecords) {
                    cout << "File is empty.\n";
                }
                inFile.close();
                break;
            }

            case 3: {
                string searchId;
                cout << "Enter Stall ID to search: ";
                cin >> searchId;

                int pos = findRecordPosition(searchId);
                if (pos == -1) {
                    cout << "Stall ID not found.\n";
                    break;
                }

                ifstream inFile(FILE_NAME, ios::binary);
                Stall foundStall;
                // Random-access seek using seekg()
                inFile.seekg(pos * sizeof(Stall), ios::beg);
                inFile.read((char*)&foundStall, sizeof(Stall));
                inFile.close();

                cout << "\nRecord Found at Index " << pos << ":\n";
                foundStall.displayStall();
                break;
            }

            case 4: {
                string updateId;
                cout << "Enter Stall ID to update: ";
                cin >> updateId;

                int pos = findRecordPosition(updateId);
                if (pos == -1) {
                    cout << "Stall ID not found.\n";
                    break;
                }

                fstream file(FILE_NAME, ios::binary | ios::in | ios::out);
                if (!file) {
                    cout << "Error opening file for update!\n";
                    break;
                }

                Stall currentStall;
                file.seekg(pos * sizeof(Stall), ios::beg);
                file.read((char*)&currentStall, sizeof(Stall));

                // Invoke Copy Constructor to keep backup of old state
                Stall oldBackup = currentStall;
                cout << "[Backup Created] Previous Vendor: ";
                oldBackup.displayStall();

                string vendor, cat;
                int size, bookedInput;
                double fee;

                cout << "Enter New Vendor Name: ";
                cin.ignore();
                getline(cin, vendor);
                cout << "Enter New Category: ";
                getline(cin, cat);

                do {
                    cout << "Enter New Space Size (Sq. Ft > 0): ";
                    cin >> size;
                } while (size <= 0);

                do {
                    cout << "Enter New Rental Fee (>= 0): ";
                    cin >> fee;
                } while (fee < 0);

                cout << "Enter Booking Status (1 = Booked, 0 = Available): ";
                cin >> bookedInput;

                currentStall.updateDetails(vendor, cat, size, fee, (bookedInput == 1));

                // Random-access write using seekp()
                file.seekp(pos * sizeof(Stall), ios::beg);
                file.write((char*)&currentStall, sizeof(Stall));
                file.close();

                cout << "Stall record updated successfully using seekp()!\n";
                break;
            }

            case 5: {
                string auditId;
                cout << "Enter Stall ID to audit: ";
                cin >> auditId;

                int pos = findRecordPosition(auditId);
                if (pos == -1) {
                    cout << "Stall ID not found.\n";
                    break;
                }

                ifstream inFile(FILE_NAME, ios::binary);
                Stall s;
                inFile.seekg(pos * sizeof(Stall), ios::beg);
                inFile.read((char*)&s, sizeof(Stall));
                inFile.close();

                auditStall(s);
                break;
            }

            case 6:
                cout << "Exiting program... Data saved in " << FILE_NAME << ".\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 6);

    return 0;
}