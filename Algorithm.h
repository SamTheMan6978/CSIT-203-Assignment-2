#pragma once

class Customer;
class Teller;
class Queue;

class Queue {
private:
    Customer* queue;
    int capacity;
    int front;
    int rear;

public:
    Queue();
    ~Queue();

    int size();
    bool isFull();
    bool isEmpty();
    void enQueue(const Customer& customer);
    Customer deQueue();
    Customer peek();

    friend class Simulation;
};

class Simulation {
private:
    Teller* tellers;
    Queue customerQueue;
    int numTellers;
    float simulationClock;
    float runningQueueLengthSum = 0;
    float* tellerBecameIdleAt;
    int lastAssignedTeller;
    const float INVALID_TIME = -1.0f;

    
    // Statistics variables
    float totalTime;
    float totalServiceTime;
    float totalWaitingTime;
    int maxQueueLength;
    int totalCustomers;
    float* tellerIdleTime;
    int* customersServedByTeller; // Array to store number of customers served by each teller
    float totalQueuingTime;

public:
    Simulation(int numTellers);
    ~Simulation();

    void runSimulation();
    bool assignCustomerToTeller(const Customer& customer);
    void displayStatistics() const;
    void tellerCompletion(int tellerIndex);
    void enqueueCustomer(const Customer& customer);
    bool anyTellerActive();


    friend class Queue;
};

class Customer {
public:
    float arrivalTime;
    float serviceTime;
    int priority;

    Customer() : arrivalTime(0), serviceTime(0), priority(0) {}
    Customer(float a, float s, int p) : arrivalTime(a), serviceTime(s), priority(p) {}
};

class Teller {
public:
    bool idleStatus;   // Renamed from 'isIdle' to avoid the naming conflict
    float endTime;

    Teller() : idleStatus(true), endTime(0) {}

    bool isIdle() const { return idleStatus; }

    void serve(const Customer& customer, float currentSimulationTime);
    void becomeIdle();
};

