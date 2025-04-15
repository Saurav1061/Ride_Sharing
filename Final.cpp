#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

class Node {
public:
    string name;
    vector<pair<Node*, double>> neighbors;
    double lat, lon;

    Node(const string& name, double lat = 0, double lon = 0) : name(name), lat(lat), lon(lon) {}
};

class Graph {
public:
    unordered_map<string, Node*> nodes;

    struct RideMetrics {
        double distance;
        double time;
        double price;
    };

    // Calculate metrics for a specific path segment
    RideMetrics calculateSegmentMetrics(const vector<string>& path) const {
    RideMetrics metrics;
    
    // If there's only one entry in the path, set distance to 0
    if (path.size() <= 1) {
        metrics.distance = 0.0;
    } else {
        metrics.distance = calculatePathDistance(path);
    }
    
    metrics.time = metrics.distance / 50.0;
    metrics.price = metrics.distance * 10.0;

    return metrics;
}


    Graph() {}
    ~Graph() {
        for (auto& entry : nodes) {
            delete entry.second;
        }
    }

    void addEdge(const string& node1, const string& node2, double distance) {
        if (nodes.find(node1) == nodes.end()) {
            nodes[node1] = new Node(node1);
        }
        if (nodes.find(node2) == nodes.end()) {
            nodes[node2] = new Node(node2);
        }
        nodes[node1]->neighbors.emplace_back(nodes[node2], distance);
        nodes[node2]->neighbors.emplace_back(nodes[node1], distance);
    }

