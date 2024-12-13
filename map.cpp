#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <conio.h>
#include <thread>
#include <chrono>
using namespace std;

const int max_cities = 100;
const float infinity = 1e6;
const float cost_per_km = 20.0;

#define RESET "\033[0m"            // resets text and bg
#define BOLD  "\033[1m"            // bold text
#define BLACK "\033[40m"           // bg color

void slowPrint(std::string& message, int delayMilliseconds) {
    for (char c : message) {                          // size_t data type, message length
        cout << BLACK << BOLD << c << RESET << flush; // flush to ensure character is printed immediately
        this_thread::sleep_for(chrono::milliseconds(delayMilliseconds));
    }
}


///////////////////////////////////////// Haversine /////////////////////////////////////////

// degrees to radians
float deg2rad(float deg) {
    return deg * (3.14159 / 180.0);
}

// since earth isn't flat, it's a sphere (oblate spheroid)
// haversine formula to calculate the distance between two cities
float haversine(float lat1, float lon1, float lat2, float lon2) {
    const float r = 6371.0; // radius of earth (km)

    float dLat = deg2rad(lat2 - lat1);
    float dLon = deg2rad(lon2 - lon1);
    float a = sin(dLat / 2) * sin(dLat / 2) +      // sin^2 * (△latitude/2) +
        cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *  // cos(lat1) * cos(lat2)  *
        sin(dLon / 2) * sin(dLon / 2);             // sin^2 * (△longitude/2)
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));     // central angle = how far 2 cities are in terms of angle around the earth, arctangent is used

    return r * c; // distance in km
}


///////////////////////////////////////// Linked List /////////////////////////////////////////

class Node {
    public:
        string city;
        float latitude;
        float longitude;
        Node* next;

        Node() {
            latitude = 0;
            longitude = 0;
            next = NULL;
        }

        Node(string city, float latitude, float longitude) {
            this->city = city;
            this->latitude = latitude;
            this->longitude = longitude;
            next = NULL;
        }
};

class LinkedList {
    public:

        Node* head;

        LinkedList() {
            head = NULL;
        }

        void addNode(string city, float latitude, float longitude) {
            Node* new_node = new Node(city, latitude, longitude);
            if (head == NULL) {
                head = new_node;
            }
            else {
                Node* temp = head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = new_node;
            }
        }

        Node* searchCity(string city) {
            Node* temp = head;
            while (temp != NULL) {
                if (temp->city == city) {
                    return temp;
                }
                temp = temp->next;
            }
            return NULL; // city not found
        }

        void displayCities() {
            Node* temp = head;
            while (temp) {
                cout << "City: " << temp->city << " | Latitude: " << temp->latitude << " | Longitude: " << temp->longitude << endl;
                temp = temp->next;
            }
        }

        ~LinkedList() {
            Node* temp = head;
            while (temp != NULL) {
                Node* node_to_delete = temp;
                temp = temp->next;
                delete node_to_delete;
            }
        }
};


///////////////////////////////////////// Graph /////////////////////////////////////////

class Vertex {
    public:
        string city;
        float latitude;
        float longitude;
        vector<Vertex*> neighbors; // list of neighboring cities (adjacent vertices)

        float shortestDistance;
        bool visited;
        bool settled;
        vector<Vertex*> shortestPath;

        Vertex(string city, float latitude, float longitude) {
            this->city = city;
            this->latitude = latitude;
            this->longitude = longitude;
            this->shortestDistance = infinity; // infinity is defined
            this->visited = false;
            this->settled = false;
        }

        void addNeighbor(Vertex* neighbor) {
            for (int i = 0; i < neighbors.size(); i++) {  // check if the neighbor is already in the list to prevent duplicates
                if (neighbors[i] == neighbor) {
                    return;  // neighbor is already added
                }
            }
            neighbors.push_back(neighbor);
        }

        double calculateDistance(Vertex* other) {
            return haversine(this->latitude, this->longitude, other->latitude, other->longitude);
        }
};

class Graph {
    private:
        vector<Vertex*> vertices;

    public:
        void addVertex(string city, float latitude, float longitude) {
            Vertex* newCity = new Vertex(city, latitude, longitude);
            vertices.push_back(newCity);
        }

        Vertex* getVertex(string city) {
            for (int i = 0; i < vertices.size(); i++) {
                Vertex* v = vertices[i];
                if (v->city == city) {
                    return v;
                }
            }
            return NULL; // city not found
        }

        void addEdge(string city1, string city2) {
            Vertex* v1 = getVertex(city1);
            Vertex* v2 = getVertex(city2);

            if (v1 != NULL && v2 != NULL) { // no object as such exists
                v1->addNeighbor(v2);        // add v2 as a neighbor of v1
                v2->addNeighbor(v1);        // add v1 as a neighbor of v2 (for undirected graph)
            }
        }

