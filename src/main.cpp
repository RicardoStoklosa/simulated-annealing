#include <bits/stdc++.h>

#include "omp.h"

using namespace std;

#define MAX_SIZE 101
double originalGraphMatrix[MAX_SIZE][MAX_SIZE];

vector<int> cities;
vector<int> finalSolution;

typedef struct {
    int label;
    int x;
    int y;
} Point;

class TSPInstance {
   public:
    int numCities;
    Point points[MAX_SIZE];

    void inputData() {
        scanf("%d", &numCities);
        for (int i = 0; i < numCities; i++) {
            scanf("%d %d %d", &points[i].label, &points[i].x, &points[i].y);
        }
    }

    double euclidianDistance(Point p1, Point p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    void calculateOriginalGraph() {
        for (int i = 0; i < numCities; i++) {
            for (int j = i; j < numCities; j++) {
                if (i == j)
                    originalGraphMatrix[i][i] = 0;
                else {
                    originalGraphMatrix[i][j] =
                        euclidianDistance(points[i], points[j]);
                    originalGraphMatrix[j][i] =
                        originalGraphMatrix[i][j];  // TSP simetrico
                }
            }
        }
        // for (int i = 0; i < numCities; i++) {
        //     for (int j = 0; j < numCities; j++) {
        //         printf("[%.2f]", originalGraphMatrix[i][j]);
        //     }
        //     printf("\n");
        // }
    }
};

double distance(int city1, int city2) {
    return originalGraphMatrix[city1][city2];
}

double getProbability(int difference, double temperature) {
    return exp(-1 * difference / temperature);
}

unsigned getSeed() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}

double getRandomNumber(double i, double j) {
    default_random_engine generator(getSeed());
    uniform_real_distribution<double> distribution(i, j);
    return double(distribution(generator));
}

// void swap_elem(int i, int j) {
//     int temp = arr[i];
//     arr[i] = arr[j];
//     arr[j] = temp;
// }

void swap_it(int i, int j) {
    vector<int>::iterator it = cities.begin();
    int temp = *(it + i);
    *(it + i) = *(it + j);
    *(it + j) = temp;
}

double tourLength() {
    vector<int>::iterator it = cities.begin();
    int atual_city = *it, next_city;
    double tourLength = distance(0, atual_city);
    for (it = cities.begin() + 1; it != cities.end(); it++) {
        next_city = *it;
        tourLength += distance(atual_city, next_city);
        atual_city = next_city;
    }
    tourLength += distance(atual_city, 0);
    return tourLength;
}

double getRandomNeighbourTour(int numCities) {
    vector<int>::iterator it;
    int order[numCities];
    for (int i = 0; i < numCities; i++) order[i] = i;
    shuffle(order + 1, order + numCities - 1, default_random_engine(getSeed()));
    cities.clear();
    for (int i = 0; i < numCities - 1; i++) {
        cities.push_back(order[i]);
        finalSolution.push_back(order[i]);
    }
    double solution = tourLength();
    return solution;
}

int main() {
    vector<int>::iterator it, it2;
    TSPInstance testIntance;
    testIntance.inputData();
    int numCities = testIntance.numCities;
    printf("Finish read %d points\n", numCities);
    testIntance.calculateOriginalGraph();
    printf("Finish calculate graph\n");

    double bestTourLength = getRandomNeighbourTour(numCities);
    printf("initial solution: %.2f\n", bestTourLength);
    double temperature;
    double coolingRate = 0.9;
    double absoluteTemperature = 0.00001;
    double probability;
    double newTourLength, difference;

    std::fstream fs;
    double mini = DBL_MAX;
    fs.open("tspResults.txt", std::fstream::in | std::fstream::out);
    for (int test = 0; test < 100; test++) {
        temperature = DBL_MAX;
        fs << "Test " << test << " | Initial solution : " << bestTourLength
           << std::endl
           << "[";
        printf("Started Test: %d\n", test);
        int p1, p2;
        while (temperature > absoluteTemperature) {
            p1 = int(getRandomNumber(0, numCities - 1));
            p2 = int(getRandomNumber(0, numCities - 1));
            while (p1 == p2 or ((p1 > numCities - 2) or (p2 > numCities - 2))) {
                p1 = int(getRandomNumber(0, numCities - 2));
                p2 = int(getRandomNumber(0, numCities - 2));
            }
            swap_it(p1, p2);
            it2 = cities.begin();
            if (p2 > p1) random_shuffle(it2 + p1, it2 + p2);
            newTourLength = tourLength();
            mini = min(mini, newTourLength);
            fs << newTourLength << ",";
            difference = newTourLength - bestTourLength;

            if (difference < 0 or
                (difference > 0 and getProbability(difference, temperature) >
                                        getRandomNumber(0, 1))) {
                finalSolution.clear();
                for (it = cities.begin(); it != cities.end(); it++) {
                    finalSolution.push_back(*it);
                }
                bestTourLength = difference + bestTourLength;
            }
            temperature = temperature * coolingRate;
        }
        fs << "]\n";
        random_shuffle(cities.begin(), cities.end());
    }
    fs.close();
    cout << "the best solution is " << bestTourLength << "\n";
    cout << " the minimum solution found is  " << mini << "\n";
}