    void loadGraphFromFile(const string& filePath) {
        for (auto& entry : nodes) {
            delete entry.second;
        }
        nodes.clear();

        ifstream file(filePath);
        if (!file) {
            cerr << "Error: Could not open the file." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.find_first_not_of(" \t\n\v\f\r") == string::npos) {
                continue;
            }

            stringstream ss(line);
            string node1, node2;
            double distance;

            if (!(ss >> node1 >> node2 >> distance)) {
                continue;
            }

            addEdge(node1, node2, distance);
        }
        file.close();
        cout << "Graph updated from file." << endl;
    }

    void displayGraph() const {
        for (const auto& entry : nodes) {
            const string& city = entry.first;
            const auto& neighbors = entry.second->neighbors;
            cout << city << " -> ";
            for (const auto& neighborEntry : neighbors) {
                cout << "(" << neighborEntry.first->name << ", " << neighborEntry.second << ") ";
            }
            cout << endl;
        }
    }

    double heuristic(const Node* start, const Node* goal) const {
        double lat1 = start->lat * M_PI / 180.0;
        double lon1 = start->lon * M_PI / 180.0;
        double lat2 = goal->lat * M_PI / 180.0;
        double lon2 = goal->lon * M_PI / 180.0;

        double dlat = lat2 - lat1;
        double dlon = lon2 - lon1;
        double a = sin(dlat / 2) * sin(dlat / 2) +
                   cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return 6371.0 * c;
    }

    vector<string> aStarShortestPath(const string& startNode, const string& endNode) const {
        if (nodes.find(startNode) == nodes.end() || nodes.find(endNode) == nodes.end()) {
            return {};
        }

        unordered_map<string, double> dist;
        unordered_map<string, string> cameFrom;
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<>> openSet;

        for (const auto& entry : nodes) {
            dist[entry.first] = numeric_limits<double>::infinity();
        }
        dist[startNode] = 0.0;
        openSet.push({0.0, startNode});

        while (!openSet.empty()) {
            string current = openSet.top().second;
            openSet.pop();

            if (current == endNode) {
                return reconstructPath(cameFrom, startNode, endNode);
            }

            for (const auto& neighbor : nodes.at(current)->neighbors) {
                const string& neighborNode = neighbor.first->name;
                double weight = neighbor.second;
                double tentativeDist = dist[current] + weight + heuristic(nodes.at(current), neighbor.first);

                if (tentativeDist < dist[neighborNode]) {
                    dist[neighborNode] = tentativeDist;
                    cameFrom[neighborNode] = current;
                    openSet.push({tentativeDist, neighborNode});
                }
            }
        }
        return {};
    }

    vector<string> reconstructPath(const unordered_map<string, string>& cameFrom,
                                 const string& start, const string& end) const {
        vector<string> path;
        string current = end;
        while (current != start) {
            path.push_back(current);
            current = cameFrom.at(current);
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        return path;
    }

    double calculatePathDistance(const vector<string>& path) const {
        double totalDistance = 0.0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            for (const auto& neighbor : nodes.at(path[i])->neighbors) {
                if (neighbor.first->name == path[i + 1]) {
                    totalDistance += neighbor.second;
                    break;
                }
            }
        }
        return totalDistance;
    }

    // Calculate metrics for individual rides
    RideMetrics calculateIndividualRideMetrics(const string& start, const string& end) {
        vector<string> path = aStarShortestPath(start, end);
        return calculateSegmentMetrics(path);
    }

    // Calculate shared ride metrics for both users
    pair<RideMetrics, RideMetrics> calculateSharedRideMetrics(
        const string& user1Start, const string& user1End,
        const string& user2Start, const string& user2End) {
        
        RideMetrics user1Metrics = {0.0, 0.0, 0.0};
        RideMetrics user2Metrics = {0.0, 0.0, 0.0};

        vector<string> user1InitialPath;
        vector<string> sharedPath;
        vector<string> user2FinalPath;

        // Calculate path segments

        user1InitialPath = aStarShortestPath(user1Start, user2Start);
        sharedPath = aStarShortestPath(user2Start, user1End);
        user2FinalPath = aStarShortestPath(user1End, user2End);

        // Calculate metrics for each segment
        RideMetrics initialSegment = calculateSegmentMetrics(user1InitialPath);
        RideMetrics sharedSegment = calculateSegmentMetrics(sharedPath);
        RideMetrics finalSegment = calculateSegmentMetrics(user2FinalPath);

        // Calculate User 1's metrics (initial solo + shared segment)
        user1Metrics.distance = initialSegment.distance + sharedSegment.distance;
        user1Metrics.time = initialSegment.time + sharedSegment.time;
        user1Metrics.price = initialSegment.price + (sharedSegment.price / 2.0);

        // Calculate User 2's metrics (shared + final solo segment)
        user2Metrics.distance = sharedSegment.distance + finalSegment.distance;
        user2Metrics.time = sharedSegment.time + finalSegment.time;
        user2Metrics.price = (sharedSegment.price / 2.0) + finalSegment.price;

        return {user1Metrics, user2Metrics};
    }
};
int main() {
    string filePath = "cities.txt";
    Graph graph;
    graph.loadGraphFromFile(filePath);

    string user1Origin, user1Dest, user2Origin, user2Dest;

    cout << "Enter User 1 Origin: ";
    cin >> user1Origin;
    cout << "Enter User 1 Destination: ";
    cin >> user1Dest;

    cout << "Enter User 2 Origin: ";
    cin >> user2Origin;
    cout << "Enter User 2 Destination: ";
    cin >> user2Dest;

    // Calculate individual ride metrics
    auto user1Individual = graph.calculateIndividualRideMetrics(user1Origin, user1Dest);
    auto user2Individual = graph.calculateIndividualRideMetrics(user2Origin, user2Dest);

    // Calculate shared ride metrics
    pair<Graph::RideMetrics, Graph::RideMetrics> sharedMetrics = graph.calculateSharedRideMetrics(
        user1Origin, user1Dest, user2Origin, user2Dest);

    // Manually unpack the pair into individual variables
    Graph::RideMetrics user1Shared = sharedMetrics.first;
    Graph::RideMetrics user2Shared = sharedMetrics.second;

    // Display paths before decision
    cout << "\nPaths before decision to share:" << endl;

    // Individual paths
    cout << "\nUser 1 Solo Path: ";
    vector<string> user1Path = graph.aStarShortestPath(user1Origin, user1Dest);
    for (const auto& city : user1Path) {
        cout << city << " -> ";
    }
    cout << "END" << endl;

    cout << "User 2 Solo Path: ";
    vector<string> user2Path = graph.aStarShortestPath(user2Origin, user2Dest);
    for (const auto& city : user2Path) {
        cout << city << " -> ";
    }
    cout << "END" << endl;

    // Shared paths
    vector<string> initialPath = graph.aStarShortestPath(user1Origin, user2Origin);
    vector<string> sharedPath = graph.aStarShortestPath(user2Origin, user1Dest);
    vector<string> finalPath = graph.aStarShortestPath(user1Dest, user2Dest);

    cout << "\nUser 1 solo: ";
    for (const auto& city : initialPath) {
        cout << city << " -> ";
    }
    cout << "\nShared segment: ";
    for (const auto& city : sharedPath) {
        cout << city << " -> ";
    }
    cout << "\nUser 2 solo: ";
    for (const auto& city : finalPath) {
        cout << city << " -> ";
    }
    cout << "END" << endl;

    // Display ride comparison before decision
    cout << "\nRide Stats Comparison:" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "| Ride Type       | Distance (km) | Time (hours) | Price (units) |" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "| User 1 Solo     | " << fixed << setprecision(2) 
         << user1Individual.distance << "          | " 
         << user1Individual.time << "          | " 
         << user1Individual.price << "    |" << endl;
    cout << "| User 2 Solo     | " << user2Individual.distance << "          | " 
         << user2Individual.time << "          | " 
         << user2Individual.price << "    |" << endl;
    cout << "| User 1 Shared   | " << user1Shared.distance << "          | " 
         << user1Shared.time << "          | " 
         << user1Shared.price << "    |" << endl;
    cout << "| User 2 Shared   | " << user2Shared.distance << "          | " 
         << user2Shared.time << "          | " 
         << user2Shared.price << "    |" << endl;
    cout << "------------------------------------------------------------------" << endl;

    // Prompt user for decision
    cout << "The final price for User 1: " << user1Shared.price - user1Shared.price * 0.3 << " units" << endl;
    cout << "The final price for User 2: " << user2Shared.price - user2Shared.price * 0.3 << " units" << endl;
    cout << "\nDo you want to share a ride? (yes/no): "<<endl;
    string decision;
    cin >> decision;

    if (decision == "yes") {
        cout << "\nCongratulations! You've decided to share the ride!" << endl;

        // Final prices
        cout << "\nFinal prices:" << endl;
        cout <<endl;
        cout <<"If you decide to share a ride" << endl;
        cout << "User 1 Discounted Price: " << user1Shared.price - user1Shared.price * 0.3 << " units" << endl;
        cout << "User 2 Discounted Price " << user2Shared.price - user2Shared.price * 0.3 << " units" << endl;
    } else {
        cout << "\nIndividual ride prices will be:" << endl;
        cout << "User 1: " << user1Individual.price << " units" << endl;
        cout << "User 2: " << user2Individual.price << " units" << endl;
    }

    return 0;
    
}