        void displayAdjacencyList() {
            for (int i = 0; i < vertices.size(); i++) {
                Vertex* v = vertices[i];
                cout << v->city << " | ";
                for (int j = 0; j < v->neighbors.size(); j++)
                    cout << v->neighbors[j]->city << " ";
                cout << endl;
            }
        }
};


///////////////////////////////////////// Min Heap Binary Tree /////////////////////////////////////////

class MinHeap {
    private:
        vector<Vertex*> heap;

        int getParent(int index) { return (index - 1) / 2; }
        int getLeftChild(int index) { return 2 * index + 1; }
        int getRightChild(int index) { return 2 * index + 2; }

        void heapifyUp(int index) { // insertion
            if (index == 0) return;  // root node, no parent
            int parentIndex = getParent(index);
            if (heap[index]->shortestDistance < heap[parentIndex]->shortestDistance) {
                Vertex* temp = heap[index]; // swap
                heap[index] = heap[parentIndex];
                heap[parentIndex] = temp;
                heapifyUp(parentIndex);
            }
        }

        void heapifyDown(int index) { // extraction
            int leftChild = getLeftChild(index);
            int rightChild = getRightChild(index);
            int smallest = index;

            if (leftChild < heap.size() && heap[leftChild]->shortestDistance < heap[smallest]->shortestDistance)
                smallest = leftChild;

            if (rightChild < heap.size() && heap[rightChild]->shortestDistance < heap[smallest]->shortestDistance)
                smallest = rightChild;

            if (smallest != index) {
                Vertex* temp = heap[index]; // swap
                heap[index] = heap[smallest];
                heap[smallest] = temp;
                heapifyDown(smallest);
            }
        }

    public:
        MinHeap() = default;

        void insert(Vertex* element) {
            heap.push_back(element);
            heapifyUp(heap.size() - 1);
        }

        Vertex* extractMin() {
            if (heap.empty()) {
                cout << "Heap is empty" << endl;
            }
            Vertex* minElement = heap[0];
            heap[0] = heap.back();
            heap.pop_back();
            heapifyDown(0);
            return minElement;
        }

        bool isEmpty() const {
            return heap.empty();
        }
};


///////////////////////////////////////// Dijkstra /////////////////////////////////////////

class Dijkstra {
    private:
        static void calculateShortestPath(Vertex* source) { // O(E*logV)
            source->shortestDistance = 0;
            MinHeap unsettledVertices;

            unsettledVertices.insert(source);

            while (!unsettledVertices.isEmpty()) { // until heap is not empty, i.e. there is atleast some node which can be further explored
                Vertex* currentVertex = unsettledVertices.extractMin();

                if (currentVertex->visited) continue;
                currentVertex->visited = true;

                for (int i = 0; i < currentVertex->neighbors.size(); i++) { // getting all unvisited neighbours for the current vertex 
                    Vertex* adjacentVertex = currentVertex->neighbors[i];
                    if (!adjacentVertex->visited) {

                        // calculate distance between the current and adjacent vertices using haversine through graph class
                        float edgeDistance = currentVertex->calculateDistance(adjacentVertex);
                        evaluateDistanceAndPath(adjacentVertex, currentVertex, edgeDistance);
                        if (!adjacentVertex->settled) {
                            unsettledVertices.insert(adjacentVertex);
                            adjacentVertex->settled = true;
                        }
                    }
                }
            }
        }

        static vector<Vertex*> getPath(Vertex* destination) { // O(V)
            vector<Vertex*> paths = destination->shortestPath;
            paths.push_back(destination);
            return paths;
        }

        static void evaluateDistanceAndPath(Vertex* adjacentVertex, Vertex* sourceVertex, float edgeDistance) { // O(V)
            float newDistance = sourceVertex->shortestDistance + edgeDistance;

            if (newDistance < adjacentVertex->shortestDistance) {
                adjacentVertex->shortestDistance = newDistance;
                adjacentVertex->shortestPath = sourceVertex->shortestPath;
                adjacentVertex->shortestPath.push_back(sourceVertex);
            }
        }

    public:
        static vector<Vertex*> getShortestPath(Vertex* from, Vertex* to) { // O(E*logV)
            calculateShortestPath(from);
            return getPath(to);
        }
};


///////////////////////////////////////// Passenger /////////////////////////////////////////

class Passenger {
    public:
        int ID;
        string name;
        char passenger_type;

        Passenger()
        {
            ID = 0;
            name = "";
            passenger_type = '\0';
        }

        Passenger(int ID, string name, char passenger_type) {
            this->ID = ID;
            this->name = name;
            this->passenger_type = passenger_type;
        }

        void display_details() {
            string category;

            cout << "Passenger ID = " << ID << endl;
            cout << "Passenger name = " << name << endl;

            if (passenger_type == 'A' || passenger_type == 'a') {
                category = "Adult";
            }
            else if (passenger_type == 'S' || passenger_type == 's') {
                category = "Student";
            }
            else if (passenger_type == 'E' || passenger_type == 'e') {
                category = "Elderly";
            }
            else {
                category = "Default Adult";
            }
            cout << "Passenger category = " << category << endl;
            
        }

