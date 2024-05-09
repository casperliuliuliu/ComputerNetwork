#include <iostream>
#include <chrono>
#include <ctime>    
using namespace std;

int main()
{
    chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
    // Some computation here
    int jj=0;
    for (int ii = 0; ii < 1000000; ii++){jj = ii;}
    chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
 
    chrono::duration<double> elapsed_seconds = end-start;
    time_t start_time = chrono::system_clock::to_time_t(start);
    time_t end_time = chrono::system_clock::to_time_t(end);
 
    cout << "finished computation at " << ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s" << endl
              << "start: " << &start_time << endl
              << "ended: " << &end_time << endl;
}