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
    double final_x = 0, final_y = 0;
    vector<pair<int, int> > point; 
    vector<pair<int, int> > detail_point; 
};

//vector<char> legal_dir{'u', 'd', 'l', 'r'};

int cal_overlapping(Module a, Module b);
double tot_overlapping(int row, int col, map<string, Module> s_module, map<string, Module> h_module);
pair<int, int> tot_overlapping_count(map<string, Module> s_module, map<string, Module> h_module);
pair<int, double> cost(Module a, map<string, Module> s_module, map<string, Module> h_module);
pair<int, int> cost2(Module a, map<string, Module> s_module, map<string, Module> h_module);
pair<int, int> cost_count(Module a, map<string, Module> s_module, map<string, Module> h_module);
string move_dir(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y);
vector<Module> overlap_with_h(Module a, map<string, Module> h_module);
vector<Module> overlap_with_s(Module a, map<string, Module> s_module);
vector<Module> who_overlap(map<string, Module> s_module, map<string, Module> h_module);
bool check_area_limit(int num, Module a, map<string, Module> s_module, map<string, Module> h_module);
// void clear(Module a, map<string, Module> s_module, map<string, Module> h_module, int type);//0:all、1:no up、2:no down、3:no left、4:no right 
// void push(Module a, map<string, Module> s_module, map<string, Module> h_module, int posi);
// int relative_position(Module a, Module b);//1:up、2:down、3:left、4:right
bool legalize(Module &a, map<string, Module> &s_module, map<string, Module> h_module, int c_x, int c_y);
void point(map<string, Module> &s_module, map<string, Module> h_module);
bool all_boundary_check(map<string, Module> s_module, int c_x, int c_y);