        virtual void cost_cal(float distance, int cost_per_KM) = 0;
};

class Student : public Passenger {
    public:
        Student(int ID, string name, char passenger_type) {
            this->ID = ID;
            this->name = name;
            this->passenger_type = passenger_type;
        }

        void cost_cal(float distance, int cost_per_KM) override {
            float cost = distance * cost_per_KM;
            cout << "Price before student discount = Rs. " << cost << endl;
            cost = cost * 0.5;
            cout << "Total cost after student discount (50 %) = Rs. " << cost << endl;
        }
};

class Adult : public Passenger {
    public:
        Adult(int ID, string name, char passenger_type) {
            this->ID = ID;
            this->name = name;
            this->passenger_type = passenger_type;
        }

        void cost_cal(float distance, int cost_per_KM) override {
            float cost = distance * cost_per_KM;
            cout << "No discount applicable :(" << endl;
            cout << "Total cost = Rs. " << cost << endl;
        }
};

class Elderly : public Passenger {
    public:
        Elderly(int ID, string name, char passenger_type) {
            this->ID = ID;
            this->name = name;
            this->passenger_type = passenger_type;
        }

        void cost_cal(float distance, int cost_per_KM) override {
            float cost = distance * cost_per_KM;
            cout << "Price before elderly discount = Rs. " << cost << endl;
            cost = cost * 0.8;
            cout << "Total cost after elderly discount (80 %) = Rs. " << cost << endl;
        }
};

class Vehicle {
    protected:
        int cost_per_KM;

    public:
        Vehicle() {
            cost_per_KM = 0;
        }
        Vehicle(int cost) {
            cost_per_KM = cost;
        }
        int get_cost_per_KM() {
            return cost_per_KM; 
        }
};

class Mini : public Vehicle {
    public:
        Mini() {
            cost_per_KM = 20;
            cout << "Mini Vehicle: Cost per KM = Rs." << cost_per_KM << endl;
        }
};

class Standard : public Vehicle {
    public:
        Standard() {
            cost_per_KM = 50;
            cout << "Standard Vehicle: Cost per KM = Rs." << cost_per_KM << endl;
        }
};

class Luxury : public Vehicle {
    public:
        Luxury() {
            cost_per_KM = 100;
            cout << "Luxury Vehicle: Cost per KM = Rs." << cost_per_KM << endl;
        }
};


///////////////////////////////////////// Main /////////////////////////////////////////

