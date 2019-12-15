// Ensure that only one thread at a time can run a method of my C++ class.
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

class Edge {
  
	public: string from_vertice;
	public: string to_vertice;
	private: int h;
	private: int p;
	static sem_t gate;

	Edge(string from_vertice, string to_vertice, int h, int p) 
    { 
        this->from_vertice = from_vertice; 
        this->to_vertice = to_vertice;
        this->h = h;
        this->p = p; 
    }

	public: void enter(){
		sem_wait(&gate);
		total_emission += this->emission();
		sem_post(&gate);
	}

	private: int emission()
	{
		int sum = 0;
		for (int k = 0; k <= 10000000; ++k)
			sum += k / (this->p * this->h * 1000000);
		return sum;	
	}

	public: string epoch_time()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time (&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
		std::string str(buffer);
		return str;
	}
};

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

int get_rand(int lowest=1, int highest=10)
{
	srand((int)time(0));
	int range=(highest-lowest)+1;
	return rand() % range + 1;
}

int main()
{
	ifstream infile("input");
	if (infile.good())
    {
        vector<vector<string>> inputs;  
        string line;
        int is_next_input = 0;
        int is_second_line = 0;    
        vector<string> path;
    	map<vector<string>, int> dict;
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
            			for (int i = 0; i < cars_num; ++i)
            			{
            				int p = get_rand();
            				std::vector<Edge *> car_path;
            				int is_first_node = 0;
            				string tmp;
            				for (string node : path)
            				{
            					if (is_first_node == 0)
            					{
            						tmp = node;
            						is_first_node = 1;
            					}else{
            						vector<string> v;
    								v.push_back(tmp);
    								v.push_back(node);
            						int h = dict[v];
            						car_path.push_back(new Edge(tmp, node, h, p));
            						tmp = node;
            					}
            				
            				}
            				car_counter++;

            				//create thread to run pass function

            			}
            		}
            	}
            	is_next_input = 1;
            }
        }
           
    }else
        cout << "input file not exists!" << endl;
    return 0;
}