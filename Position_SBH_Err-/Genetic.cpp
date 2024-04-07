#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std;

#define CROSSES 2500
#define POPULATION 5000
#define BEST 5000
#define STABLE 400
#define ITERATIONS1 3000
#define ITERATIONS2 1000

vector<int> visited1;
vector<int> visited2;
vector<int> child1;
vector<int> child2;

struct sequence
{
    string chain;
    int low, high;
};

string start;
int length;
int amount;
vector<sequence> probes;
vector<int> permutation;
int permutationSize;

class Solution
{
public:
    vector<int> points;
    double sum;
    Solution()
    {
        this->points = vector<int>();
        this->sum = 0;
    }
    // default constructor is necessary for resize to work
    Solution(vector<int> points, double sum)
    {
        this->points = points;
        this->sum = sum;
    }
    void display()
    {
        cout << sum << endl;
        for (int i = 0; i < permutationSize; i++)
        {
            // cout << i + 1 << " " << probes[points[i]].low << " " << probes[points[i]].high << endl;
        }
    }
    void CalcValue()
    {
        this->sum = 0.0;
        for (int i = 0; i < permutationSize; i++)
        {
            int position = i + 1;
            this->sum += max(0, probes[points[i]].low - position);
            this->sum += max(0, position - probes[points[i]].high);
        }
    }
    bool operator<(const Solution &a) const
    {
        return sum < a.sum;
    }
    bool operator==(const Solution &a) const
    {
        if (sum != a.sum)
            return false;
        else
        {
            for (int i = 0; i < (int)this->points.size(); i++)
            {
                if (points[i] != a.points[i])
                    return false;
            }
        }
        return true;
    }
};

vector<Solution> solutions;
queue<Solution> best_of_generation;
vector<Solution> extinct;

void random()
{
    for (int i = 0; i < POPULATION; i++)
    {
        random_shuffle(permutation.begin(), permutation.end()); // tasuje wierzcholki
        Solution randomized(permutation, 0);
        randomized.CalcValue();
        solutions.push_back(randomized);
    }
}

void prepareCross()
{
    for (int j = 0; j < permutationSize; j++)
    {
        visited1.push_back(0);
        child1.push_back(0);
        visited2.push_back(0);
        child2.push_back(0);
    }
}

void Cross()
{
    for (int i = 0; i < CROSSES; i++)
    {
        // Select parents
        int parent1 = rand() % BEST;
        int parent2 = rand() % POPULATION;
        for (int j = 0; j < permutationSize; j++)
        {
            visited1[j] = 0;
            visited2[j] = 0;
        }
        // Select 2 points
        int a = rand() % permutationSize;
        int b = rand() % permutationSize;
        int index = a;
        while (index != b)
        {
            // Mark visited sequences for both children
            visited1[solutions[parent1].points[index]] = 1;
            visited2[solutions[parent2].points[index]] = 1;
            // Duplicate a part of a parent
            child1[index] = solutions[parent1].points[index];
            child2[index] = solutions[parent2].points[index];
            // Move forward
            index = (index + 1) % permutationSize;
        }
        visited1[solutions[parent1].points[index]] = 1;
        visited2[solutions[parent2].points[index]] = 1;
        // Duplicate a part of a parent
        child1[index] = solutions[parent1].points[index];
        child2[index] = solutions[parent2].points[index];
        // Move forward
        index = (index + 1) % permutationSize;
        int index1 = index;
        int index2 = index;
        // Add unused points
        for (int j = 0; j < permutationSize; j++)
        {
            if (visited1[solutions[parent2].points[index]] == 0)
            {
                child1[index1] = solutions[parent2].points[index];
                index1 = (index1 + 1) % permutationSize;
            }
            if (visited2[solutions[parent1].points[index]] == 0)
            {
                child2[index2] = solutions[parent1].points[index];
                index2 = (index2 + 1) % permutationSize;
            }
            // Move forward
            index = (index + 1) % permutationSize;
        }
        if (child1.size() == 0)
            return;
        if (child2.size() == 0)
            return;
        Solution crossed1(child1, 0);
        Solution crossed2(child2, 0);
        crossed1.CalcValue();
        crossed2.CalcValue();
        solutions.push_back(crossed1);
        solutions.push_back(crossed2);
    }
}

