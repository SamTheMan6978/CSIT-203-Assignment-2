#include "Algorithm.h"
#include <iostream>
#include <fstream>
using namespace std;

// MAX IMPLEMENTATION

float max(float a, float b) {
    return (a > b) ? a : b;
}

// QUEUE CLASS IMPLEMENTATIONS

Queue::Queue() : capacity(100), front(-1), rear(-1) {  
    queue = new Customer[capacity];
}

int Queue::size() {
    if (front == -1) return 0;
    return rear - front + 1;
}

Queue::~Queue() {
    delete[] queue;
}

bool Queue::isFull() {
    return (rear == capacity - 1);
}

bool Queue::isEmpty() {
    return (front == -1);
}

Customer Queue::peek() {
    if (isEmpty()) {
        cout << "Queue is empty!" << endl;
        return Customer(); // Return a default customer if the queue is empty
    }
    return queue[front];
}

void Queue::enQueue(const Customer& customer) {
    if (isFull()) {
        cout << "Queue is full! Can't add more customers." << endl;
        return;
    }
    if (front == -1) {
        front = 0;
        rear = 0;
        queue[rear] = customer;
    }
    else {
        int i;
        for (i = rear; i >= front; i--) {
            if (customer.priority > queue[i].priority ||
                (customer.priority == queue[i].priority && customer.arrivalTime <= queue[i].arrivalTime)) {
                queue[i + 1] = queue[i];  // Shift element forward
            }
            else {
                break;  // Found the insertion point
            }
        }
        queue[i + 1] = customer;  // Insert the new customer
        rear++;
    }
}



Customer Queue::deQueue() {
    if (isEmpty()) {
        cout << "Queue is empty!" << endl;
        return Customer(); // Return a default customer if the queue is empty
    }

    Customer dequeued = queue[front];
    if (front >= rear) {
        front = -1;
        rear = -1;
    }
    else {
        front++;
    }

    return dequeued;
}

// SIMULATION CLASS IMPLEMENTATIONS

Simulation::Simulation(int numTellers)
    : numTellers(numTellers), totalTime(0), totalServiceTime(0), totalWaitingTime(0),
    maxQueueLength(0), totalCustomers(0), simulationClock(0.0f), totalQueuingTime(0.0f), runningQueueLengthSum(0.0f), lastAssignedTeller(0) {

    tellers = new Teller[numTellers];
    tellerIdleTime = new float[numTellers];
    tellerBecameIdleAt = new float[numTellers];  // Initialize the new array

    customersServedByTeller = new int[numTellers];
    for (int i = 0; i < numTellers; i++) {
        tellerIdleTime[i] = 0.0f;
        tellerBecameIdleAt[i] = 0.0f;  // Initialize to 0
        customersServedByTeller[i] = 0;
    }
}

Simulation::~Simulation() {
    delete[] tellers;
    delete[] tellerIdleTime;
    delete[] customersServedByTeller;
}

bool Simulation::assignCustomerToTeller(const Customer& customer) {
    // Start from the teller next to the lastAssignedTeller
    int startIndex = (lastAssignedTeller + 1) % numTellers;

    // Loop through all tellers to check their idle status
    for (int i = 0; i < numTellers; i++) {
        int tellerIndex = (startIndex + i) % numTellers;
        
        if (tellers[tellerIndex].isIdle()) {
            // Serve the customer using this teller
            tellers[tellerIndex].serve(customer, simulationClock);
            customersServedByTeller[tellerIndex]++;
            
            // Update service time, waiting time, and metrics
            totalServiceTime += customer.serviceTime;

            totalWaitingTime += abs(simulationClock - customer.arrivalTime);

            // Update the lastAssignedTeller to this teller
            lastAssignedTeller = tellerIndex;
            
            return true;
        }
    }

    // If no teller is idle
    return false;
}



