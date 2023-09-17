#include <iostream>
#include <fstream>
#include "Algorithm.h"
using namespace std;

int main() {
    // Variable declaration for file reading
    char filename[200];
    ifstream file;

    // File opening
    cout << "Please enter your file name: " << endl;
    cin >> filename;
    file.open(filename);

    if (!file.is_open()) {
        cout << "ERROR: File: " << filename << " not found!" << endl;
        return 0;
    }

    int teller = 0;
    cout << "Please enter your number of tellers for the simulation: " << endl;
    cin >> teller;

    // Create the simulation
    Simulation bankSimulation(teller);


    // Reading Customer Data and adding them to the simulation
        float arrivalTime, serviceTime;
    int priority;
    while (file >> arrivalTime >> serviceTime >> priority) {
        if (arrivalTime == 0 && serviceTime == 0) {
            break;   // Dummy record that indicates the end of data
        }
        Customer customer(arrivalTime, serviceTime, priority);
        bankSimulation.enqueueCustomer(customer);   // Enqueue the customer directly
    }

    file.close();
    
    // Running simulation after enqueuing
    bankSimulation.runSimulation();


    cout << "Simulation finished. Displaying statistics:" << endl;

    bankSimulation.displayStatistics();
    cout << "End of Simulation." << endl;

    system("pause");


    return 0;
}
