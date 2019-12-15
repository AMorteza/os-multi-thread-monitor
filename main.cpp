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

class Edge {
  
	public: string from_vertice;
	public: string to_vertice;
	private: int h;
	private: int p;

	public: Edge(string from_vertice, string to_vertice, int h, int p) 
    { 
        this->from_vertice = from_vertice; 
        this->to_vertice = to_vertice;
        this->h = h;
        this->p = p; 
    }

	public: void enter(){
        // cv.wait(lck);
		total_emission += this->emission();
        // cv.notify_one();
	}

	public: int emission()
	{
		int sum = 0;
		for (int k = 0; k <= 10000000; ++k){
			int term = (this->p * this->h * 1000000); 
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

void pass(std::vector<Edge*> edges, int path_num, int car_num, int cars_num)
{
	string entry_time = epoch_time();
	string exit_time;
	int path_emission = 0;
	for (int i = 0; i < edges.size(); ++i)
	{
		// edges[i]->enter();
		path_emission += edges[i]->emission();
	}

	exit_time = epoch_time();
    total_emission += path_emission;

	// cout << "path_num: " << path_num << "\n car_num: " << car_num 
	// << "\n entry_time: " << entry_time << "\n path_emission:" << path_emission
	// << "\n exit_time: " << exit_time << "\n total_emission:" << total_emission
	// << "\n enter_node: " << edges[0]->from_vertice
	// << "\n exit_node: " << edges[edges.size() - 1]->to_vertice 
	// << "\n---------------------------------------------------" << endl; 
    
	ofstream outFile(itos(path_num) + "-" + itos(car_num), ios::out);
    outFile << edges[0]->from_vertice << ", "
    << entry_time << ", "
    << edges[edges.size() - 1]->to_vertice  << ", "
    << exit_time << ", "
    << path_emission << ", "
    << total_emission << endl;
    outFile.close();
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
    			// cout << items[0] << items[1] << dict[v] << endl;
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
            				int p = (get_rand() * car_counter) % 10 + 1;
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

            				threads[i] = thread(pass, car_path, path_counter, car_counter, cars_num);
            				threads[i].join();

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