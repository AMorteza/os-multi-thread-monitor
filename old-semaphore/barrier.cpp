
#include <iostream>
#include <fstream>
#include <thread> 
#include <mutex>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>

using namespace std;

ifstream infile("inputs.txt");
mutex mtx;

sem_t b_read;
sem_t b_read_2;
sem_t b_read_mutex;

sem_t b_write;
sem_t b_write_2;
sem_t b_write_mutex;

const int NUM_THREAD = 10;
int numbers[20][10] = {-10000};
int counter = 0;
unsigned int microseconds;

template<typename Out>
void split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

void pause_thread(int n) 
{
  this_thread::sleep_for (chrono::seconds(n));
  cout << "pause of " << n << " seconds ended\n";
}

void do_task(int th_id, int step, vector<vector<string>> v){
        
        // read
        mtx.lock();
        counter++;
        string s = v[th_id][step];
        numbers[step][th_id] = stoi(s);
        mtx.unlock();

        if (counter != NUM_THREAD - 1)
            sem_wait(&b_read);
        sem_post(&b_read);
          
        mtx.lock();
        counter--;
        mtx.unlock();
        if (counter != 0)
            sem_wait(&b_read);
        sem_post(&b_read);

        int max_t = -10000;
        mtx.lock();
        counter++;

        for (int i = 0; i < 10; ++i)
        {
            if (numbers[step][i] > max_t)
            {
                max_t = numbers[step][i];
            }
        }
        mtx.unlock();    
        
        if (counter != NUM_THREAD)
            sem_wait(&b_read);
        sem_post(&b_read);

        mtx.lock();
        counter--;
        mtx.unlock();
        if (counter != 0)
            sem_wait(&b_read_2);
        sem_post(&b_read_2);
        
        // write
        mtx.lock();
        counter++;
        mtx.unlock();
        
        string tmp_string = to_string(th_id) + ".dat"; 
        ofstream outFile(tmp_string.c_str(),  ios::out | ios::app);
        double normalized = ((double)stoi(s)/max_t);  
        outFile << s << " / " << max_t << " = "<< normalized << endl;

        if (counter != NUM_THREAD)
            sem_wait(&b_write);
        sem_post(&b_write);

        mtx.lock();
        counter--;
        mtx.unlock();
        if (counter != 0)
            sem_wait(&b_write_2);
        sem_post(&b_write_2);

        //sleep
        usleep(microseconds);
}

int main()
{
    sem_init(&b_read, 0, 0);
    sem_init(&b_read_2, 0, 0);

    sem_init(&b_write, 0, 0);
    sem_init(&b_write_2, 0, 0);
    
    if (infile.good())
    {
        vector<vector<string>> inputs;  
        string line;
    
        while (getline(infile, line))
        {
            vector<string> items;
            items = split(line, ',');
            inputs.push_back(items);
        }

        for (int j = 0; j < 20; ++j)
        {
            thread threads[NUM_THREAD];
            for (int i = 0; i < NUM_THREAD; i++)
                threads[i] = thread(do_task, i, j, inputs);
        
            for (int i = 0; i < NUM_THREAD; i++)
                threads[i].join();
        }
           
    }else
        cout << "inputs.txt file not exists!" << endl;
    return 0;
}