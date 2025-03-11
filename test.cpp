#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cmath>
#include <random>
#include <cstdlib>
#include <limits>
#include "legalize.h"
using namespace std;

int row, col, soft_num, fixed_num, connection_num, true_random_num = 0;
map<string, int> soft_block;
map<string, pair<int, int>> soft_block_loc;
map<string, tuple<int, int, int, int>> fixed_block;
map<string, pair<int, int>> mid_fixed;
multimap<int, pair<string, string>> connection;
map<string, pair<int, int>> hardened_soft_block;
random_device rd;
mt19937 gen(rd());

void input(ifstream &fin);
void find_area(string name, int area);

void forced_directed(ofstream &fout);
void large_macro_gp(multimap<int, tuple<string, int, int>, greater<int>> &large_macro);
pair<int, int> compute_zero_force_loc(const string &name, map<string, tuple<int, int, bool>> &locations);
bool find_if_occupied1(const string &name, const pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations);
bool fixed_block_check(const string &name, const pair<int, int> &loc);
bool translation(const string &name, pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations, const int &direction);
void update_location(const string &name, const pair<string, string> &update_name, const pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations, const bool &choice);
bool boundary_check(const string &name, const pair<int, int> &loc);
pair<int, int> boundary_fix(const string &name, const pair<int, int> &loc);

void overlap_reduction(map<string, Module> &s_module, map<string, Module> &h_module, bool choice);
pair<int, int> calculate_resultant_force(const string &name);
void move(const string &name, const pair<int, int> &force);
bool find_if_occupied2(const string &name, const pair<int, int> &loc, map<string, pair<int, int>> &locations);

int calculate_wirelength();
void plot(ofstream &fout);

int main(int argc, char *argv[])
{
    ifstream fin;
    ofstream fout1, fout2;
    fin.open(argv[1]);
    fout1.open(argv[2]);
    fout2.open(argv[3]);
    string name(argv[1]);

    input(fin);
    
    double overlap_area = numeric_limits<double>::max();
    map<string, pair<int, int>> temp = soft_block_loc, final;
    map<string, Module> s_module, h_module;
    for(int i = 0; i < 10; i ++)
    {
        cout << "Round " << i << endl;
        forced_directed(fout1);
        map<string, Module> s_temp, h_temp;
        for(auto i : soft_block) 
        {
            Module temp(i.first, soft_block_loc[i.first].first, soft_block_loc[i.first].second, hardened_soft_block[i.first].first, hardened_soft_block[i.first].second, i.second, 0, 1);
            s_temp.insert(make_pair(i.first, temp));
        }

        for(auto i : mid_fixed)
        {
            Module temp(i.first, mid_fixed[i.first].first, mid_fixed[i.first].second, get<2>(fixed_block[i.first]), get<3>(fixed_block[i.first]), 0, 0, 0);
            h_temp.insert(make_pair(i.first, temp));
        }

        double num = tot_overlapping(row, col, s_temp, h_temp);
        cout << tot_overlapping(row, col, s_temp, h_temp) << "%" << endl;
        //cout << calculate_wirelength() << endl;
        if(num < overlap_area)
        {
            overlap_area = num;
            s_module = s_temp;
            h_module = h_temp;
            final = soft_block_loc;
        }
        soft_block_loc = temp;
    }
    
    soft_block_loc = final;
    cout << "Overlap percentage before: " << tot_overlapping(row, col, s_module, h_module) << "%" << ", wirelength before reduction: " << calculate_wirelength() << endl;
    plot(fout1);

    //int fail_num = 0;
    double performance;
    overlap_reduction(s_module, h_module, 0);
    cout << "Overlap percentage after reduction: " << tot_overlapping(row, col, s_module, h_module) << "%" << ", wirelength after reduction: " << calculate_wirelength() << endl;
    performance = tot_overlapping(row, col, s_module, h_module);
    while(1)
    {
        overlap_reduction(s_module, h_module, 1);
        double result = tot_overlapping(row, col, s_module, h_module);
        cout << "Overlap percentage after reduction: " << tot_overlapping(row, col, s_module, h_module) << "%" << ", wirelength after reduction: " << calculate_wirelength() << endl;
        if(performance == result)
                break;
        else
            performance = result;
    }
    
    plot(fout2);
    
    //large_macro_legalization();
    //plot(fout2);
    
    //cout << calculate_wirelength() << endl;
    //cout << "True random num: " << true_random_num << endl;

    return 0;
}

void input(ifstream &fin)
{
    string trash;
    int used_area = 0;

    fin >> trash >> col >> row >> trash >> soft_num;
    for(int i = 0; i < soft_num; i ++)
    {
        string name;
        int size;

        fin >> name >> size;
        soft_block.insert(make_pair(name, size));
        soft_block_loc.insert(make_pair(name, make_pair(col / 2, row / 2)));
        find_area(name, size);
    }

    fin >> trash >> fixed_num;
    for(int i = 0; i < fixed_num; i ++)
    {
        string name;
        int x, y, width, height;

        fin >> name >> x >> y >> width >> height;
        int mid_x = x + (width / 2), mid_y = y + (height / 2);
        fixed_block.insert(make_pair(name, make_tuple(x, y, width, height)));
        mid_fixed.insert(make_pair(name, make_pair(mid_x, mid_y)));
    }

    fin >> trash >> connection_num;
    for(int i = 0; i < connection_num; i ++)
    {
        string con1, con2;
        int weight;

        fin >> con1 >> con2 >> weight;
        connection.insert(pair<int, pair<string, string>>(weight, make_pair(con1, con2)));
    }
}

void find_area(string name, int area)
{
    int length = ceil(sqrt(area)), temp = length;

    for(; temp > 0; temp --)
    {
        if(length * temp < area)
        {
            temp += 1;
            break;
        }
    }
    hardened_soft_block.insert(pair<string, pair<int, int>>(name, make_pair(length, temp)));
}