int main() {

    LinkedList ll;

    ll.addNode("Badin", 24.6558, 68.8383);
    ll.addNode("Thatta", 24.7461, 67.9243);
    ll.addNode("Karachi", 24.86, 67.01);
    ll.addNode("Gwadar", 25.1264, 62.3225);
    ll.addNode("Pasni", 25.251, 63.4154);
    ll.addNode("Ormara", 25.2666, 64.6096);
    ll.addNode("Hyderabad", 25.3792, 68.3683);
    ll.addNode("Tando Allahyar", 25.4667, 68.7167);
    ll.addNode("Lasbela", 25.87, 66.7129);
    ll.addNode("Turbat", 26.0031, 63.0544);
    ll.addNode("Sanghar", 26.0464, 68.9481);
    ll.addNode("Nawabshah", 26.2442, 68.41);
    ll.addNode("Panjgur", 26.9706, 64.0887);
    ll.addNode("Khairpur", 27.5295, 68.7592);
    ll.addNode("Larkana", 27.56, 68.2264);
    ll.addNode("Sukkur", 27.6995, 68.8673);
    ll.addNode("Khuzdar", 27.8, 66.6167);
    ll.addNode("Basima", 27.9094, 65.8739);
    ll.addNode("Shikarpur", 27.9556, 68.6382);
    ll.addNode("Jacobabad", 28.2769, 68.4514);
    ll.addNode("Rahim Yar Khan", 28.4202, 70.2952);
    ll.addNode("Kashmore", 28.4482, 69.5857);
    ll.addNode("Surab", 28.4901, 66.2635);
    ll.addNode("Kharan", 28.5855, 65.4163);
    ll.addNode("Dalbandin", 28.8854, 64.3964);
    ll.addNode("Kalat", 29.0258, 66.59);
    ll.addNode("Dera Bugti", 29.0362, 69.1585);
    ll.addNode("Rajanpur", 29.1041, 70.3297);
    ll.addNode("Bahawalpur", 29.3956, 71.6722);
    ll.addNode("Sibi", 29.543, 67.8773);
    ll.addNode("Noshki", 29.5514, 66.0107);
    ll.addNode("Dera Ghazi Khan", 30.05, 70.6333);
    ll.addNode("Muzaffargarh", 30.0703, 71.1933);
    ll.addNode("Quetta", 30.192, 67.007);
    ll.addNode("Multan", 30.1978, 71.4711);
    ll.addNode("Khanewal", 30.3017, 71.9321);
    ll.addNode("Pashin", 30.5833, 67);
    ll.addNode("Sahiwal", 30.6706, 73.1064);
    ll.addNode("Okara", 30.81, 73.4597);
    ll.addNode("Chaman", 30.921, 66.4597);
    ll.addNode("Toba Tek Singh", 30.9667, 72.4833);
    ll.addNode("Jhang", 31.2681, 72.3181);
    ll.addNode("Zhob", 31.3417, 69.4486);
    ll.addNode("Faisalabad", 31.418, 73.079);
    ll.addNode("Lahore", 31.5497, 74.3436);
    ll.addNode("Sheikhupura", 31.7083, 74);
    ll.addNode("Dera Ismail Khan", 31.8167, 70.9167);
    ll.addNode("Hafizabad", 32.0709, 73.688);
    ll.addNode("Sargodha", 32.0836, 72.6711);
    ll.addNode("Gujranwala", 32.15, 74.1833);
    ll.addNode("Khushab", 32.2917, 72.35);
    ll.addNode("Sialkot", 32.5, 74.5333);
    ll.addNode("Gujrat", 32.5736, 74.0789);
    ll.addNode("Mianwali", 32.5853, 71.5436);
    ll.addNode("Mandi Bahauddin", 32.5861, 73.4917);
    ll.addNode("Chakwal", 32.93, 72.85);
    ll.addNode("Jhelum", 32.9333, 73.7333);
    ll.addNode("Bannu", 32.9889, 70.6056);
    ll.addNode("Karak", 33.1167, 71.0833);
    ll.addNode("Kohat", 33.5869, 71.4414);
    ll.addNode("Rawalpindi", 33.6007, 73.0679);
    ll.addNode("Islamabad", 33.6989, 73.0369);
    ll.addNode("Attock", 33.7667, 72.3667);
    ll.addNode("Murree", 33.907, 73.3943);
    ll.addNode("Peshawar", 34, 71.5);
    ll.addNode("Nowshera", 34.0153, 71.9747);
    ll.addNode("Charsadda", 34.1453, 71.7308);
    ll.addNode("Abbottabad", 34.15, 73.2167);
    ll.addNode("Mardan", 34.1958, 72.0447);
    ll.addNode("Mohmand", 34.5356, 71.2874);
    ll.addNode("Malakand", 34.5656, 71.9304);
    ll.addNode("Dir", 35.1977, 71.8749);
    ll.addNode("Swat", 35.2227, 72.4258);

    Graph graph;

    Node* head = ll.head;
    while (head->next != NULL) {
        graph.addVertex(head->city, head->latitude, head->longitude);
        head = head->next;
    }

    graph.addEdge("Badin", "Thatta");

    graph.addEdge("Thatta", "Karachi");
    graph.addEdge("Thatta", "Hyderabad");
    graph.addEdge("Thatta", "Badin");

    graph.addEdge("Karachi", "Hyderabad");
    graph.addEdge("Karachi", "Thatta");
    graph.addEdge("Karachi", "Lasbela");

    graph.addEdge("Gwadar", "Turbat");
    graph.addEdge("Gwadar", "Pasni");

    graph.addEdge("Pasni", "Gwadar");
    graph.addEdge("Pasni", "Turbat");
    graph.addEdge("Pasni", "Ormara");

    graph.addEdge("Ormara", "Pasni");
    graph.addEdge("Ormara", "Lasbela");

    graph.addEdge("Hyderabad", "Karachi");
    graph.addEdge("Hyderabad", "Sanghar");
    graph.addEdge("Hyderabad", "Nawabshah");
    graph.addEdge("Hyderabad", "Tando Allahyar");
    graph.addEdge("Hyderabad", "Thatta");

    graph.addEdge("Tando Allahyar", "Hyderabad");

    graph.addEdge("Lasbela", "Ormara");
    graph.addEdge("Lasbela", "Karachi");
    graph.addEdge("Lasbela", "Khuzdar");

    graph.addEdge("Turbat", "Gwadar");
    graph.addEdge("Turbat", "Panjgur");
    graph.addEdge("Turbat", "Pasni");

    graph.addEdge("Sanghar", "Hyderabad");
    graph.addEdge("Sanghar", "Nawabshah");

    graph.addEdge("Nawabshah", "Hyderabad");
    graph.addEdge("Nawabshah", "Sanghar");
    graph.addEdge("Nawabshah", "Khairpur");

    graph.addEdge("Panjgur", "Turbat");
    graph.addEdge("Panjgur", "Basima");

    graph.addEdge("Khairpur", "Larkana");
    graph.addEdge("Khairpur", "Nawabshah");
    graph.addEdge("Khairpur", "Sukkur");

    graph.addEdge("Larkana", "Shikarpur");
    graph.addEdge("Larkana", "Sukkur");
    graph.addEdge("Larkana", "Jacobabad");
    graph.addEdge("Larkana", "Khairpur");
    graph.addEdge("Larkana", "Khuzdar");

    graph.addEdge("Sukkur", "Larkana");
    graph.addEdge("Sukkur", "Shikarpur");
    graph.addEdge("Sukkur", "Kashmore");
    graph.addEdge("Sukkur", "Khairpur");
    graph.addEdge("Sukkur", "Rahim Yar Khan");

    graph.addEdge("Khuzdar", "Lasbela");
    graph.addEdge("Khuzdar", "Larkana");
    graph.addEdge("Khuzdar", "Surab");
    graph.addEdge("Khuzdar", "Basima");

    graph.addEdge("Basima", "Surab");
    graph.addEdge("Basima", "Khuzdar");
    graph.addEdge("Basima", "Panjgur");
    graph.addEdge("Basima", "Kharan");

    graph.addEdge("Shikarpur", "Sukkur");
    graph.addEdge("Shikarpur", "Jacobabad");
    graph.addEdge("Shikarpur", "Larkana");
    graph.addEdge("Shikarpur", "Kashmore");

    graph.addEdge("Jacobabad", "Larkana");
    graph.addEdge("Jacobabad", "Shikarpur");
    graph.addEdge("Jacobabad", "Sibi");

    graph.addEdge("Rahim Yar Khan", "Multan");
    graph.addEdge("Rahim Yar Khan", "Rajanpur");
    graph.addEdge("Rahim Yar Khan", "Bahawalpur");
    graph.addEdge("Rahim Yar Khan", "Sukkur");

    graph.addEdge("Kashmore", "Sukkur");
    graph.addEdge("Kashmore", "Shikarpur");
    graph.addEdge("Kashmore", "Rajanpur");

    graph.addEdge("Surab", "Khuzdar");
    graph.addEdge("Surab", "Kalat");
    graph.addEdge("Surab", "Basima");

    graph.addEdge("Kharan", "Basima");
    graph.addEdge("Kharan", "Noshki");

    graph.addEdge("Dalbandin", "Noshki");

    graph.addEdge("Kalat", "Quetta");
    graph.addEdge("Kalat", "Surab");

    graph.addEdge("Dera Bugti", "Sibi");

    graph.addEdge("Rajanpur", "Dera Ghazi Khan");
    graph.addEdge("Rajanpur", "Rahim Yar Khan");
    graph.addEdge("Rajanpur", "Kashmore");

    graph.addEdge("Bahawalpur", "Multan");
    graph.addEdge("Bahawalpur", "Rahim Yar Khan");
    graph.addEdge("Bahawalpur", "Sahiwal");
    graph.addEdge("Bahawalpur", "Dera Ghazi Khan");
    graph.addEdge("Bahawalpur", "Khanewal");

    graph.addEdge("Sibi", "Dera Bugti");
    graph.addEdge("Sibi", "Quetta");
    graph.addEdge("Sibi", "Jacobabad");

    graph.addEdge("Noshki", "Quetta");
    graph.addEdge("Noshki", "Dalbandin");
    graph.addEdge("Noshki", "Kharan");

    graph.addEdge("Dera Ghazi Khan", "Rajanpur");
    graph.addEdge("Dera Ghazi Khan", "Muzaffargarh");
    graph.addEdge("Dera Ghazi Khan", "Bahawalpur");
    graph.addEdge("Dera Ghazi Khan", "Dera Ismail Khan");

    graph.addEdge("Muzaffargarh", "Dera Ghazi Khan");
    graph.addEdge("Muzaffargarh", "Multan");

    graph.addEdge("Quetta", "Pashin");
    graph.addEdge("Quetta", "Zhob");
    graph.addEdge("Quetta", "Kalat");
    graph.addEdge("Quetta", "Sibi");
    graph.addEdge("Quetta", "Chaman");
    graph.addEdge("Quetta", "Noshki");

    graph.addEdge("Multan", "Muzaffargarh");
    graph.addEdge("Multan", "Bahawalpur");
    graph.addEdge("Multan", "Khanewal");
    graph.addEdge("Multan", "Rahim Yar Khan");

    graph.addEdge("Khanewal", "Multan");
    graph.addEdge("Khanewal", "Bahawalpur");
    graph.addEdge("Khanewal", "Toba Tek Singh");
    graph.addEdge("Khanewal", "Sahiwal");

    graph.addEdge("Pashin", "Quetta");
    graph.addEdge("Pashin", "Zhob");
    graph.addEdge("Pashin", "Chaman");

    graph.addEdge("Sahiwal", "Bahawalpur");
    graph.addEdge("Sahiwal", "Khanewal");
    graph.addEdge("Sahiwal", "Okara");

    graph.addEdge("Okara", "Lahore");
    graph.addEdge("Okara", "Sahiwal");
    graph.addEdge("Okara", "Faisalabad");
    graph.addEdge("Okara", "Sheikhupura");
    graph.addEdge("Okara", "Jhang");

    graph.addEdge("Chaman", "Pashin");
    graph.addEdge("Chaman", "Quetta");

    graph.addEdge("Toba Tek Singh", "Faisalabad");
    graph.addEdge("Toba Tek Singh", "Jhang");
    graph.addEdge("Toba Tek Singh", "Khanewal");

    graph.addEdge("Jhang", "Faisalabad");
    graph.addEdge("Jhang", "Sargodha");
    graph.addEdge("Jhang", "Toba Tek Singh");
    graph.addEdge("Jhang", "Okara");

    graph.addEdge("Zhob", "Quetta");
    graph.addEdge("Zhob", "Pashin");
    graph.addEdge("Zhob", "Dera Ismail Khan");

    graph.addEdge("Faisalabad", "Lahore");
    graph.addEdge("Faisalabad", "Jhang");
    graph.addEdge("Faisalabad", "Toba Tek Singh");
    graph.addEdge("Faisalabad", "Sargodha");
    graph.addEdge("Faisalabad", "Okara");
    graph.addEdge("Faisalabad", "Hafizabad");

    graph.addEdge("Lahore", "Faisalabad");
    graph.addEdge("Lahore", "Sheikhupura");
    graph.addEdge("Lahore", "Gujranwala");
    graph.addEdge("Lahore", "Sargodha");
    graph.addEdge("Lahore", "Hafizabad");
    graph.addEdge("Lahore", "Okara");

    graph.addEdge("Sheikhupura", "Lahore");
    graph.addEdge("Sheikhupura", "Gujranwala");
    graph.addEdge("Sheikhupura", "Okara");

    graph.addEdge("Dera Ismail Khan", "Bannu");
    graph.addEdge("Dera Ismail Khan", "Zhob");
    graph.addEdge("Dera Ismail Khan", "Dera Ghazi Khan");
    graph.addEdge("Dera Ismail Khan", "Mianwali");

    graph.addEdge("Hafizabad", "Gujranwala");
    graph.addEdge("Hafizabad", "Gujrat");
    graph.addEdge("Hafizabad", "Lahore");
    graph.addEdge("Hafizabad", "Faisalabad");

    graph.addEdge("Sargodha", "Khushab");
    graph.addEdge("Sargodha", "Faisalabad");
    graph.addEdge("Sargodha", "Jhang");
    graph.addEdge("Sargodha", "Gujranwala");
    graph.addEdge("Sargodha", "Mandi Bahauddin");
    graph.addEdge("Sargodha", "Gujrat");
    graph.addEdge("Sargodha", "Jhelum");
    graph.addEdge("Sargodha", "Lahore");
    graph.addEdge("Sargodha", "Chakwal");

    graph.addEdge("Gujranwala", "Lahore");
    graph.addEdge("Gujranwala", "Sheikhupura");
    graph.addEdge("Gujranwala", "Hafizabad");
    graph.addEdge("Gujranwala", "Sialkot");
    graph.addEdge("Gujranwala", "Gujrat");
    graph.addEdge("Gujranwala", "Mandi Bahauddin");
    graph.addEdge("Gujranwala", "Sargodha");

    graph.addEdge("Khushab", "Jhelum");
    graph.addEdge("Khushab", "Sargodha");
    graph.addEdge("Khushab", "Mianwali");
    graph.addEdge("Khushab", "Chakwal");

    graph.addEdge("Sialkot", "Gujranwala");
    graph.addEdge("Sialkot", "Gujrat");

    graph.addEdge("Gujrat", "Gujranwala");
    graph.addEdge("Gujrat", "Jhelum");
    graph.addEdge("Gujrat", "Sargodha");
    graph.addEdge("Gujrat", "Mandi Bahauddin");
    graph.addEdge("Gujrat", "Hafizabad");
    graph.addEdge("Gujrat", "Islamabad");
    graph.addEdge("Gujrat", "Rawalpindi");
    graph.addEdge("Gujrat", "Sialkot");

    graph.addEdge("Mianwali", "Jhelum");
    graph.addEdge("Mianwali", "Dera Ismail Khan");
    graph.addEdge("Mianwali", "Khushab");

    graph.addEdge("Mandi Bahauddin", "Gujranwala");
    graph.addEdge("Mandi Bahauddin", "Gujrat");
    graph.addEdge("Mandi Bahauddin", "Sargodha");

    graph.addEdge("Chakwal", "Attock");
    graph.addEdge("Chakwal", "Rawalpindi");
    graph.addEdge("Chakwal", "Jhelum");
    graph.addEdge("Chakwal", "Sargodha");
    graph.addEdge("Chakwal", "Khushab");

    graph.addEdge("Jhelum", "Rawalpindi");
    graph.addEdge("Jhelum", "Gujrat");
    graph.addEdge("Jhelum", "Sargodha");
    graph.addEdge("Jhelum", "Mianwali");
    graph.addEdge("Jhelum", "Khushab");
    graph.addEdge("Jhelum", "Islamabad");
    graph.addEdge("Jhelum", "Chakwal");

    graph.addEdge("Bannu", "Karak");
    graph.addEdge("Bannu", "Dera Ismail Khan");

    graph.addEdge("Karak", "Bannu");
    graph.addEdge("Karak", "Kohat");

    graph.addEdge("Kohat", "Karak");
    graph.addEdge("Kohat", "Peshawar");

    graph.addEdge("Rawalpindi", "Islamabad");
    graph.addEdge("Rawalpindi", "Jhelum");
    graph.addEdge("Rawalpindi", "Gujrat");
    graph.addEdge("Rawalpindi", "Chakwal");
    graph.addEdge("Rawalpindi", "Attock");

    graph.addEdge("Islamabad", "Rawalpindi");
    graph.addEdge("Islamabad", "Jhelum");
    graph.addEdge("Islamabad", "Gujrat");
    graph.addEdge("Islamabad", "Attock");
    graph.addEdge("Islamabad", "Murree");

    graph.addEdge("Attock", "Rawalpindi");
    graph.addEdge("Attock", "Chakwal");
    graph.addEdge("Attock", "Islamabad");
    graph.addEdge("Attock", "Nowshera");

    graph.addEdge("Murree", "Islamabad");
    graph.addEdge("Murree", "Abbottabad");

    graph.addEdge("Peshawar", "Charsadda");
    graph.addEdge("Peshawar", "Nowshera");
    graph.addEdge("Peshawar", "Kohat");
    graph.addEdge("Peshawar", "Mohmand");

    graph.addEdge("Nowshera", "Charsadda");
    graph.addEdge("Nowshera", "Peshawar");
    graph.addEdge("Nowshera", "Attock");
    graph.addEdge("Nowshera", "Mardan");

    graph.addEdge("Charsadda", "Peshawar");
    graph.addEdge("Charsadda", "Mardan");
    graph.addEdge("Charsadda", "Nowshera");

    graph.addEdge("Abbottabad", "Murree");

    graph.addEdge("Mardan", "Malakand");
    graph.addEdge("Mardan", "Mohmand");
    graph.addEdge("Mardan", "Charsadda");
    graph.addEdge("Mardan", "Nowshera");

    graph.addEdge("Mohmand", "Malakand");
    graph.addEdge("Mohmand", "Mardan");
    graph.addEdge("Mohmand", "Peshawar");

    graph.addEdge("Malakand", "Dir");
    graph.addEdge("Malakand", "Swat");
    graph.addEdge("Malakand", "Mohmand");
    graph.addEdge("Malakand", "Mardan");

    graph.addEdge("Dir", "Malakand");

    graph.addEdge("Swat", "Malakand");


    string message = "\n      ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * \n      ' ` . * ' . * , ` * ' ` . * ' . * WELCOME TO FAST EXPLORER! * ` * ' ` . * ' . * , ` * ' ` . * ' ` * \n      ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * ' ` . * ' . * , ` * \n";
    slowPrint(message, 10); 
    cout << endl << endl;


    system("Color 07");
    cout << "\n   ==========================================================================================================" << endl;
    cout << "   ||                                                                                                      ||" << endl;
    cout << "   ||                               Welcome to the Fast Explorer System!                                   ||" << endl;
    cout << "   ||                                                                                                      ||" << endl;
    cout << "   ||   This system allows you to explore and analyze various aspects of maps in Pakistan, including:      ||" << endl;
    cout << "   ||    - Displaying cities and their geographical coordinates                                            ||" << endl;
    cout << "   ||    - Exploring cities and their neighboring cities                                                   ||" << endl;
    cout << "   ||    - Calculating the shortest path between 2 selected cities                                         ||" << endl;
    cout << "   ||                                                                                                      ||" << endl;
    cout << "   ||   Our goal:                                                                                          ||" << endl;
    cout << "   ||   To contribute to a more open and democratic information landscape by:                              ||" << endl;
    cout << "   ||    - Increasing transparency in mapping platforms                                                    ||" << endl;
    cout << "   ||    - Challenging monopolies and promoting inclusivity                                                ||" << endl;
    cout << "   ||    - Providing cost-effective, optimized routes based on passenger and vehicle type                  ||" << endl;
    cout << "   ||                                                                                                      ||" << endl;
    cout << "   ||                   Thank you for using Fast Explorer. Let's begin your journey! :)                    ||" << endl;
    cout << "   ||                                                                                                      ||" << endl;
    cout << "   ==========================================================================================================" << endl;

    cout << "\033[32m" << "                                          Press any key to proceed!" << endl;
    char c = getch();
    system("cls");


    while (true) {
        cout << endl;
        cout << "   -----------------------------------------------------------------------------------------" << endl;
        cout << "   |       Please select one of the following options to proceed with your exploration:    |" << endl;
        cout << "   |                                                                                       |" << endl;
        cout << "   |            1. Display cities and their geographical coordinates                       |" << endl;
        cout << "   |            2. Display cities with their neighboring cities                            |" << endl;
        cout << "   |            3. Calculate path between source and destination                           |" << endl;
        cout << "   |            4. Exit                                                                    |" << endl;
        cout << "   -----------------------------------------------------------------------------------------" << endl;
        int choice;
        cout << "\n\tChoice Entered : ";
        cin >> choice;
        system("cls");


        switch (choice) {

            case 1: {
                system("Color 05");
                cout << "CITIES IN PAKISTAN" << endl << endl;
                ll.displayCities();
                cout << endl << endl;
                break;
            }

            case 2: {
                system("cls");
                system("Color 03");
                cout << "CITIES AND THEIR NEIGHBORING CITIES IN PAKISTAN" << endl << endl;
                graph.displayAdjacencyList();
                cout << endl << endl;
                break;
            }

            case 3: {
                /////  passenger  /////
                system("cls");
                int ID = 1;
                string name;
                char passenger_type;
                double distance;

                system("Color 05");
                cout << "PASSENGER INFORMATION" << endl << endl;
                cout << "Enter passenger name = ";;
                cin >> name;

                bool correct_passenger = false;

                cout << "\nChoose passenger category " << endl;
                cout << " \n Student = S \n Adult = A \n Elderly = E" << endl;
                while (!correct_passenger)
                {
                    cin >> passenger_type;


                    if (passenger_type != 'S' && passenger_type != 's' &&
                        passenger_type != 'A' && passenger_type != 'a' &&
                        passenger_type != 'E' && passenger_type != 'e') 
                    {
                        correct_passenger = false;
                        cout << "Unexpected input, try again: " << endl;
                    }
                    else 
                    {
                        correct_passenger = true;
                    }
                }

                cout << endl << "Display Passenger Details" << endl << endl;

                Passenger* p = NULL;

                if (passenger_type == 'A' || passenger_type == 'a') {
                    p = new Adult(ID, name, passenger_type);
                }
                else if (passenger_type == 'S' || passenger_type == 's') {
                    p = new Student(ID, name, passenger_type);
                }
                else if (passenger_type == 'E' || passenger_type == 'e') {
                    p = new Elderly(ID, name, passenger_type);
                }

                p->display_details();
                cout << endl << "=================================" << endl << endl;

                
                /////  pathfinding  /////
                string source;
                string destination;

                cout << "SHORTEST PATH BETWEEN CITIES" << endl << endl;
                cout << "Enter source city name: ";
                cin >> source;
                Vertex* src = graph.getVertex(source);

                cout << "Enter destination city name: ";
                cin >> destination;
                Vertex* dest = graph.getVertex(destination);

                // using dijkstra's algo to compute the shortest path (+ distance) for each vertex in the graph w.r.t. the src vertex
                vector<Vertex*> shortestPath = Dijkstra::getShortestPath(src, dest);

                cout << endl;
                // total (cumulative) distance to destination city will be stored in the shortestDistance in destination vector
                cout << "Shortest distance from " << source << " to " << destination << ": " << dest->shortestDistance << " km." << endl;

                // load shortest distance locally for calculation
                distance = dest->shortestDistance;

                cout << endl;
                // shortest path in the form of a chain (linked list) will be stored in each vector
                cout << "Shortest path from " << source << " to " << destination << ": " << endl;

                // outputting:
                // city | cumulative distance to the city from the source city
                for (vector<Vertex*>::iterator it = shortestPath.begin(); it != shortestPath.end(); it++) {
                    cout << (*it)->city << " | " << (*it)->shortestDistance << '\n';
                }
                
                cout << endl << "=================================" << endl << endl;


                ///// vehicle ///// 
                int vehicle_choice;
                bool correct_vehicle = false;
                Vehicle* v = nullptr;

                cout << "VEHICLE SELECTION" << endl << endl;
                while (!correct_vehicle)
                {
                    cout << "Choose vehicle type" << endl;
                    cout << " 1. Mini \n 2. Standard \n 3. Luxury" << endl;
                    cin >> vehicle_choice;
                    cout << endl;

                    if (vehicle_choice == 1) {
                        v = new Mini();
                        correct_vehicle = true;
                    }
                    else if (vehicle_choice == 2) {
                        v = new Standard();
                        correct_vehicle = true;
                    }
                    else if (vehicle_choice == 3) {
                        v = new Luxury();
                        correct_vehicle = true;
                    }
                    else 
                    {
                        correct_vehicle = false;
                        system("cls");
                        cout << "Unexpected input, try again: " << endl;
                    }
                }
        
                p->cost_cal(distance, v->get_cost_per_KM());

                cout << endl << "=================================" << endl;
                break;
            }

            case 4: {
                system("cls");
                cout << endl << endl << endl;
                system("color 05");
                cout << "\t\tThank you for using Fast Explorer!" << endl << endl;
                cout << "\t\t\tBYE BYE BYE! :)";
                cout << endl << endl << endl << endl;
                return 0;
            }

            default: {
                cout << "Invalid input. Try Again!" << endl;
                break;
            }

        }
    }

    return 0;
}
