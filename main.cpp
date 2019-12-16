// Ensure that only one thread at a time can run a method of my class.
// In other words, make the class behave like a Monitor.

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <thread> 
#include <mutex>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unistd.h>
#include <chrono>
#include <semaphore.h>
#include <map> 
#include <ctime>

using namespace std;

int total_emission = 0;
int counter = 0;
mutex mtx;
sem_t total_em;
sem_t total_em_2;

class Monitor {
  
	public: string from_vertice;
	public: string to_vertice;
	public: int h;

	public: Monitor(string from_vertice, string to_vertice, int h) 
    { 
        this->from_vertice = from_vertice; 
        this->to_vertice = to_vertice;
        this->h = h;
    }

	public: void enter(){
        // cv.wait(lck);
        // cv.notify_one();
	}

	public: int emission(int p)
	{
		int sum = 0;
		for (int k = 0; k <= 10000000; ++k){
			int term = (p * this->h * 1000000); 
			if (k >= term and term != 0)
				sum += k / term;
		}
		return sum;	
	}
};

string epoch_time()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	string str(buffer);
	return str;
}

template<typename Out>
void split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

int strtoi(string item)
{
    int d;
    istringstream(item) >> d;
    return d;
}

string itos(int number)
{
    if (number == 0)
    {
        return "0";
    }
    string snum;
    while(number > 0){
        snum = char(number%10 + '0') + snum;
        number = number/10;
    }
    return snum;
}

int get_rand(int lowest=1, int highest=10)
{
	srand((int)time(0));
	int range=(highest-lowest)+1;
	return rand() % range + 1;
}

void pass(std::vector<Monitor*> edges, int p, int path_num, int car_num, int cars_num)
{
    mtx.lock();	
	string entry_time = epoch_time();
	string exit_time;
	int path_emission = 0;
	for (int i = 0; i < edges.size(); ++i)
	{
		// edges[i]->enter();
		path_emission += edges[i]->emission(p);
	}

	exit_time = epoch_time();
    total_emission += path_emission;
    counter++;
    mtx.unlock();

    if (counter < cars_num)
    	sem_wait(&total_em);
    sem_post(&total_em);

    mtx.lock();    
    counter--;	
    mtx.unlock();

    if (counter > 0)
    	sem_wait(&total_em_2);
    sem_post(&total_em_2);

    mtx.lock();    
	ofstream outFile(itos(path_num) + "-" + itos(car_num), ios::out);
    outFile << edges[0]->from_vertice << ", "
    << entry_time << ", "
    << edges[edges.size() - 1]->to_vertice  << ", "
    << exit_time << ", "
    << path_emission << ", "
    << total_emission << endl;
    outFile.close();
    mtx.unlock();
}

int main()
{
	ifstream infile("input");
    sem_init(&total_em, 0, 0);
    sem_init(&total_em_2, 0, 0);

	if (infile.good())
    {
        vector<vector<string>> inputs;  
        string line;
        int is_next_input = 0;
        int is_second_line = 0;    
        vector<string> path;
    	map<vector<string>, int> dict;
		vector<Monitor *> monitors;    	
    	int path_counter = 0;
        while (getline(infile, line))
        {
            vector<string> items;
			split(line, '-', back_inserter(items));
            if (strcmp(line.c_str(), "#") != 0 and is_next_input == 0)
            {
    			vector<string> v;
    			v.push_back(items[0]);
    			v.push_back(items[1]);
    			dict.insert(pair<vector<string>, int>(v, strtoi(items[2])));
    			monitors.push_back(new Monitor(items[0], items[1], strtoi(items[2])));
            }else{
            	if (is_next_input == 1)
            	{
            		if (is_second_line == 0)
            		{
            			is_second_line = 1;
            			path = items;
            			path_counter++;
            		}else{
            			int car_counter = 0;
            			is_second_line = 0;
            			int cars_num = strtoi(items[0]);
            			thread threads[cars_num];
            			for (int i = 0; i < cars_num; ++i)
            			{
            				int car_rand = (get_rand() * car_counter) % 10 + 1;
            				std::vector<Monitor *> car_path;
            				int is_first_node = 0;
            				string prev;
            				for (string node : path)
            				{
            					if (is_first_node == 0)
            					{
            						prev = node;
            						is_first_node = 1;
            					}else{
            						vector<string> v;
    								v.push_back(prev);
    								v.push_back(node);
            						int h = dict[v];
            						for (int k = 0; k < monitors.size(); ++k)
            						{
            							if (strcmp(monitors[k]->from_vertice.c_str(), prev.c_str()) == 0 
        								and strcmp(monitors[k]->to_vertice.c_str(), node.c_str()) == 0
        								and monitors[k]->h == h)
            							{
            								car_path.push_back(monitors[k]);
            								break;
            							}
            						}
            						prev = node;
            					}
            				}
            				car_counter++;
            				threads[i] = thread(pass, car_path, car_rand, path_counter, car_counter, cars_num);            				
            			}

            			for (int j = 0; j < cars_num; j++)
        					threads[j].join();
            		}
            	}
            	is_next_input = 1;
            }
        }
           
    }else
        cout << "input file not exists!" << endl;
    return 0;
}