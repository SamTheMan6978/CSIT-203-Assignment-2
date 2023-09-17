#pragma once  // Ensures the header is only included once in a compilation

// Forward declarations to allow for mutual references between classes
class Customer;
class Teller;
class Queue;

// Definition of the Queue class, which handles a queue of customers
class Queue {
private:
    Customer* queue;  // Dynamic array to store the customers
    int capacity;     // Total capacity of the queue
    int front;        // Front index of the queue
    int rear;         // Rear index of the queue

public:
    Queue();          // Constructor
    ~Queue();         // Destructor

    int size();                       // Returns the current size of the queue
    bool isFull();                    // Checks if the queue is full
    bool isEmpty();                   // Checks if the queue is empty
    void enQueue(const Customer& customer);  // Adds a customer to the queue
    Customer deQueue();               // Removes and returns the front customer from the queue
    Customer peek();                  // Peeks at the front customer without removing it

    friend class Simulation;          // Grants access to private members of Queue to the Simulation class
};

// Definition of the Simulation class, which handles the main bank simulation
class Simulation {
private:
    Teller* tellers;                  // Dynamic array to store the tellers
    Queue customerQueue;              // Queue of customers
    int numTellers;                  // Number of tellers
    float simulationClock;            // Current time in the simulation

    // Various simulation and statistics variables
    float runningQueueLengthSum = 0;
    float* tellerBecameIdleAt;
    int lastAssignedTeller;
    const float INVALID_TIME = -1.0f;

    float totalTime;
    float totalServiceTime;
    float totalWaitingTime;
    int maxQueueLength;
    int totalCustomers;
    float* tellerIdleTime;
    int* customersServedByTeller;
    float totalQueuingTime;

public:
    Simulation(int numTellers);       // Constructor
    ~Simulation();                    // Destructor

    void runSimulation();             // Runs the simulation
    bool assignCustomerToTeller(const Customer& customer);  // Assigns a customer to an available teller
    void displayStatistics() const;   // Displays simulation results
    void tellerCompletion(int tellerIndex);  // Handles when a teller completes serving a customer
    void enqueueCustomer(const Customer& customer);  // Adds a customer to the queue
    bool anyTellerActive();           // Checks if any teller is currently active

    friend class Queue;               // Grants access to private members of Simulation to the Queue class
};

// Definition of the Customer class, which represents a bank customer
class Customer {
public:
    float arrivalTime;    // Time when the customer arrives
    float serviceTime;    // Time required to serve the customer
    int priority;         // Priority of the customer

    // Constructors
    Customer() : arrivalTime(0), serviceTime(0), priority(0) {}
    Customer(float a, float s, int p) : arrivalTime(a), serviceTime(s), priority(p) {}
};

// Definition of the Teller class, which represents a bank teller
class Teller {
public:
    bool idleStatus;     // Current status of the teller (idle or busy)
    float endTime;       // Time when the teller will finish serving the current customer

    // Constructor
    Teller() : idleStatus(true), endTime(0) {}

    bool isIdle() const { return idleStatus; }  // Checks if the teller is idle

    // Methods
    void serve(const Customer& customer, float currentSimulationTime);  // Begins serving a customer
    void becomeIdle();  // Sets the teller to idle status
};