void Simulation::runSimulation() {
    Customer currentCustomer;

    while (!customerQueue.isEmpty() || anyTellerActive()) {
        float nextEventTime = -1;

        if (!customerQueue.isEmpty() && (nextEventTime < 0 || customerQueue.peek().arrivalTime < nextEventTime)) {
            nextEventTime = customerQueue.peek().arrivalTime;
        }
        for (int i = 0; i < numTellers; i++) {
            if (!tellers[i].isIdle() && (nextEventTime < 0 || tellers[i].endTime < nextEventTime)) {
                nextEventTime = tellers[i].endTime;
            }
        }

        float elapsedTime = nextEventTime - simulationClock;
        runningQueueLengthSum += customerQueue.size() * elapsedTime;

        // Check and update the maxQueueLength here
        if (customerQueue.size() > maxQueueLength) {
            maxQueueLength = customerQueue.size();
        }

        // Update tellers' status and their idle time
        for (int i = 0; i < numTellers; i++) {
            if (!tellers[i].isIdle()) {
                tellerCompletion(i); // Use the tellerCompletion method here
            }

            if (tellers[i].isIdle()) {
                float actualIdleTime = nextEventTime - tellerBecameIdleAt[i];
                tellerIdleTime[i] += actualIdleTime;
                tellerBecameIdleAt[i] = nextEventTime;  // Reset the idle start time to avoid double counting
            }
        }

        if (!customerQueue.isEmpty() && customerQueue.peek().arrivalTime == nextEventTime) {
            while (!customerQueue.isEmpty() && customerQueue.peek().arrivalTime == nextEventTime) {
                currentCustomer = customerQueue.peek();
                assignCustomerToTeller(currentCustomer);
                customerQueue.deQueue();
            }
        }

        simulationClock = nextEventTime;

        totalTime = simulationClock;
    }

    for (int i = 0; i < numTellers; i++) {
        if (tellers[i].isIdle()) {
            tellerIdleTime[i] += totalTime - tellerBecameIdleAt[i];
        }
    }
}


bool Simulation::anyTellerActive() {
    for (int i = 0; i < numTellers; i++) {
        if (!tellers[i].isIdle()) {
            return true;  // Found an active teller
        }
    }
    return false;  // All tellers are idle
}

void Simulation::enqueueCustomer(const Customer& customer) {
    customerQueue.enQueue(customer);
    totalCustomers++; // Increment the number of customers
}

void Simulation::tellerCompletion(int tellerIndex) {
    tellers[tellerIndex].becomeIdle();
    tellerBecameIdleAt[tellerIndex] = simulationClock;  // Set the time the teller became idle

    if (!customerQueue.isEmpty()) {
        // First, attempt to assign the customer to any idle teller
        for (int i = 0; i < numTellers; i++) {
            if (tellers[i].isIdle()) {
                Customer nextCustomer = customerQueue.deQueue();

                // Serve the customer directly
                tellers[i].serve(nextCustomer, simulationClock);

                // Increment the number of customers served by this teller
                customersServedByTeller[i]++;

                // Update total service time
                totalServiceTime += abs(nextCustomer.serviceTime);

                // Update total waiting time
                float currentWaitingTime = abs(simulationClock - nextCustomer.arrivalTime);
                totalWaitingTime += currentWaitingTime;

                // If there are no more customers in the queue, break out
                if (customerQueue.isEmpty()) {
                    break;
                }
            }
        }
    }
}


void Simulation::displayStatistics() const {

    cout << "Total time of the simulation: " << totalTime << " units" << endl;
    cout << "-----------------------------------------" << endl;

    for (int i = 0; i < numTellers; i++) {
        cout << "Teller " << i + 1 << " Statistics:" << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Number of customers served: " << customersServedByTeller[i] << endl;

        cout << "Teller idle time: " << tellerIdleTime[i] << "  " << "Total Time: " << totalTime << "  " << endl;

        float idleRate = (tellerIdleTime[i] / totalTime) * 100;
        cout << "Idle rate: " << idleRate << "%" << endl;
        cout << "-----------------------------------------" << endl;
    }

    cout << "Overall Statistics:" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Average service time per customer: " << totalServiceTime / totalCustomers << " units/customer" << endl;
    cout << "Average waiting time per customer: " << totalWaitingTime / totalCustomers << " units/customer" << endl;
    cout << "Maximum Queue Length: " << maxQueueLength << " customers" << endl;
    cout << "Average length of the queue: " << runningQueueLengthSum / totalTime << endl;
    cout << "-----------------------------------------" << endl;
}

// TELLER CLASS IMPLEMENTATIONS

void Teller::serve(const Customer& customer, float currentSimulationTime) {
    // Set the teller to busy
    idleStatus = false;

    // Set the endTime based on the current simulation time and the customer's serviceTime
    endTime = currentSimulationTime + customer.serviceTime;
}

void Teller::becomeIdle() {
    // Reset the teller to idle
    idleStatus = true;
    endTime = 0;  // reset the endTime as the teller is now free

}