void forced_directed(ofstream &fout)
{
    multimap<int, tuple<string, int, int>, greater<int>> large_macro;
    map<string, int> large_macro_name;
    int max_size = 0;

    for(auto i : soft_block)
    {
        if(i.second > max_size)
            max_size = i.second;
    }

    int size = max_size / 5;
    for(auto i : soft_block)
    {
        if(i.second > size)
        {
            large_macro.insert(make_pair(soft_block[i.first], make_tuple(i.first, soft_block_loc[i.first].first, soft_block_loc[i.first].second)));
            large_macro_name.insert(make_pair(i.first, soft_block[i.first]));
        }
    }
    if(large_macro.size() < soft_block.size() / 2)
        large_macro_gp(large_macro);
    else
    {
        large_macro.clear();
        large_macro_name.clear();
    } 
    
    multimap<int, string> connectivity;
    for(auto i : soft_block)
    {
        string name = i.first;
        int num = 0;
        for(auto j : connection)
        {
            if(j.second.first == name || j.second.second == name)
                 num ++;
        }
        connectivity.insert(make_pair(num, name));
    }
    multimap<int, string> blocks = connectivity;

    map<string, tuple<int, int, bool>> locations;
    for(auto i : mid_fixed)
        locations.insert(make_pair(i.first, make_tuple(i.second.first, i.second.second, true)));
    for(auto i : soft_block_loc)
    {
        if(large_macro_name.find(i.first) != large_macro_name.end())
            locations.insert(make_pair(i.first, make_tuple(i.second.first, i.second.second, true)));
        else
            locations.insert(make_pair(i.first, make_tuple(i.second.first, i.second.second, false)));
    }

    int iter_count = 0, iter_limit = 3, abort_count = 0, abort_limit = 10;
    while(iter_count < iter_limit)
    {
        multimap<int, string>::iterator it;
        while(1)
        {
            if(!blocks.size())
            {
                blocks = connectivity;
                it = prev(blocks.end());
                if(!get<2>(locations[it -> second]))
                    break;
                else
                    blocks.erase(prev(blocks.end()));
            }
            else
            {
                it = prev(blocks.end());
                if(!get<2>(locations[it -> second]))
                    break;
                else
                    blocks.erase(prev(blocks.end()));
            }
        }

        pair<int, string> info(it -> first, it -> second);
        bool end_ripple = false;
        string name = info.second;
        int connections = info.first;
        blocks.erase(prev(blocks.end())); 

        while(!end_ripple)
        {
            pair<int, int> loc = compute_zero_force_loc(name, locations);
            // cout << "\nBlock name: " << name << ", zero force location, x: " << loc.first << ", y: " << loc.second << endl;
            // cout << "Full coordinates: " << loc.first - hardened_soft_block[name].first / 2 << ", " << loc.first + hardened_soft_block[name].first / 2 << 
            //     ", " << loc.second - hardened_soft_block[name].second / 2 << ", " << loc.second + hardened_soft_block[name].second / 2 << endl;
            bool same_place = false, locked = false, occupied = false;
            pair<string, string> locked_name, occupied_name;
            for(auto i : locations)
            {
                if(name == i.first && loc.first == get<0>(i.second) && loc.second == get<1>(i.second))
                {
                    same_place = true;
                    break;
                }
            }
            if(!same_place)
            {
                for(auto i : locations)
                {
                    if(name != i.first)
                    {
                        if(mid_fixed.find(i.first) != mid_fixed.end())
                        {
                            int x1 = get<0>(locations[i.first]) - (get<2>(fixed_block[i.first]) / 2), x2 = get<0>(locations[i.first]) + (get<2>(fixed_block[i.first]) / 2) + (get<2>(fixed_block[i.first]) % 2),
                                y1 = get<1>(locations[i.first]) - (get<3>(fixed_block[i.first]) / 2), y2 = get<1>(locations[i.first]) + (get<3>(fixed_block[i.first]) / 2) + (get<3>(fixed_block[i.first]) % 2),
                                loc_x1 = loc.first - (hardened_soft_block[name].first / 2), loc_x2 = loc.first + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2),
                                loc_y1 = loc.second - (hardened_soft_block[name].second / 2), loc_y2 = loc.second + (hardened_soft_block[name].second / 2) + (hardened_soft_block[name].second % 2);
                                //cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                                //cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                                //cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;

                            if((((((loc_x1 < x2 - get<2>(fixed_block[i.first]) / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + get<2>(fixed_block[i.first]) / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + get<3>(fixed_block[i.first]) / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - get<3>(fixed_block[i.first]) / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))) && get<2>(i.second))
                            {
                                //cout << x2 - get<2>(fixed_block[i.first]) / 6 << ", " << x1 + get<2>(fixed_block[i.first]) / 6 << ", " << y1 + get<3>(fixed_block[i.first]) / 6 << ", " << y2 - get<3>(fixed_block[i.first]) / 6 << endl;
                                //cout << "Find locked block, name: " << i.first << ", location: " << get<0>(locations[i.first]) << ", "  << get<1>(locations[i.first]) << endl;
                                locked = true;
                                locked_name = make_pair("fixed", i.first);
                                break;
                            }
                        }
                        else
                        {
                            int x1 = get<0>(locations[i.first]) - double(hardened_soft_block[i.first].first / 2), x2 = get<0>(locations[i.first]) + double(hardened_soft_block[i.first].first / 2) + 1,
                                y1 = get<1>(locations[i.first]) - double(hardened_soft_block[i.first].second / 2), y2 = get<1>(locations[i.first]) + double(hardened_soft_block[i.first].second / 2) + 1,
                                loc_x1 = loc.first - double(hardened_soft_block[name].first / 2), loc_x2 = loc.first + double(hardened_soft_block[name].first / 2) + 1,
                                loc_y1 = loc.second - double(hardened_soft_block[name].second / 2), loc_y2 = loc.second + double(hardened_soft_block[name].second / 2) + 1;
                                //cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                                //cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                                //cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
                            
                            if((((((loc_x1 < x2 - hardened_soft_block[i.first].first / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + hardened_soft_block[i.first].first / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + hardened_soft_block[i.first].second / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - hardened_soft_block[i.first].second / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))) && get<2>(i.second))
                            {
                                //cout << x2 - hardened_soft_block[i.first].first / 6 << ", " << x1 + hardened_soft_block[i.first].first / 6 << ", " << y1 + hardened_soft_block[i.first].second / 6 << ", " << y2 - hardened_soft_block[i.first].second / 6 << endl;
                                //cout << "Find locked block, name: " << i.first << ", location: " << get<0>(locations[i.first]) << ", "  << get<1>(locations[i.first]) << endl;
                                locked = true;
                                locked_name = make_pair("soft", i.first);
                                break;
                            }
                            else if((((((loc_x1 < x2 - hardened_soft_block[i.first].first / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + hardened_soft_block[i.first].first / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + hardened_soft_block[i.first].second / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - hardened_soft_block[i.first].second / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))) && !get<2>(i.second))
                            {
                                //cout << x2 - hardened_soft_block[i.first].first / 6 << ", " << x1 + hardened_soft_block[i.first].first / 6 << ", " << y1 + hardened_soft_block[i.first].second / 6 << ", " << y2 - hardened_soft_block[i.first].second / 6 << endl;
                                //cout << "Find occupied block, name: " << i.first << ", location: " << get<0>(locations[i.first]) << ", "  << get<1>(locations[i.first]) << endl;
                                occupied = true;
                                occupied_name = make_pair("soft", i.first);  
                                break;
                            }
                        }
                    }
                }
            }

            if(same_place)
            {
                //cout << "Same place" << endl;
                end_ripple = true;
                abort_count = 0;
            }
            else if(locked)
            {
                //cout << "Locked block" << endl;
                update_location(name, locked_name, loc, locations, 0);

                end_ripple = true;
                abort_count ++;
                if(abort_count > abort_limit)
                {
                    for(auto &i : locations)
                    {
                        if(fixed_block.find(i.first) != fixed_block.end() || large_macro_name.find(i.first) != large_macro_name.end())
                            get<2>(i.second) = true;
                        else
                            get<2>(i.second) = false;
                    }
                    iter_count += 1;
                    abort_count = 0;
                    //cout << "Iter count: " << iter_count << endl;
                }
            }
            else if(occupied)
            {
                //cout << "Occupied" << endl;
                map<string, tuple<int, int, bool>> temp = locations;
                temp.erase(occupied_name.second);
                if(!find_if_occupied1(name, make_pair(loc.first, loc.second), temp))
                {
                    if(boundary_check(name, make_pair(loc.first, loc.second)))
                    {
                        locations[name] = make_tuple(loc.first, loc.second, true);
                        name = occupied_name.second;
                    }
                    else
                    {
                        pair<int, int> temp = boundary_fix(name, make_pair(loc.first, loc.second));
                        locations[name] = make_tuple(temp.first, temp.second, true);
                        name = occupied_name.second;
                    }
                    abort_count = 0;
                }
                else
                {
                    update_location(name, occupied_name, loc, locations, 1);
                    end_ripple = true;
                }
            }
            else
            {
                locations[name] = make_tuple(loc.first, loc.second, true);
                end_ripple = true;
                abort_count = 0;
            }
        }
    }

    for(auto &i : soft_block_loc)
        i.second = make_pair(get<0>(locations[i.first]), get<1>(locations[i.first]));

    // for(auto i : soft_block_loc)
    //     cout << "Block name: " << i.first << ", location, x: " << i.second.first << ", y: " << i.second.second << endl;
}

pair<int, int> compute_zero_force_loc(const string &name, map<string, tuple<int, int, bool>> &locations)
{
    int total_weight = 0, partial_x = 0, partial_y = 0;
    for(auto i : connection)
    {
        if(i.second.first == name || i.second.second == name)
        {
            int weight = i.first;
            total_weight += weight;
            if(i.second.first == name && locations.find(i.second.second) != locations.end())
            { 
                string connected = i.second.second; 
                int x = get<0>(locations[connected]), y = get<1>(locations[connected]);
                
                partial_x += x * weight;
                partial_y += y * weight;
            }
            else if(locations.find(i.second.first) != locations.end())
            {
                string connected = i.second.first;   
                int x = get<0>(locations[connected]), y = get<1>(locations[connected]);
                
                partial_x += x * weight;
                partial_y += y * weight;
            }
        }
    }
    int x = round(double(partial_x) / double(total_weight)), y = round(double(partial_y) / double(total_weight));
    
    return make_pair(x, y);
}

void large_macro_gp(multimap<int, tuple<string, int, int>, greater<int>> &large_macro)
{
    // for(auto i : large_macro)
    //     cout << "Name: " << get<0>(i.second) << ", size: " << i.first << ", x: " << get<1>(i.second) << ", y: " << get<2>(i.second) << endl;

    multimap<int, tuple<string, int, int>, greater<int>> blocks = large_macro;

    map<string, tuple<int, int, bool>> locations;
    for(auto i : mid_fixed)
        locations.insert(make_pair(i.first, make_tuple(i.second.first, i.second.second, true)));
    // for(auto i : large_macro)
    //     locations.insert(make_pair(get<0>(i.second), make_tuple(get<1>(i.second), get<2>(i.second), false)));

    // for(auto i : locations)
    //     cout << "Name: " << i.first << ", x: " << get<0>(i.second) << ", y: " << get<1>(i.second) << ", locked: " << get<2>(i.second) << endl;

    while(blocks.size() != 0)
    {
        string name = get<0>(blocks.begin() -> second);
        blocks.erase(blocks.begin());

        pair<int, int> loc = compute_zero_force_loc(name, locations);
        locations.insert(make_pair(name, make_tuple(loc.first, loc.second, false)));
        //cout << "\nZero force location: " << loc.first << ", " << loc.second << endl;
        if(blocks.size() == large_macro.size() - 1)
        {
            //cout << "Hi" << endl;
            int x = loc.first, y = loc.second;
            bool done = false;
            // cout << "\nBlock name: " << name << ", zero force location, x: " << loc.first << ", y: " << loc.second << endl;
            // cout << "Full coordinates: " << get<0>(locations[name]) - hardened_soft_block[name].first / 2 << ", " << get<0>(locations[name]) + hardened_soft_block[name].first / 2 << 
            //     ", " << get<1>(locations[name]) - hardened_soft_block[name].second / 2 << ", " << get<1>(locations[name]) + hardened_soft_block[name].second / 2 << endl;

            if(x >= col / 2 && y >= row / 2)
            {
                //cout << "1" << endl;
                x = col;
                y = row;
                while(x >= col / 2 || y >= row / 2)
                {
                    if(!fixed_block_check(name, loc) && boundary_check(name, loc))
                    {
                        //cout << "Find free space at x: " << x << ", y: " << y << endl;
                        soft_block_loc[name] = make_pair(x, y);
                        locations[name] = make_tuple(x, y, true);
                        done = true;
                        break;
                    }
                    if(x < col / 2 && y >= row / 2)
                        y -= 10;
                    else if(x >= col / 2 && y < row / 2)
                        x -= 10;
                    else
                    {
                        x -= 10;
                        y -= 10;
                    }
                    
                }
                if(!done)
                {
                    //cout << "Failed to find free space" << endl;
                }
            }
            else if(x >= col / 2 && y < row / 2)
            {
                //cout << "2" << endl;
                x = col;
                y = 0;
                while(x >= col / 2 || y < row / 2)
                {
                    if(!fixed_block_check(name, loc) && boundary_check(name, loc))
                    {
                        //cout << "Find free space at x: " << x << ", y: " << y << endl;
                        soft_block_loc[name] = make_pair(x, y);
                        locations[name] = make_tuple(x, y, true);
                        done = true;
                        break;
                    }
                    if(x < col / 2 && y < row / 2)
                        y += 10;
                    else if(x >= col / 2 && y >= row / 2)
                        x -= 10;
                    else
                    {
                        x -= 10;
                        y += 10;
                    }  
                }
                if(!done)
                {
                    //cout << "Failed to find free space" << endl;
                }
            }
            else if(x < col / 2 && y >= row / 2)
            {
                //cout << "3" << endl;
                x = 0;
                y = row;
                while(x < col / 2 || y >= row / 2)
                {
                    if(!fixed_block_check(name, loc) && boundary_check(name, loc))
                    {
                        //cout << "Find free space at x: " << x << ", y: " << y << endl;
                        soft_block_loc[name] = make_pair(x, y);
                        locations[name] = make_tuple(x, y, true);
                        done = true;
                        break;
                    }
                    if(x >= col / 2 && y >= row / 2)
                        y -= 10;
                    else if(x < col / 2 && y < row / 2)
                        x += 10;
                    else
                    {
                        x += 10;
                        y -= 10;
                    }              
                }
                if(!done)
                {
                    //cout << "Failed to find free space" << endl;
                }
            }
            else
            {
                //cout << "4, x: " << col / 2 << ",y: " << row / 2 << endl;
                x = 0;
                y = 0;
                while(x < col / 2 || y < row / 2)
                {
                    //cout << x << ", " << y << endl;
                    if(!fixed_block_check(name, make_pair(x, y)) && boundary_check(name, make_pair(x, y)))
                    {
                        //cout << "Find free space at x: " << x << ", y: " << y << endl;
                        soft_block_loc[name] = make_pair(x, y);
                        locations[name] = make_tuple(x, y, true);
                        done = true;
                        break;
                    }
                    if(x >= col / 2 && y < row / 2)
                        y += 10;
                    else if(x < col / 2 && y >= row / 2)
                        x += 10;
                    else
                    {
                        x += 10;
                        y += 10;
                    }
                }
                if(!done)
                {
                    //cout << "Failed to find free space" << endl;
                }
            }
        }
        else
        {
            //cout << "Not first block" << endl;
            int x = loc.first, y = loc.second;
            bool done = false, locked = false;
            pair<string, string> locked_name;

            for(auto i : locations)
            {
                if(name != i.first)
                {
                    if(mid_fixed.find(i.first) != mid_fixed.end())
                    {
                        int x1 = get<0>(locations[i.first]) - (get<2>(fixed_block[i.first]) / 2), x2 = get<0>(locations[i.first]) + (get<2>(fixed_block[i.first]) / 2) + (get<2>(fixed_block[i.first]) % 2),
                            y1 = get<1>(locations[i.first]) - (get<3>(fixed_block[i.first]) / 2), y2 = get<1>(locations[i.first]) + (get<3>(fixed_block[i.first]) / 2) + (get<3>(fixed_block[i.first]) % 2),
                            loc_x1 = loc.first - (hardened_soft_block[name].first / 2), loc_x2 = loc.first + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2),
                            loc_y1 = loc.second - (hardened_soft_block[name].second / 2), loc_y2 = loc.second + (hardened_soft_block[name].second / 2) + (hardened_soft_block[name].second % 2);

                        if((((((loc_x1 < x2 - get<2>(fixed_block[i.first]) / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + get<2>(fixed_block[i.first]) / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + get<3>(fixed_block[i.first]) / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - get<3>(fixed_block[i.first]) / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))) && get<2>(i.second))
                        {
                            locked = true;
                            locked_name = make_pair("fixed", i.first);
                            break;
                        }
                    }
                    else
                    {
                        int x1 = get<0>(locations[i.first]) - double(hardened_soft_block[i.first].first / 2), x2 = get<0>(locations[i.first]) + double(hardened_soft_block[i.first].first / 2) + 1,
                            y1 = get<1>(locations[i.first]) - double(hardened_soft_block[i.first].second / 2), y2 = get<1>(locations[i.first]) + double(hardened_soft_block[i.first].second / 2) + 1,
                            loc_x1 = loc.first - double(hardened_soft_block[name].first / 2), loc_x2 = loc.first + double(hardened_soft_block[name].first / 2) + 1,
                            loc_y1 = loc.second - double(hardened_soft_block[name].second / 2), loc_y2 = loc.second + double(hardened_soft_block[name].second / 2) + 1;
                        
                        if((((((loc_x1 < x2 - hardened_soft_block[i.first].first / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + hardened_soft_block[i.first].first / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + hardened_soft_block[i.first].second / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - hardened_soft_block[i.first].second / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))) && get<2>(i.second))
                        {
                            locked = true;
                            locked_name = make_pair("soft", i.first);
                            break;
                        }
                    }
                }
            }
            if(locked)
            {
                //cout << "Zero force location occupied" << endl;
                update_location(name, locked_name, loc, locations, 1);
                soft_block_loc[name] = make_pair(get<0>(locations[name]), get<1>(locations[name]));
                get<2>(locations[name]) = true;
            }
            else
            {
                //cout << "First time find free space at x: " << x << ", y: " << y << endl;
                soft_block_loc[name] = make_pair(x, y);
                locations[name] = make_tuple(x, y, true);
            }
        }
    }
}

bool find_if_occupied1(const string &name, const pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations)
{
    bool occupied = false;

    for(auto i : locations)
    {
        if(name != i.first)
        {
            if(mid_fixed.find(i.first) != mid_fixed.end())
            {
                int x1 = get<0>(locations[i.first]) - (get<2>(fixed_block[i.first]) / 2), x2 = get<0>(locations[i.first]) + (get<2>(fixed_block[i.first]) / 2) + (get<2>(fixed_block[i.first]) % 2),
                    y1 = get<1>(locations[i.first]) - (get<3>(fixed_block[i.first]) / 2), y2 = get<1>(locations[i.first]) + (get<3>(fixed_block[i.first]) / 2) + (get<3>(fixed_block[i.first]) % 2),
                    loc_x1 = loc.first - (hardened_soft_block[name].first / 2), loc_x2 = loc.first + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2),
                    loc_y1 = loc.second - (hardened_soft_block[name].second / 2), loc_y2 = loc.second + (hardened_soft_block[name].second / 2) + (hardened_soft_block[name].second % 2);
                // cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                // cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                // cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
                if((((((loc_x1 < x2 - get<2>(fixed_block[i.first]) / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + get<2>(fixed_block[i.first]) / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + get<3>(fixed_block[i.first]) / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - get<3>(fixed_block[i.first]) / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))))
                {
                    occupied = true;
                    break;
                }
            }
            else
            {
                int x1 = get<0>(locations[i.first]) - double(hardened_soft_block[i.first].first / 2), x2 = get<0>(locations[i.first]) + double(hardened_soft_block[i.first].first / 2) + 1,
                    y1 = get<1>(locations[i.first]) - double(hardened_soft_block[i.first].second / 2), y2 = get<1>(locations[i.first]) + double(hardened_soft_block[i.first].second / 2) + 1,
                    loc_x1 = loc.first - double(hardened_soft_block[name].first / 2), loc_x2 = loc.first + double(hardened_soft_block[name].first / 2) + 1,
                    loc_y1 = loc.second - double(hardened_soft_block[name].second / 2), loc_y2 = loc.second + double(hardened_soft_block[name].second / 2) + 1;
                // cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                // cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                // cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
                // cout << "Original block width / 6: " << hardened_soft_block[i.first].first / 6 << ", new block(loc) width / 6: " << hardened_soft_block[name].first / 6 << endl;
                if((((((loc_x1 < x2 - hardened_soft_block[i.first].first / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + hardened_soft_block[i.first].first / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + hardened_soft_block[i.first].second / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - hardened_soft_block[i.first].second / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))))
                {
                    occupied = true;
                    break;
                }
            }
        }
    }
    return occupied;
}

bool fixed_block_check(const string &name, const pair<int, int> &loc)
{
    bool occupied = false;

    for(auto i : mid_fixed)
    {
        int x1 = i.second.first - (get<2>(fixed_block[i.first]) / 2), x2 = i.second.first + (get<2>(fixed_block[i.first]) / 2) + (get<2>(fixed_block[i.first]) % 2),
            y1 = i.second.second - (get<3>(fixed_block[i.first]) / 2), y2 = i.second.second + (get<3>(fixed_block[i.first]) / 2) + (get<2>(fixed_block[i.first]) % 2),
            loc_x1 = loc.first - (hardened_soft_block[name].first / 2), loc_x2 = loc.first + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2),
            loc_y1 = loc.second - (hardened_soft_block[name].second / 2), loc_y2 = loc.second + (hardened_soft_block[name].second / 2) + (hardened_soft_block[name].first % 2);

        if((((((loc_x1 < x2 - get<2>(fixed_block[i.first]) / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + get<2>(fixed_block[i.first]) / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + get<3>(fixed_block[i.first]) / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - get<3>(fixed_block[i.first]) / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))))
        {
            //cout << "Occupied, x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << "; loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
            occupied = true;
            break;
        }
    }
    return occupied;
}

bool translation(const string &name, pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations, const int &direction)
{
    bool find = false;
    //cout << "Original x: " << loc.first << ", y: " << loc.second << endl;
    // for(auto i : locations)
    //     cout << "Block name: " << i.first << ", x: " << get<0>(i.second) << ", y: " << get<1>(i.second) << ", locked: " << get<2>(i.second) << endl;

    if(direction == 0)
    {
        for(int i = 0; i < col; i += 50)
        {
            if(!find_if_occupied1(name, make_pair(i, loc.second), locations) && boundary_check(name, make_pair(i, loc.second)))
            {
                //cout << "Find during translation0, x: " << i << ", y: " << loc.second << endl;
                loc.first = i;
                find = true;
                break;
            }
        }
    }
    else
    {
        for(int i = 0; i < row; i += 50)
        {
            if(!find_if_occupied1(name, make_pair(loc.first, i), locations) && boundary_check(name, make_pair(loc.first, i)))
            {
                //cout << "Find during translation1, x: " << loc.first << ", y: " << i << endl;
                loc.second = i;
                find = true;
                break;
            }
        }
    }

    return find;
}

void update_location(const string &name, const pair<string, string> &update_name, const pair<int, int> &loc, map<string, tuple<int, int, bool>> &locations, const bool &choice)
{
    uniform_int_distribution<> distr1(0, col);
    uniform_int_distribution<> distr2(0, row);

    if(update_name.first == "fixed")
    {
        //cout << "Update block is fixed" << endl;
        bool find_not_occupied = false;
        pair<int, int> offset = hardened_soft_block[name];
        int offset_x = offset.first / 6, offset_y = offset.second / 6,
            x1 = get<0>(locations[update_name.second]) - mid_fixed[update_name.second].first / 2, 
            x2 = get<0>(locations[update_name.second]) + mid_fixed[update_name.second].first / 2 + mid_fixed[update_name.second].first % 2,
            y1 = get<1>(locations[update_name.second]) - mid_fixed[update_name.second].second / 2, 
            y2 = get<1>(locations[update_name.second]) + mid_fixed[update_name.second].second / 2 + mid_fixed[update_name.second].second % 2;

        if(x1 + offset_x > offset.first)
        {
            int mid_x = x1 + offset_x - (offset.first / 2), mid_y =  loc.second;
            //cout << "Finding new place1, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 1))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if((x2 - offset_x) < (col - offset.first) && !find_not_occupied)
        {
            int mid_x = x2 - offset_x + (offset.first / 2), mid_y =  loc.second;
            //cout << "Finding new place2, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 1))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(y1 + offset_y > offset.second && !find_not_occupied)
        {
            int mid_x = loc.first, mid_y = y1 + offset_y - (offset.second / 2);
            //cout << "FInding new place3, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 0))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(y2 - offset_y < row - offset.second && !find_not_occupied)
        {
            int mid_x = loc.first, mid_y = y2 - offset_y + (offset.second / 2);
            //cout << "FInding new place4, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 0))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(!find_not_occupied)
        {
            //cout << "Cannot Find new place, start randomizing" << endl;
            int fail_num = 0;
            bool find = false;
            while(fail_num < 50000)
            {
                //cout << fail_num << endl;
                int x = distr1(gen), y = distr2(gen);
                pair<int, int> new_loc(x, y);
                for(auto i : locations)
                {
                    if(name != i.first && boundary_check(name, new_loc) && !find_if_occupied1(name, new_loc, locations))
                    {
                        find = true;
                        break;
                    }
                }
                if(find)
                {
                    //cout << "Find place in " << fail_num << " try, x: " << x << ", y: " << y << endl;
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    break;
                }
                fail_num ++;
            }
            if(!find)
            {
                while(1)
                {
                    int x = distr1(gen), y = distr2(gen);
                    //cout << "Failed over 50000 times, random x: " << x << ", y: " << y << endl;
                    pair<int, int> new_loc(x, y);

                    if(boundary_check(name, new_loc) && !fixed_block_check(name, new_loc))
                    {
                        tuple<int, int, bool> temp;
                        if(choice == 0)
                            temp = make_tuple(new_loc.first, new_loc.second, false);
                        else
                            temp = make_tuple(new_loc.first, new_loc.second, true);
                        locations[name] = temp;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        //cout << "Update block is soft" << endl;
        bool find_not_occupied = false;
        pair<int, int> offset = hardened_soft_block[name];
        int offset_x = offset.first / 6, offset_y = offset.second / 6,
            x1 = get<0>(locations[update_name.second]) - hardened_soft_block[update_name.second].first / 2, 
            x2 = get<0>(locations[update_name.second]) + hardened_soft_block[update_name.second].first / 2 + hardened_soft_block[update_name.second].first % 2,
            y1 = get<1>(locations[update_name.second]) - hardened_soft_block[update_name.second].second / 2, 
            y2 = get<1>(locations[update_name.second]) + hardened_soft_block[update_name.second].second / 2 + hardened_soft_block[update_name.second].second % 2;

        // cout << "New block: " << name << ", previous block: " << update_name.second << endl;
        // cout << "x1: " << x1 << ", x2: " << x2 << ", y1: " << y1 << ", y2: " << y2 << endl;
        // cout << "Offset_x: " << offset_x << ", offset_y: " << offset_y << endl;

        if((x1 + offset_x) > offset.first)
        {
            int mid_x = x1 + offset_x - (offset.first / 2), mid_y =  loc.second;
            //cout << "FInding new place1, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 1))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(((x2 - offset_x) < (col - offset.first)) && !find_not_occupied)
        {
            int mid_x = x2 - offset_x + (offset.first / 2), mid_y =  loc.second;
            //cout << "FInding new place2, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 1))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(((y1 + offset_y) > (offset.second)) && !find_not_occupied)
        {
            int mid_x = loc.first, mid_y = y1 + offset_y - (offset.second / 2);
            //cout << "FInding new place3, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 0))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(((y2 - offset_y) < (row - offset.second)) && !find_not_occupied)
        {
            int mid_x = loc.first, mid_y = y2 - offset_y + (offset.second / 2);
            //cout << "FInding new place4, x: " << mid_x << ", y: " << mid_y << endl;
            pair<int, int> new_loc(mid_x, mid_y);
            if(!find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
            {
                //cout << "Free space found for " << name << ", location x: " << new_loc.first << ", y: " << new_loc.second << endl;
                tuple<int, int, bool> temp;
                if(choice == 0)
                    temp = make_tuple(new_loc.first, new_loc.second, false);
                else
                    temp = make_tuple(new_loc.first, new_loc.second, true);
                locations[name] = temp;
                find_not_occupied = true;
            }
            else
            {
                if(translation(name, new_loc, locations, 0))
                {
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    find_not_occupied = true;
                }
            }
            // if(!find_not_occupied)
            //     cout << "Cannot find free space" << endl;
        }
        if(!find_not_occupied)
        {
            //cout << "Cannot Find new place, start randomizing" << endl;
            int fail_num = 0;
            bool find = false;
            while(fail_num < 50000)
            {
                //cout << "Hi" << endl;
                int x = distr1(gen), y = distr2(gen);
                //cout << "random x: " << x << ", y: " << y << endl;
                pair<int, int> new_loc(x, y);
                for(auto i : locations)
                {
                    if(name != i.first && !find_if_occupied1(name, new_loc, locations) && boundary_check(name, new_loc))
                    {
                        find = true;
                        break;
                    }
                    //break;
                }
                if(find)
                {
                    //cout << "Find place in " << fail_num << " try, x: " << x << ", y: " << y << endl;
                    tuple<int, int, bool> temp;
                    if(choice == 0)
                        temp = make_tuple(new_loc.first, new_loc.second, false);
                    else
                        temp = make_tuple(new_loc.first, new_loc.second, true);
                    locations[name] = temp;
                    break;
                }
                fail_num ++;
            }
            if(!find)
            {
                while(1)
                {
                    int x = distr1(gen), y = distr2(gen);
                    //cout << "Failed over 50000 times, random x: " << x << ", y: " << y << endl;
                    pair<int, int> new_loc(x, y);

                    if(boundary_check(name, new_loc) && !fixed_block_check(name, new_loc))
                    {
                        tuple<int, int, bool> temp;
                        if(choice == 0)
                            temp = make_tuple(new_loc.first, new_loc.second, false);
                        else
                            temp = make_tuple(new_loc.first, new_loc.second, true);
                        locations[name] = temp;
                        break;
                    }
                }
            }
        }
    }
}

bool boundary_check(const string &name, const pair<int, int> &loc)
{
    bool check = false;

    if(loc.first - (hardened_soft_block[name].first / 2) > 0 && loc.first + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2) < col &&
        loc.second - (hardened_soft_block[name].second / 2) > 0 && loc.second + (hardened_soft_block[name].second / 2) + (hardened_soft_block[name].second % 2) < row)
        check = true;

    return check;
}

pair<int, int> boundary_fix(const string &name, const pair<int, int> &loc)
{
    int x = loc.first, y = loc.second;
    if(x - (hardened_soft_block[name].first / 2) < 0)
    {
        int offset = abs(x - (hardened_soft_block[name].first / 2));
        x += offset;
    } 
    else if(x + (hardened_soft_block[name].first / 2) + (hardened_soft_block[name].first % 2) > col)
    {
        int offset = abs(row - x - (hardened_soft_block[name].first / 2) - (hardened_soft_block[name].first % 2));
        x -= offset;
    }
    else if(y - (hardened_soft_block[name].second / 2) < 0)
    {
        int offset = abs(y - (hardened_soft_block[name].second / 2));
        y += offset;
    }
    else
    {
        int offset = abs(row - y - (hardened_soft_block[name].second / 2) - (hardened_soft_block[name].second % 2));
        y -= offset;
    }

    return make_pair(x, y);
}




void overlap_reduction(map<string, Module> &s_module, map<string, Module> &h_module, bool choice)
{
    double total_overlap_percentage = tot_overlapping(row, col, s_module, h_module);
    multimap<int, Module> overlap_count;
    map<string, double> overlap_percent;
    map<string, pair<int, int>> locations;
    vector<pair<int, Module>> blocks;
    
    for(auto i : s_module)
    {
        pair<int, double> temp = cost(i.second, s_module, h_module);
        overlap_count.insert(make_pair(temp.first, i.second));
        overlap_percent.insert(make_pair(i.second.name, temp.second));
        soft_block_loc[i.second.name] = make_pair(i.second.x, i.second.y);
        hardened_soft_block[i.second.name] = make_pair(i.second.w, i.second.h);
        locations.insert(make_pair(i.second.name, make_pair(i.second.x, i.second.y)));
        // cout << "Block name: " << i.second.name << ", x: " << i.second.x << ", y: " << i.second.y << ", overlap num: " << temp.first << ", overlap percent: " << temp.second << "%\n";
    }
    for(auto i : h_module)
        locations.insert(make_pair(i.second.name, make_pair(i.second.x, i.second.y)));

    if(choice)
    {
        for(auto i : overlap_count)
        {
            blocks.push_back(make_pair(i.first, i.second));
            shuffle(blocks.begin(), blocks.end(), gen);
        }
    }
    else
    {
        for(auto i : overlap_count)
            blocks.push_back(make_pair(i.first, i.second));
    }
    // cout << endl;
    // for(auto i : blocks)
    //     cout << "Block name: " << i.second.name << ", x: " << i.second.x << ", y: " << i.second.y << ", overlap num: " << i.first << endl;

    // for(auto i : overlap_percent)
    //      cout << "Block name: " << i.second.name << ", x: " << i.second.x << ", y: " << i.second.y << ", overlap percentage: " << i.first << "%\n";
    // cout << total_overlap_percentage << "%\n";
    // for(auto i : soft_block_loc)
    //     cout << "Block name: " << i.first << ", x: " << i.second.first << ", y: " << i.second.second << endl;
    // for(auto i : locations)
    //     cout << "Block name: " << i.first << ", x: " << i.second.first << ", y: " << i.second.second << endl;

    for(auto &i : blocks)
    {
        if(i.first != 0)
        {
            pair<int, int> force = calculate_resultant_force(i.second.name);
            bool clear = false;
            //cout << "Block name: " << i.second.name << ", x: " << i.second.x << ", y: " << i.second.y << ", force(x, y): (" << force.first << ", " << force.second << ")" << endl;

            if(force.first >= 0)
            {
                Module temp = i.second;
                temp.x += 10;
                pair<int, double> outcome = cost(temp, s_module, h_module);
                if(outcome.second < overlap_percent[i.second.name])
                {
                    //cout << "Right side clear" << endl;
                    int new_overlap_num = outcome.first;
                    double new_overlap_percentage = outcome.second;
                    clear = true;

                    for(int x = temp.x + 10; x < col - (s_module[i.second.name].w / 2 + s_module[i.second.name].w % 2); x += 10)
                    {
                        temp.x = x;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < new_overlap_percentage)
                        {
                            new_overlap_percentage = outcome.second;
                            //overlap_percent[i.second.name] = new_overlap_percentage;
                            soft_block_loc[i.second.name].first = x;
                            s_module[i.second.name].x = x;
                        }
                        else
                            break;
                    }

                    if(force.second >= 0)
                    {
                        temp.y += 10;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Up side clear" << endl;
                            new_overlap_num = outcome.first;
                            new_overlap_percentage = outcome.second;

                            for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y -= 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force up, down side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                    else
                    {
                        temp.y -= 10;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Down side clear" << endl;
                            new_overlap_num = outcome.first;
                            new_overlap_percentage = outcome.second;

                            for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y += 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "force down, up side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    temp.x -= 20;
                    outcome = cost(temp, s_module, h_module);
                    if(outcome.second < overlap_percent[i.second.name])
                    {
                        //cout << "Force right, left side clear" << endl;
                        int new_overlap_num = outcome.first;
                        double new_overlap_percentage = outcome.second;
                        clear = true;

                        for(int x = temp.x - 10; x > 0 + (s_module[i.second.name].w / 2); x -= 10)
                        {
                            temp.x = x;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < new_overlap_percentage)
                            {
                                new_overlap_percentage = outcome.second;
                                soft_block_loc[i.second.name].first = x;
                                s_module[i.second.name].x = x;
                            }
                            else
                                break;
                        }

                        if(force.second >= 0)
                        {
                            temp.y += 10;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Up side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                temp.y -= 20;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < overlap_percent[i.second.name])
                                {
                                    //cout << "Force up, down side clear" << endl;
                                    new_overlap_num = outcome.first;
                                    new_overlap_percentage = outcome.second;

                                    for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                    {
                                        temp.y = y;
                                        outcome = cost(temp, s_module, h_module);
                                        if(outcome.second < new_overlap_percentage)
                                        {
                                            new_overlap_percentage = outcome.second;
                                            soft_block_loc[i.second.name].second = y;
                                            s_module[i.second.name].y = y;
                                        }
                                        else
                                            break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            temp.y -= 10;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Down side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                temp.y += 20;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < overlap_percent[i.second.name])
                                {
                                    //cout << "Force down, up side clear" << endl;
                                    new_overlap_num = outcome.first;
                                    new_overlap_percentage = outcome.second;

                                    for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                    {
                                        temp.y = y;
                                        outcome = cost(temp, s_module, h_module);
                                        if(outcome.second < new_overlap_percentage)
                                        {
                                            new_overlap_percentage = outcome.second;
                                            soft_block_loc[i.second.name].second = y;
                                            s_module[i.second.name].y = y;
                                        }
                                        else
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(!clear)
                {
                    //cout << "First direction failed" << endl;
                    if(force.second >= 0)
                    {
                        temp.y += 10;
                        outcome = cost(temp, s_module, h_module);

                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Up side clear" << endl;
                            int new_overlap_num = outcome.first;
                            int new_overlap_percentage = outcome.second;

                            for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y -= 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force up, down side clear" << endl;
                                int new_overlap_num = outcome.first;
                                int new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                    else
                    {
                        temp.y -= 10;
                        outcome = cost(temp, s_module, h_module);

                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Down side clear" << endl;
                            int new_overlap_num = outcome.first;
                            int new_overlap_percentage = outcome.second;

                            for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y += 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force down, up side clear" << endl;
                                int new_overlap_num = outcome.first;
                                int new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                Module temp = i.second;
                temp.x -= 10;
                pair<int, double> outcome = cost(temp, s_module, h_module);
                if(outcome.second < overlap_percent[i.second.name])
                {
                    //cout << "Left side clear" << endl;
                    int new_overlap_num = outcome.first;
                    double new_overlap_percentage = outcome.second;
                    clear = true;

                    for(int x = temp.x - 10; x > 0 + (s_module[i.second.name].w / 2); x -= 10)
                    {
                        temp.x = x;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < new_overlap_percentage)
                        {
                            new_overlap_percentage = outcome.second;
                            soft_block_loc[i.second.name].first = x;
                            s_module[i.second.name].x = x;
                        }
                        else
                            break;
                    }

                    if(force.second >= 0)
                    {
                        temp.y += 10;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Up side clear" << endl;
                            new_overlap_num = outcome.first;
                            new_overlap_percentage = outcome.second;

                            for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y -= 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force up, down side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                    else
                    {
                        temp.y -= 10;
                        outcome = cost(temp, s_module, h_module);
                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Down side clear" << endl;
                            new_overlap_num = outcome.first;
                            new_overlap_percentage = outcome.second;

                            for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y += 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force down, up side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    temp.x += 20;
                    outcome = cost(temp, s_module, h_module);
                    if(outcome.second < overlap_percent[i.second.name])
                    {
                        //cout << "Force left, right side clear" << endl;
                        int new_overlap_num = outcome.first;
                        double new_overlap_percentage = outcome.second;

                        for(int x = temp.x + 10; x < col - (s_module[i.second.name].w / 2 + s_module[i.second.name].w % 2); x += 10)
                        {
                            temp.x = x;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < new_overlap_percentage)
                            {
                                new_overlap_percentage = outcome.second;
                                soft_block_loc[i.second.name].first = x;
                                s_module[i.second.name].x = x;
                            }
                            else
                                break;
                        }

                        if(force.second >= 0)
                        {
                            temp.y += 10;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Up side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                temp.y -= 20;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < overlap_percent[i.second.name])
                                {
                                    //cout << "Force up, down side clear" << endl;
                                    new_overlap_num = outcome.first;
                                    new_overlap_percentage = outcome.second;

                                    for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                    {
                                        temp.y = y;
                                        outcome = cost(temp, s_module, h_module);
                                        if(outcome.second < new_overlap_percentage)
                                        {
                                            new_overlap_percentage = outcome.second;
                                            soft_block_loc[i.second.name].second = y;
                                            s_module[i.second.name].y = y;
                                        }
                                        else
                                            break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            temp.y -= 10;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Down side clear" << endl;
                                new_overlap_num = outcome.first;
                                new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                temp.y += 20;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < overlap_percent[i.second.name])
                                {
                                    //cout << "Force down, up side clear" << endl;
                                    new_overlap_num = outcome.first;
                                    new_overlap_percentage = outcome.second;

                                    for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                    {
                                        temp.y = y;
                                        outcome = cost(temp, s_module, h_module);
                                        if(outcome.second < new_overlap_percentage)
                                        {
                                            new_overlap_percentage = outcome.second;
                                            soft_block_loc[i.second.name].second = y;
                                            s_module[i.second.name].y = y;
                                        }
                                        else
                                            break;
                                    }
                                }
                            }
                        }
                    }
                } 
                if(!clear)
                {
                    //cout << "First direction failed" << endl;
                    if(force.second >= 0)
                    {
                        temp.y += 10;
                        outcome = cost(temp, s_module, h_module);

                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Up side clear" << endl;
                            int new_overlap_num = outcome.first;
                            int new_overlap_percentage = outcome.second;

                            for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y -= 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force up, down side clear" << endl;
                                int new_overlap_num = outcome.first;
                                int new_overlap_percentage = outcome.second;

                                for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                    else
                    {
                        temp.y -= 10;
                        outcome = cost(temp, s_module, h_module);
                        
                        if(outcome.second < overlap_percent[i.second.name])
                        {
                            //cout << "Down side clear" << endl;
                            int new_overlap_num = outcome.first;
                            int new_overlap_percentage = outcome.second;

                            for(int y = temp.y - 10; y > 0 + (s_module[i.second.name].h / 2); y -= 10)
                            {
                                temp.y = y;
                                outcome = cost(temp, s_module, h_module);
                                if(outcome.second < new_overlap_percentage)
                                {
                                    new_overlap_percentage = outcome.second;
                                    soft_block_loc[i.second.name].second = y;
                                    s_module[i.second.name].y = y;
                                }
                                else
                                    break;
                            }
                        }
                        else
                        {
                            temp.y += 20;
                            outcome = cost(temp, s_module, h_module);
                            if(outcome.second < overlap_percent[i.second.name])
                            {
                                //cout << "Force down, up side clear" << endl;
                                int new_overlap_num = outcome.first;
                                int new_overlap_percentage = outcome.second;

                                for(int y = temp.y + 10; y < row - (s_module[i.second.name].h / 2 + s_module[i.second.name].h % 2); y += 10)
                                {
                                    temp.y = y;
                                    outcome = cost(temp, s_module, h_module);
                                    if(outcome.second < new_overlap_percentage)
                                    {
                                        new_overlap_percentage = outcome.second;
                                        soft_block_loc[i.second.name].second = y;
                                        s_module[i.second.name].y = y;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                }  
            }
        }
    }

    // for(auto i : soft_block_loc)
    //     cout << "Block name: " << i.first << ", x: " << i.second.first << ", y: " << i.second.second << endl;
    // cout << "\n";
    // for(auto i : s_module)
    //     cout << "Block name: " << i.second.name << ", x: " << i.second.x << ", y: " << i.second.y << endl;
}

void move(const string &name, const pair<int, int> &force)
{

}

pair<int, int> calculate_resultant_force(const string &name)
{
    long long int x = 0, y = 0;
    for(auto i : connection)
    {
        if(i.second.first == name || i.second.second == name)
        {
            if(i.second.first == name)
            {
                if(mid_fixed.find(i.second.second) != mid_fixed.end())
                {
                    x += (mid_fixed[i.second.second].first - soft_block_loc[name].first) * i.first;
                    y += (mid_fixed[i.second.second].second - soft_block_loc[name].second) * i.first;
                }
                else
                {
                    x += (soft_block_loc[i.second.second].first - soft_block_loc[name].first) * i.first;
                    y += (soft_block_loc[i.second.second].second - soft_block_loc[name].second) * i.first;
                }
            }
            else
            {
                if(mid_fixed.find(i.second.first) != mid_fixed.end())
                {
                    x += (mid_fixed[i.second.first].first - soft_block_loc[name].first) * i.first;
                    y += (mid_fixed[i.second.first].second - soft_block_loc[name].second) * i.first;
                }
                else
                {
                    x += (soft_block_loc[i.second.first].first - soft_block_loc[name].first) * i.first;
                    y += (soft_block_loc[i.second.first].second - soft_block_loc[name].second) * i.first;
                }
            }
        }
    }
    if(x > 0)
    {
        if(y > 0)
            return make_pair(1, 1);
        else if(y < 0)
            return make_pair(1, -1);
        else
            return make_pair(1, 0);
    }
    else if(x < 0)
    {
        if(y > 0)
            return make_pair(-1, 1);
        else if(y < 0)
            return make_pair(-1, -1);
        else
            return make_pair(-1, 0);
    }
    else
    {
        if(y > 0)
            return make_pair(0, 1);
        else if(y < 0)
            return make_pair(0, -1);
        else
            return make_pair(0, 0);
    }
}

bool find_if_occupied2(const string &name, const pair<int, int> &loc, map<string, pair<int, int>> &locations)
{
    bool occupied = false;

    for(auto i : locations)
    {
        if(name != i.first)
        {
            if(mid_fixed.find(i.first) != mid_fixed.end())
            {
                int x1 = locations[i.first].first - double(get<2>(fixed_block[i.first]) / 2), x2 = locations[i.first].first + double(get<2>(fixed_block[i.first]) / 2) + 1,
                    y1 = locations[i.first].second - double(get<3>(fixed_block[i.first]) / 2), y2 = locations[i.first].second + double(get<3>(fixed_block[i.first]) / 2) + 1,
                    loc_x1 = loc.first - double(hardened_soft_block[name].first / 2), loc_x2 = loc.first + double(hardened_soft_block[name].first / 2) + 1,
                    loc_y1 = loc.second - double(hardened_soft_block[name].second / 2), loc_y2 = loc.second + double(hardened_soft_block[name].second / 2) + 1;
                // cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                // cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                // cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
                if((((((loc_x1 < x2 - get<2>(fixed_block[i.first]) / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + get<2>(fixed_block[i.first]) / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + get<3>(fixed_block[i.first]) / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - get<3>(fixed_block[i.first]) / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))))
                {
                    occupied = true;
                    break;
                }
            }
            else
            {
                int x1 = locations[i.first].first - double(hardened_soft_block[i.first].first / 2), x2 = locations[i.first].first + double(hardened_soft_block[i.first].first / 2) + 1,
                    y1 = locations[i.first].second - double(hardened_soft_block[i.first].second / 2), y2 = locations[i.first].second + double(hardened_soft_block[i.first].second / 2) + 1,
                    loc_x1 = loc.first - double(hardened_soft_block[name].first / 2), loc_x2 = loc.first + double(hardened_soft_block[name].first / 2) + 1,
                    loc_y1 = loc.second - double(hardened_soft_block[name].second / 2), loc_y2 = loc.second + double(hardened_soft_block[name].second / 2) + 1;
                // cout << "Block name: " << i.first << ", location x: " << get<0>(locations[i.first]) << ", y: " << get<1>(locations[i.first]) << endl;
                // cout << "x1 ~ y2: " << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
                // cout << "loc_x1 ~ loc_y2: " << loc_x1 << ", " << loc_x2 << ", " << loc_y1 << ", " << loc_y2 << endl;
                // cout << "Original block width / 6: " << hardened_soft_block[i.first].first / 6 << ", new block(loc) width / 6: " << hardened_soft_block[name].first / 6 << endl;
                if((((((loc_x1 < x2 - hardened_soft_block[i.first].first / 6) || (loc_x1 < x2 - hardened_soft_block[name].first / 6)) && (loc_x1 > x1)) || (((loc_x2 > x1 + hardened_soft_block[i.first].first / 6) || (loc_x2 > x1 + hardened_soft_block[name].first / 6)) && (loc_x2 < x2)) || ((loc_x1 <= x1) && (loc_x2 >= x2))) && ((((loc_y2 > y1 + hardened_soft_block[i.first].second / 6) || (loc_y2 > y1 + hardened_soft_block[name].second / 6)) && (loc_y2 < y2)) || (((loc_y1 < y2 - hardened_soft_block[i.first].second / 6) || (loc_y1 < y2 - hardened_soft_block[name].second / 6)) && (loc_y1 > y1)) || ((loc_y1 <= y1) && (loc_y2 >= y2)))))
                {
                    occupied = true;
                    break;
                }
            }
        }
    }
    return occupied;
}

int calculate_wirelength()
{
    int total_wirelength = 0;
    for(auto i : connection)
    {
        if(mid_fixed.find(i.second.first) != mid_fixed.end())
        {
            if(mid_fixed.find(i.second.second) != mid_fixed.end())
                total_wirelength += abs(mid_fixed[i.second.second].first - mid_fixed[i.second.first].first) + abs(mid_fixed[i.second.second].second - mid_fixed[i.second.first].second);
            else
                total_wirelength += abs(soft_block_loc[i.second.second].first - mid_fixed[i.second.first].first) + abs(soft_block_loc[i.second.second].second - mid_fixed[i.second.first].second);
        }
        else
        {
            if(mid_fixed.find(i.second.second) != mid_fixed.end())
                total_wirelength += abs(mid_fixed[i.second.second].first - soft_block_loc[i.second.first].first) + abs(mid_fixed[i.second.second].second - soft_block_loc[i.second.first].second);
            else
                total_wirelength += abs(soft_block_loc[i.second.second].first - soft_block_loc[i.second.first].first) + abs(soft_block_loc[i.second.second].second - soft_block_loc[i.second.first].second);
        }
    }
    return total_wirelength;
}

void plot(ofstream &fout)
{
    //prefix
    fout << "\nset title \" gp.plt, block= " << soft_num + fixed_num << ", net= " << connection_num << " \" font \"Times, 22\"\n\nset size ratio 1\nset nokey\n\nplot[:][:] '-' w l lt 3, '-' w l lt 4, '-' w l lt 1, '-' w l lt 7, '-' w l lt 5, '-' w l lt 2\n\n\n";

    //core region
    fout << "# core region\n0, 0\n" << col << ", 0\n" << col << ", " << row << "\n0, " << row << "\n0, 0\n\n\n";

    //die area
    fout << "# die area\n0, 0\n0, 0\n0, 0\n0, 0\n0, 0\n\n\n";

    //blocks
    fout << "# blocks\n0, 0\n\n";
    for(auto i : soft_block_loc)
    {
        fout << i.second.first - (hardened_soft_block[i.first].first / 2) << ", " << i.second.second - (hardened_soft_block[i.first].second / 2) << "\n" <<
                i.second.first + (hardened_soft_block[i.first].first / 2) + (hardened_soft_block[i.first].first % 2) << ", " << i.second.second - (hardened_soft_block[i.first].second / 2) << "\n" <<
                i.second.first + (hardened_soft_block[i.first].first / 2) + (hardened_soft_block[i.first].first % 2) << ", " << i.second.second + (hardened_soft_block[i.first].second / 2) + (hardened_soft_block[i.first].second % 2) << "\n" <<
                i.second.first - (hardened_soft_block[i.first].first / 2) << ", " << i.second.second + (hardened_soft_block[i.first].second / 2) + (hardened_soft_block[i.first].second % 2) << "\n" <<
                i.second.first - (hardened_soft_block[i.first].first / 2) << ", " << i.second.second - (hardened_soft_block[i.first].second / 2) << "\n\n";

        //cout << i.first << ", size: " << abs((i.second.first - double(hardened_soft_block[i.first].first / 2)) - (i.second.first + double(hardened_soft_block[i.first].first / 2) + 1)) * abs((i.second.second - double(hardened_soft_block[i.first].second / 2)) - (i.second.second + double(hardened_soft_block[i.first].second / 2) + 1)) << endl;
    }
    fout << "\nEOF\n\n\n";

    //fixed block
    fout << "# fixed blocks\n0, 0\n\n";
    for(auto i : fixed_block)
    {
        fout << get<0>(i.second) << ", " << get<1>(i.second) << "\n" <<
                get<0>(i.second) + get<2>(i.second) << ", " << get<1>(i.second) << "\n" <<
                get<0>(i.second) + get<2>(i.second) << ", " << get<1>(i.second) + get<3>(i.second) << "\n" <<
                get<0>(i.second) << ", " << get<1>(i.second) +get<3>(i.second) << "\n" <<
                get<0>(i.second) << ", " << get<1>(i.second) << "\n\n";
    }
    fout << "\nEOF\n\n\n";

    //cell move
    fout << "# cell move\n0, 0\n\n\nEOF\n\n\n";

    //nets
    fout << "# nets\n0, 0\n\n\nEOF\n\n\n";

    //dummy modules
    fout << "# dummy modules\n0, 0\n\n\nEOF\n\n\n";

    //pins
    fout << "# pins\n0, 0\n\n\nEOF\n\n";

    fout << "pause -1 \'Press any key\'";
}
