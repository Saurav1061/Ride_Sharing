#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

class DriverRecords {
private:
    string DriverName;
    string DriverId;
    string mobile_number;

public:
    void set_DriverName(string Name) {
        DriverName = Name;
    }
    string get_DriverName() {
        return DriverName;
    }
    void set_DriverId(string Id) {
        DriverId = Id;
    }
    string get_DriverId() {
        return DriverId;
    }
    void set_Mobile_Number(string mobileNum) {
        mobile_number = mobileNum;
    }
    string get_Mobile_Number() {
        return mobile_number;
    }
};

class UserRecords {
private:
    string UserName;
    string UserId;
    string mobile_number;

public:
    void set_UserName(string Name) {
        UserName = Name;
    }
    string get_UserName() {
        return UserName;
    }
    void set_UserId(string user_Id) {
        UserId = user_Id;
    }
    string get_UserId() {
        return UserId;
    }
    void set_Mobile_Number(string mobileNum) {
        mobile_number = mobileNum;
    }
    string get_Mobile_Number() {
        return mobile_number;
    }
};

class Node {
private:
    Node* next;
    DriverRecords* driverElement;
    UserRecords* userElement;

public:
    Node() {
        next = nullptr;
        driverElement = nullptr;
        userElement = nullptr;
    }

    Node* get_next() {
        return next;
    }
    DriverRecords* get_driverElement() {
        return driverElement;
    }
    UserRecords* get_userElement() {
        return userElement;
    }

    void set_next(Node* value) {
        next = value;
    }
    void set_driverElement(DriverRecords* driver) {
        driverElement = driver;
    }
    void set_userElement(UserRecords* user) {
        userElement = user;
    }
};

// Entity class representing either Drivers or Users
class Entity {
private:
    string name;
    Node* iterator = nullptr;

public:
    string get_name() const {
        return name;
    }
    void set_name(string Name) {
        name = Name;
    }
    Node* get_iterator() const {
        return iterator;
    }
    void set_iterator(Node* iter) {
        iterator = iter;
    }
};

// LinkedList class for managing driver or user records
class LinkedList : public Entity {
public:
    void add_driver(DriverRecords& driver) {
        DriverRecords* drv = new DriverRecords(driver);
        Node* node = new Node();
        node->set_driverElement(drv);
        node->set_next(get_iterator());
        set_iterator(node);
    }

    void add_user(UserRecords& user) {
        UserRecords* usr = new UserRecords(user);
        Node* node = new Node();
        node->set_userElement(usr);
        node->set_next(get_iterator());
        set_iterator(node);
    }

    // Fixed delete_driver method
    void delete_driver(string driverName) {
        Node* prev = nullptr;
        Node* current = get_iterator();

        // If the list is empty
        if (current == nullptr) {
            return;
        }

        // If the first element is the one to be deleted
        if (current->get_driverElement() != nullptr && current->get_driverElement()->get_DriverName() == driverName) {
            set_iterator(current->get_next());
            delete current;  // Free memory
            return;
        }

        while (current != nullptr && current->get_driverElement()->get_DriverName() != driverName) {
            prev = current;
            current = current->get_next();
        }

        // If we found the driver to delete
        if (current != nullptr) {
            prev->set_next(current->get_next());
            delete current;  
        }
    }

    // Fixed delete_user method
    void delete_user(string userName) {
        Node* prev = nullptr;
        Node* current = get_iterator();

        // If the list is empty
        if (current == nullptr) {
            return;
        }

        // If the first element is the one to be deleted
        if (current->get_userElement() != nullptr && current->get_userElement()->get_UserName() == userName) {
            set_iterator(current->get_next());
            delete current;  
            return;
        }

        while (current != nullptr && current->get_userElement()->get_UserName() != userName) {
            prev = current;
            current = current->get_next();
        }

        // If we found the user to delete
        if (current != nullptr) {
            prev->set_next(current->get_next());
            delete current;  
        }
    }

    // Destructor to free allocated memory
    ~LinkedList() {
        Node* current = get_iterator();
        while (current != nullptr) {
            Node* next = current->get_next();
            delete current->get_driverElement(); 
            delete current->get_userElement(); 
            delete current;
            current = next;
        }
    }
};

// Vectors to store all drivers and users
vector<DriverRecords> drivers;
vector<UserRecords> users;

// Check if a driver already exists
bool check_duplicate_driver(string name) {
    for (auto& driver : drivers) {
        if (name == driver.get_DriverName()) {
            return true;  
        }
    }
    return false;  
}

// Check if a user already exists
bool check_duplicate_user(string name) {
    for (auto& user : users) {
        if (name == user.get_UserName()) {
            return true;  
        }
    }
    return false;  
}

// Read drivers from file and parse data
void read_driver_file(const string& file_path) {
    ifstream inputFile(file_path);
    if (!inputFile) {
        cerr << "Error: Could not open the file " << file_path << endl;
        return;
    }

    string line;
    
    while (getline(inputFile, line)) {
        istringstream iss(line);
        vector<string> values;
        string token;

        while (getline(iss, token, ',')) {
            values.push_back(token);
        }

        if (values.size() < 3) continue;  // Require at least 3 fields: Name, ID, Mobile

        // Create Driver record
        DriverRecords d;
        d.set_DriverName(values[0]);
        d.set_DriverId(values[1]);
        d.set_Mobile_Number(values[2]);

        // Check for duplicates before adding
        if (!check_duplicate_driver(d.get_DriverName())) {
            drivers.push_back(d);
        } else {
            cout << "Duplicate driver found: " << d.get_DriverName() << ". Skipping entry." << endl;
        }
    }

    inputFile.close();
}

// Read users from file and parse data
void read_user_file(const string& file_path) {
    ifstream inputFile(file_path);
    if (!inputFile) {
        cerr << "Error: Could not open the file " << file_path << endl;
        return;
    }

    string line;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        vector<string> values;
        string token;

        while (getline(iss, token, ',')) {
            values.push_back(token);
        }

        if (values.size() < 3) continue;  

        // Create User record
        UserRecords u;
        u.set_UserName(values[0]);
        u.set_UserId(values[1]);
        u.set_Mobile_Number(values[2]);

        // Check for duplicates before adding
        if (!check_duplicate_user(u.get_UserName())) {
            users.push_back(u);
        } else {
            cout << "Duplicate user found: " << u.get_UserName() << ". Skipping entry." << endl;
        }
    }

    inputFile.close();
}

int main() {
    // Example file paths for drivers and users
    string driver_file_path = "drivers.txt";
    string user_file_path = "users.txt";

    // Read driver and user data
    read_driver_file(driver_file_path);
    read_user_file(user_file_path);

    // Display all drivers
    cout << "Drivers List:-" << endl;
    cout<<endl ;
    int i = 1;
    for (auto& driver : drivers) {
        cout << i << ") Driver's Name:- " << driver.get_DriverName() << endl;
        cout << "   Driver's ID:- " << driver.get_DriverId() << endl;
        cout << "   Driver's Mobile No.:- " << driver.get_Mobile_Number() << endl;
        i++;
        cout << endl;
    }
    
    cout<<"-------------------------------------------------"<<endl ;
    cout<<endl ;
    // Display all users
    cout << "Users List:-" << endl;
    cout<<endl ;
    int j = 1;
    for (auto& user : users) {
        cout << j << ") User's Name:- " << user.get_UserName() << endl;
        cout << "   User's ID:- " << user.get_UserId() << endl;
        cout << "   User's Mobile No.:- " << user.get_Mobile_Number() << endl;
        j++;
        cout << endl;
    }

    return 0;
}
