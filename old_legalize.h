#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <stack>
#include <list>
#include <map>
#include <cmath>
#include <ctime>
#include <utility>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdlib>
using namespace std;

class Module {
public:
    Module() = default;//area is real area
    Module(string name, int x, int y, int w, int h, int area, int overlapping, int identity) : name(name), x(x), y(y), w(w), h(h), area(area), overlapping(overlapping), identity(identity) 
    { 
        point.emplace_back(make_pair(x - (w / 2), y - (h / 2)));
        point.emplace_back(make_pair(x + (w / 2) + (w % 2), y - (h / 2)));
        point.emplace_back(make_pair(x + (w / 2) + (w % 2), y + (h / 2)+ (h % 2)));
        point.emplace_back(make_pair(x - (w / 2), y + (h / 2)+ (h % 2)));
    }
    string name;
    int x = 0, y = 0, w = 0, h = 0, area = 0, overlapping = 0, identity = 0;
    vector<pair<int, int> > point; 
};

//vector<char> legal_dir{'u', 'd', 'l', 'r'};

int cal_overlapping(Module a, Module b);
double tot_overlapping(int row, int col, map<string, Module> s_module, map<string, Module> h_module);
pair<int, double> cost(Module a, map<string, Module> s_module, map<string, Module> h_module);
pair<int, int> cost2(Module a, map<string, Module> s_module, map<string, Module> h_module);
pair<int, int> cost_count(Module a, map<string, Module> s_module, map<string, Module> h_module);
vector<int> relative_position(Module a, Module b);
void resize(Module a);
string move_dir(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y);
vector<Module> overlap_with_h(Module a, map<string, Module> h_module);
vector<Module> overlap_with_s(Module a, map<string, Module> s_module);
bool check_area_limit(int num, Module a, map<string, Module> s_module, map<string, Module> h_module);