void mutation(int which)
{
    int option;
    int a, b;
    vector<int> change(0);
    option = rand() % 100;
    option += 1000;
    if (option < 33) // Swap 2 points
    {
        a = rand() % permutationSize;
        b = rand() % permutationSize;
        swap(solutions[which].points[a], solutions[which].points[b]);
    }
    else if (option < 66) // Move one point into an entirely new place
    {
        a = rand() % permutationSize;
        b = rand() % permutationSize;
        change.push_back(solutions[which].points[a]);
        solutions[which].points.erase(solutions[which].points.begin() + a);
        solutions[which].points.insert(solutions[which].points.begin() + b, change[0]);
    }
    else if (option < 100) // Place a chunk of a solution into a different place
    {
        a = rand() % permutationSize;
        b = rand() % permutationSize;
        if (b < a)
            swap(a, b);
        for (int i = a; i < b; i++)
            change.push_back(solutions[which].points[i]);
        solutions[which].points.erase(solutions[which].points.begin() + a, solutions[which].points.begin() + b);
        a = rand() % solutions[which].points.size();
        solutions[which].points.insert(solutions[which].points.begin() + a, change.begin(), change.end());
    }
    solutions[which].CalcValue();
}

void massExtinction()
{
    vector<int> tmp;
    Solution help(tmp, 0);
    best_of_generation.push(solutions[0]);
    if (best_of_generation.size() == STABLE)
    {
        if (abs(solutions[0].sum - best_of_generation.front().sum) < 40)
        {
            extinct.push_back(solutions[0]);
            solutions.clear();
            random();
            best_of_generation = queue<Solution>(); // clears the queue
        }
        else
        {
            best_of_generation.pop();
        }
    }
}

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    ifstream inputFile("output.txt");
    if (!inputFile.is_open())
    {
        cout << "Failed to open the file." << endl;
        return 1;
    }

    // Read length, start, and amount from the file
    inputFile >> length;
    inputFile >> start;
    inputFile >> amount;

    sequence curr;
    for (int i = 0; i < amount; i++)
    {
        inputFile >> curr.chain >> curr.low >> curr.high;
        probes.push_back(curr);
    }
    inputFile.close();

    sequence dummy;
    dummy.chain = "";
    for (int i = 0; i < start.size(); i++)
        dummy.chain += "X";
    dummy.low = -1;
    dummy.high = length + 1;
    // Add dummies
    for (int i = amount; i < length - dummy.chain.size(); i++)
        probes.push_back(dummy);

    srand(time(0));

    for (int i = 0; i < probes.size(); i++)
        permutation.push_back(i);

    permutationSize = probes.size();
    random();
    prepareCross();

    sort(solutions.begin(), solutions.end());

    for (int k = 0; k < ITERATIONS1; k++)
    {
        if (k % 1 == 0)
        {
            cout << k << endl;
            solutions[0].display();
        }
        for (int i = solutions.size() - 1; i >= 1; i--)
        {
            if (solutions[i - 1].sum == solutions[i].sum)
            {
                mutation(i);
            }
        }
        Cross();
        sort(solutions.begin(), solutions.end());
        solutions.erase(solutions.begin() + POPULATION, solutions.end());
        massExtinction();
    }
    cout << "Extinct:" << endl; // adding best results to population
    for (int i = 0; i < (int)extinct.size(); i++)
    {
        extinct[i].display();
        for (int j = 0; j < 50; j++)
            solutions.push_back(extinct[i]);
    }
    sort(solutions.begin(), solutions.end());
    solutions.erase(solutions.begin() + POPULATION, solutions.end());

    for (int k = 0; k < ITERATIONS2; k++)
    {
        if (k % 100 == 0)
        {
            cout << k << endl;
            solutions[0].display();
        }
        for (int i = solutions.size() - 1; i >= 1; i--)
        {
            if (solutions[i - 1].sum == solutions[i].sum)
                mutation(i);
        }
        Cross();
        sort(solutions.begin(), solutions.end());
        solutions.erase(solutions.begin() + POPULATION, solutions.end());
    }

    cout << "Final:" << endl;
    cout << solutions[0].sum << endl;
    for (int i = 0; i < permutationSize; i++)
        cout << solutions[0].points[i] << " - ";
    cout << endl;
}
