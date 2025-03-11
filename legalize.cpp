#include "legalize.h"
using namespace std;

int cal_overlapping(Module a, Module b)
{
    int x_lim = 0, y_lim = 0, x_o = abs(a.x - b.x), y_o = abs(a.y - b.y);
    if(a.x < b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h;
        }
    }
    else if (a.x > b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h;
        }
    }
    else 
    {
        if(a.y < b.y)
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        
    }

    if(x_o < x_lim) 
    {
        if(x_lim - x_o > a.w)
        {
            x_o = a.w;
        }
        else if (x_lim - x_o > b.w)
        {
            x_o = b.w;
        }
        else
        {
            x_o = x_lim - x_o;
        }
    }
    else 
    {
        x_o = 0;
    }

    if(y_o < y_lim) 
    {
        if(y_lim - y_o > a.h)
        {
            y_o = a.h;
        }
        else if (y_lim - y_o > b.h)
        {
            y_o = b.h;
        }
        else
        {
            y_o = y_lim - y_o;
        }
    }
    else 
    {
        y_o = 0;
    }
    return x_o * y_o;
}

double tot_overlapping(int row, int col, map<string, Module> s_module, map<string, Module> h_module) {
    int total = 0;
    for(auto i : s_module) 
    {
        for(auto j : s_module)
        {
            if(j.first != i.first) 
            {    
                total += cal_overlapping(i.second, j.second);
            }
        }
    }

    for(auto i : s_module) 
    {
        for(auto j : h_module)
        {
            total += cal_overlapping(i.second, j.second);
        }
    }
    return (total / double(row * col)) * 100;
}

pair<int, int> tot_overlapping_count(map<string, Module> s_module, map<string, Module> h_module) {
    int s_num = 0, h_num = 0;
    for(auto i : s_module) 
    {
        for(auto j : s_module)
        {
            if(j.first != i.first && cal_overlapping(i.second, j.second)) 
            {    
                s_num++;
            }
        }
    }

    for(auto i : s_module) 
    {
        for(auto j : h_module)
        {
            if(cal_overlapping(i.second, j.second)) 
            {    
                h_num++;
            }
        }
    }
    return make_pair(s_num / 2, h_num);
}

pair<int, double> cost(Module a, map<string, Module> s_module, map<string, Module> h_module)
{
    int total = 0, num = 0;
    for(auto i : s_module) 
    {
        if(a.name != i.first) 
        {    
            if(cal_overlapping(i.second, a))
            {
                total += cal_overlapping(i.second, a);
                num++;
            }
        }
    }

    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            total += cal_overlapping(i.second, a);
            num++;
        }
    }
    return make_pair(num, (total / double(a.w * a.h)) * 100);
}

pair<int, int> cost2(Module a, map<string, Module> s_module, map<string, Module> h_module)
{
    int s_total = 0, h_total = 0;
    for(auto i : s_module) 
    {
        if(a.name != i.first) 
        {    
            if(cal_overlapping(i.second, a))
            {
                s_total += cal_overlapping(i.second, a);
            }
        }
    }

    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            h_total += cal_overlapping(i.second, a);
        }
    }
    return make_pair(s_total, h_total);
}

int sum_of_cost2(Module a, map<string, Module> s_module, map<string, Module> h_module) {
    return cost2(a, s_module, h_module).first + cost2(a, s_module, h_module).second;
}

pair<int, int> cost_count(Module a, map<string, Module> s_module, map<string, Module> h_module) {
    int s = 0, h = 0;
    for(auto i : s_module) 
    {
        if(a.name != i.first) 
        {    
            if(cal_overlapping(i.second, a))
            {
                s++;
                // cout << i.second.name << endl;
            }
        }
    }

    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            h++;
            // cout << i.second.name << endl;
        }
    }
    return make_pair(s, h);
}

pair<int, int> dir_decide(char a) {
    if (a == 'l') {
        return make_pair(-1, 0);
    }
    else if (a == 'r') {
        return make_pair(1, 0);
    }
    else if (a == 'd') {
        return make_pair(0, -1);
    }
    else if (a == 'u') {
        return make_pair(0, 1);
    }
    else {
        return make_pair(0, 0);
    }
}

Module move(Module a, int x, int y) {
    Module b(a.name, a.x + x, a.y + y, a.w, a.h, a.area, a.overlapping, a.identity);
    return b;
}

pair<bool, int> if_hdir_increase(Module a, int x, int y, map<string, Module> h_module) { //丟入未動過的a
    vector<Module> name = overlap_with_h(move(a, x, y), h_module);
    bool flag = 0;
    int value = 0;
    for(auto i : name) {
        int v = cal_overlapping(move(a, x, y), i) - cal_overlapping(a, i);
        if(v > 0) {
            flag = 1;
        }
        value += v;
    }
    return make_pair(flag, value);
}
pair<bool, int> if_sdir_increase(Module a, int x, int y, map<string, Module> s_module) {
    vector<Module> name = overlap_with_s(move(a, x, y), s_module);
    //cout << "num of over_s:" << name.size() << endl;
    bool flag = 0;
    int value = 0;
    for(auto i : name) {
        int v = cal_overlapping(move(a, x, y), i) - cal_overlapping(a, i);
        if(v > 0) {
            flag = 1;
        }
        value += v;
    }
    //cout << x << " " << y << " " << "if_sdir_increase" << " " << flag << endl;
    return make_pair(flag, value);
}

bool boundary_check(Module a, int c_x, int c_y) { //outside is true
    if((a.y + (a.h / 2) + (a.h % 2)) > c_y) {
        //cout << "UP OUT" << endl;
        return true;
    }
    else if((a.y - (a.h / 2)) < 0) {
        //cout << "DOWN OUT" << endl;
        return true;
    }
    else if((a.x + (a.w / 2) + (a.w % 2)) > c_x) {
        //cout << "RIGHT OUT" << endl;
        return true;
    }
    else if((a.x - (a.w / 2)) < 0) {
        //cout << "LEFT OUT" << endl;
        return true;
    }
    return false;
}

bool all_boundary_check(map<string, Module> s_module, int c_x, int c_y) { //outside is true
    for(auto a : s_module)   
        if((a.second.y + (a.second.h / 2) + (a.second.h % 2)) > c_y) {
            //cout << "UP OUT" << endl;
            return true;
        }
        else if((a.second.y - (a.second.h / 2)) < 0) {
            //cout << "DOWN OUT" << endl;
            return true;
        }
        else if((a.second.x + (a.second.w / 2) + (a.second.w % 2)) > c_x) {
            //cout << "RIGHT OUT" << endl;
            return true;
        }
        else if((a.second.x - (a.second.w / 2)) < 0) {
            //cout << "LEFT OUT" << endl;
            return true;
        }
    return false;
}

bool should_cancel(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y, int x, int y, int offset) {
    int now_h = cost_count(move(a, x*offset, y*offset), s_module, h_module).second;
    a.x += x;
    a.y += y;
    while(1) {
        // if(cost(a, s_module, h_module).second > 10.0) {
        //     return true;
        // }
        if ((cost_count(move(a, x, y), s_module, h_module).second != now_h) || boundary_check(a, c_x, c_y)) {
            
            double temp = cost(a, s_module, h_module).second;
            //cout << x << "should_cancel " << a.name << ':' << boundary_check(a, c_x, c_y) << " " << temp << " " << now_h << " " << cost_count(move(a, x, y), s_module, h_module).second << endl;
            if(temp > 10.0) {
                return true; //should
            }
            else {
                return false;  //should not
            }
        }
        a.x += x;
        a.y += y;
    }
    //cout << "should_cancel false" << endl;
    return false;
}

string move_dir(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y) {
    string cord;
    int offset = 2;
    int s_over = cost_count(a, s_module, h_module).first, h_over = cost_count(a, s_module, h_module).second;
    //歸類
    if (a.x < (c_x / 2 + c_x % 2) ) {
        cord.push_back('l');
    }
    else if (a.x > (c_x / 2 + c_x % 2) ) {
        cord.push_back('r');
    }
    if (a.y < (c_y / 2 + c_y % 2) ) {
        cord.push_back('d');
    }
    else if (a.y > (c_y / 2 + c_y % 2) ) {
        cord.push_back('u');
    }
    //區分象限(即 cord.size() = 0 or 1 or 2)
    if (cord.size() == 0) {  //正中間 -> 不動
        return "nn";
    }
    else if(cord.size() == 1) {  //軸上 ->移開
        if (cord == "l" ||  cord == "r") {
            return "cu";
        }
        else{
            return "cr";
        }
    }
    else {  
        if (s_over + h_over == 0) {  //free -> cord
            // return cord;
            //cout << "free" << endl;
            vector<int> dirx{dir_decide(cord[0]).first, dir_decide(cord[1]).first}, diry{dir_decide(cord[0]).second, dir_decide(cord[1]).second}, flag{1, 1};
            for(int i = 0; i < 2; i++) {  //check if touch h
                if (cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second && should_cancel(a, s_module, h_module, c_x, c_y, dirx[i], diry[i], offset)) {
                    flag[i] = 0;
                }
            }
            for(int i = 0; i < 2; i++) {  //check if touched s # increase
                if (cost_count(move(a, dirx[i], diry[i]), s_module, h_module).first && if_sdir_increase(a, dirx[i], diry[i], s_module).first) {
                    flag[i] = 0;
                }
            }
            //cout << "flag : " << flag[0] << " " << flag[1] << endl;
            if(flag[0] != flag[1]) {
                string temp = "nn";
                if(flag[0]) {
                    temp[0] = cord[0];
                    return temp;
                }
                else {
                    temp[0] = cord[1];
                    return temp;
                }
            }
            else {
                string temp = "nn";
                if (flag[0] * flag[1]) {
                    if(cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module).second < cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second) {
                        temp[0] = cord[0];
                        if (cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second <= cost(a, s_module, h_module).second) {
                            temp[1] = cord[1];
                        }
                        return temp;
                    }
                    else {
                        temp[0] = cord[1];
                        if (cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module) <= cost(a, s_module, h_module)) {
                            temp[1] = cord[0];
                        }
                        return temp;
                    }
                }
                else {
                    return "nn";
                }
            }
        }
        else if (s_over != 0 && h_over == 0) {  //only overlap with s
            //cout << "only s" << endl;
            vector<int> dirx{dir_decide(cord[0]).first, dir_decide(cord[1]).first}, diry{dir_decide(cord[0]).second, dir_decide(cord[1]).second}, flag{1, 1};
            for(int i = 0; i < 2; i++) {  //check if touch h
                if (cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second && should_cancel(a, s_module, h_module, c_x, c_y, dirx[i], diry[i], offset)) {
                    flag[i] = 0;
                }
            }
            for(int i = 0; i < 2; i++) {  //check if touched s # increase
                if ((cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).first > cost_count(a, s_module, h_module).first) && if_sdir_increase(a, dirx[i], diry[i], s_module).first) {
                    flag[i] = 0;
                }
            }
            if(flag[0] != flag[1]) {
                string temp = "nn";
                if(flag[0]) {
                    temp[0] = cord[0];
                    return temp;
                }
                else {
                    temp[0] = cord[1];
                    return temp;
                }
            }
            else {
                string temp = "nn";
                if (flag[0] * flag[1]) {
                    if(cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module).second < cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second) {
                        temp[0] = cord[0];
                        if (cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second <= cost(a, s_module, h_module).second) {
                            temp[1] = cord[1];
                        }
                        return temp;
                    }
                    else {
                        temp[0] = cord[1];
                        if (cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module) <= cost(a, s_module, h_module)) {
                            temp[1] = cord[0];
                        }
                        return temp;
                    }
                }
                else {
                    return "nn";
                }
            }
        }
        else if(s_over == 0 && h_over != 0) {  //only overlap with h
        //cout << "only h" << endl;
            vector<int> dirx{dir_decide(cord[0]).first, dir_decide(cord[1]).first}, diry{dir_decide(cord[0]).second, dir_decide(cord[1]).second}, flag{1, 1};
            for(int i = 0; i < 2; i++) {  //check if touched s # increase
                if (cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).first > cost_count(a, s_module, h_module).first) {
                    flag[i] = 0;
                }
            }
            for(int i = 0; i < 2; i++) {  //check if touched h # increase
                if (cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second > cost_count(a, s_module, h_module).second && should_cancel(a, s_module, h_module, c_x, c_y, dirx[i], diry[i], offset)) {
                    flag[i] = 0;
                }
            }


            vector<int> change{0, 0};  //cal h dir's overlapping value change
            for(int i = 0; i < 2; i++) {
                //cout << "本" << cost2(a, s_module, h_module).second << endl;
                if (cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second > cost2(a, s_module, h_module).second) {
                    change[i] = cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second - cost2(a, s_module, h_module).second;
                }
                //cout << "cost2 " << i << " : " << cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second << endl;
            }
            //cout << "change : " << change[0] << " " << change[1] << endl;
            //cout << "flag : " << flag[0] << " " << flag[1] << endl;
            if(change[0] != change[1] && (change[0] * change[1] == 0) ) {
                if(!change[0]) {
                    // if(cost(move(a, dirx[1], diry[1]), s_module, h_module).second > 10) {
                    //     flag[1] = 0;
                    // }
                    flag[1] = 0;
                }
                else {
                    // if(cost(move(a, dirx[0], diry[0]), s_module, h_module).second > 10) {
                    //     flag[0] = 0;
                    // }
                    flag[0] = 0;
                }
            }
            else {
                if(change[0] * change[1]) {
                    if(change[0] > change[1]) {
                        flag[0] = 0;
                    }
                    else {
                        flag[1] = 0;
                    }
                }
                // else {
                //     if(cost(move(a, dirx[0], diry[0]), s_module, h_module).second > 10) {
                //         flag[0] = 0;
                //     }
                //     if(cost(move(a, dirx[1], diry[1]), s_module, h_module).second > 10) {
                //         flag[1] = 0;
                //     }
                // }
            }


            if(flag[0] != flag[1]) {
                string temp = "nn";
                if(flag[0]) {
                    temp[0] = cord[0];
                    return temp;
                }
                else {
                    temp[0] = cord[1];
                    return temp;
                }
            }
            else {
                string temp = "nn";
                if (flag[0] * flag[1]) {
                    if(cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module).second < cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second) {
                        temp[0] = cord[0];
                        if (cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second <= cost(a, s_module, h_module).second) {
                            temp[1] = cord[1];
                        }
                        return temp;
                    }
                    else {
                        temp[0] = cord[1];
                        if (cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module) <= cost(a, s_module, h_module)) {
                            temp[1] = cord[0];
                        }
                        return temp;
                    }
                }
                else {
                    return "nn";
                }
            }
        }
        else {  //touch with s and h
            vector<int> dirx{dir_decide(cord[0]).first, dir_decide(cord[1]).first}, diry{dir_decide(cord[0]).second, dir_decide(cord[1]).second}, flag{1, 1};
            for(int i = 0; i < 2; i++) {  //check if touched s # increase
                if ((cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).first > cost_count(a, s_module, h_module).first) && if_sdir_increase(a, dirx[i], diry[i], s_module).first) {
                    flag[i] = 0;
                }
            }
            for(int i = 0; i < 2; i++) {  //check if touched h # increase
                if (cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second > cost_count(a, s_module, h_module).second && should_cancel(a, s_module, h_module, c_x, c_y, dirx[i], diry[i], offset)) {
                    flag[i] = 0;
                }
            }


            vector<int> change{0, 0};  //cal h dir's overlapping value change
            for(int i = 0; i < 2; i++) {
                //cout << "本" << cost2(a, s_module, h_module).second << endl;
                if (cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second > cost2(a, s_module, h_module).second) {
                    change[i] = cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second - cost2(a, s_module, h_module).second;
                }
                //cout << "cost2 " << i << " : " << cost2(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).second << endl;
            }
            //cout << "change : " << change[0] << " " << change[1] << endl;
            //cout << "flag : " << flag[0] << " " << flag[1] << endl;
            if(change[0] != change[1] && (change[0] * change[1] == 0)) {
                if(!change[0]) {
                    // if(cost(move(a, dirx[1], diry[1]), s_module, h_module).second > 10) {
                    //     flag[1] = 0;
                    // }
                    flag[1] = 0;
                }
                else {
                    // if(cost(move(a, dirx[0], diry[0]), s_module, h_module).second > 10) {
                    //     flag[0] = 0;
                    // }
                    flag[0] = 0;
                }
            }
            else {
                if(change[0] * change[1]) {
                    if(change[0] > change[1]) {
                        flag[0] = 0;
                    }
                    else {
                        flag[1] = 0;
                    }
                }
                // else {
                //     if(cost(move(a, dirx[0], diry[0]), s_module, h_module).second > 10) {
                //         flag[0] = 0;
                //     }
                //     if(cost(move(a, dirx[1], diry[1]), s_module, h_module).second > 10) {
                //         flag[1] = 0;
                //     }
                // }
            }


            if(flag[0] != flag[1]) {
                string temp = "nn";
                if(flag[0]) {
                    temp[0] = cord[0];
                    return temp;
                }
                else {
                    temp[0] = cord[1];
                    return temp;
                }
            }
            else {
                string temp = "nn";
                if (flag[0] * flag[1]) {
                    if(cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module).second < cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second) {
                        temp[0] = cord[0];
                        if (cost(move(a, dirx[1]*offset, diry[1]*offset), s_module, h_module).second <= cost(a, s_module, h_module).second) {
                            temp[1] = cord[1];
                        }
                        return temp;
                    }
                    else {
                        temp[0] = cord[1];
                        if (cost(move(a, dirx[0]*offset, diry[0]*offset), s_module, h_module) <= cost(a, s_module, h_module)) {
                            temp[1] = cord[0];
                        }
                        return temp;
                    }
                }
                else {
                    return "nn";
                }
            }
        }
    }


}

vector<Module> overlap_with_h(Module a, map<string, Module> h_module) {
    vector<Module> name;
    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            name.push_back(i.second);
        }
    }
    return name;
}

vector<Module> overlap_with_s(Module a, map<string, Module> s_module) {
    vector<Module> name;
    for(auto i : s_module) 
    {
        if(a.name != i.first) 
        {    
            if(cal_overlapping(i.second, a))
            {
                name.push_back(i.second);
            }
        }
    }
    return name;
}

vector<Module> who_overlap(map<string, Module> s_module, map<string, Module> h_module) {
    vector<Module> name;
    map<string, Module> temp;
    for(auto i : s_module){
        for(auto j : s_module)
        {
            if(j.first != i.first && cal_overlapping(i.second, j.second)) 
            {    
                
                temp[j.first] = j.second;
            }
        }
    }
    for(auto i : s_module) 
    {
        for(auto j : h_module)
        {
            if(cal_overlapping(i.second, j.second)) 
            {    
                
                temp[i.first] = i.second;
            }
        }
    }
    for(auto i : temp) {
        name.push_back((i.second));
    } 
    return name;
}

bool check_area_limit(int num, Module a, map<string, Module> s_module, map<string, Module> h_module) {
    vector<Module> name = overlap_with_s(a, s_module);
    name.push_back(a);
    for(auto i : name){
        if(cost(i, s_module, h_module).second >= num) {
            return false;
        }
    }
    return true;
}

// void clear(Module a, map<string, Module> s_module, map<string, Module> h_module, int type);//0:all、1:no up、2:no down、3:no left、4:no right 
// void push(Module a, map<string, Module> s_module, map<string, Module> h_module, int posi);
// int relative_position(Module a, Module b);//1:up、2:down、3:left、4:right

bool rule_check(Module a, map<string, Module> s_module, map<string, Module> h_module) {
    if(double(a.h * a.w) * 0.8 > a.area) {
        //cout << a.name << " area constraint false" << endl;
        return false;
    }
    if((double(a.h) / double(a.w) >= 2.0) || (double(a.h) / double(a.w) <= 0.5)) {
        //cout << a.name << " aspect ratio constraint false" << endl;
        return false;
    }
    return true;
}

void cord_move(Module &a, int x, int y) {
    if(y == 1 && a.h % 2 == 1) {a.y += 1;}
    else if(y == -1 && a.h % 2 == 0) {a.y += -1;}
    else if(x == 1 && a.w % 2 == 1) {a.x += 1;}
    else if(x == -1 && a.w % 2 == 0) {a.x += -1;}
}

Module wh_resize_with_cord(Module a, int x, int y) {
    int nx = 0, ny = 0;
    if(y == 1 && a.h % 2 == 1) {ny = 1;}
    else if(y == -1 && a.h % 2 == 0) {ny = -1;}
    else if(x == 1 && a.w % 2 == 1) {nx = 1;}
    else if(x == -1 && a.w % 2 == 0) {nx = -1;}
    // cout << "nx, ny :" << nx << ", " << ny << endl;
    Module b(a.name, a.x + nx, a.y + ny, a.w += abs(x), a.h += abs(y), a.area, a.overlapping, a.identity);
    // cord_move(b, x, y);
    return b;
}

bool should_cancel_h(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y, int x, int y, int offset) {
    if(boundary_check(wh_resize_with_cord(a, x, y), c_x, c_y)) {
        return true;
    }
    int now_h = cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).second, now_s = cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).first;
    //cout << "now_h :" << now_h << ", now_s :" << now_s << endl;
    a.w += abs(x);
    a.h += abs(y);
    cord_move(a, x, y);
    while(1) {
        //cout << "@ ";
        if(((a.w * a.h) - cost2(a, s_module, h_module).second - cost2(a, s_module, h_module).first) >= a.area) {
            //cout << "area sat in should cancel" << endl;
            return false;
        }
        else if(!rule_check(a, s_module, h_module)) {
            //cout << endl;
            return true;
        }
        // if(cost(a, s_module, h_module).second > 10.0) {
        //     return true;
        // }
        if ((cost_count(move(a, x, y), s_module, h_module).first) != now_s) {
            //cout << "touch s in should cancel" << endl;
            double temp = cost(a, s_module, h_module).second;
            if(temp > 18.0) {
                //cout << endl;
                return true; //should
            }
            //cout << endl;
            return false;
        }
        if ((cost_count(move(a, x, y), s_module, h_module).second != now_h) || boundary_check(a, c_x, c_y) ) {
            //cout << "touch h or bound_check in should cancel" << endl;
            double temp = cost(a, s_module, h_module).second;
            //cout << x << "should_cancel " << a.name << ':' << boundary_check(a, c_x, c_y) << " " << temp << " " << now_h << " " << cost_count(move(a, x, y), s_module, h_module).second << endl;
            if(temp > 18.0) {
                //cout << endl;
                return true; //should
            }
            //cout << endl;
            return false;
        }
        a.w += abs(x);
        a.h += abs(y);
        cord_move(a, x, y);
    }
    //cout << "should_cancel false" << endl;
    //cout << endl;
    return false;
}

string dir_choose_for_h(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y) {
    vector<pair<int, int> > dir{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    vector<char> dir_n{'u', 'd', 'l', 'r'};
    string out = "";
    for(int i = 0; i < 4; i++) {
        //cout << "dir_choose : " << dir_n[i] << endl;
        int flag = 1;
        if(cost_count(move(a, dir[i].first, dir[i].second), s_module, h_module).first > cost_count(a, s_module, h_module).first) { //check if over_s # increase
            //cout << "s # increase、 ";
            flag = 0;
        }
        // if(cost_count(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost_count(a, s_module, h_module).second) { //check if over_h # increase
        //     cout << "h # increase、 ";
        //     flag = 0;
        // }
        if(cost2(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost2(a, s_module, h_module).second) {  //check if over_h value increase with should_cancel
            //cout << "over_h value increase、 ";
            if(should_cancel_h(a, s_module, h_module, c_x, c_y, dir[i].first, dir[i].second, 1)) {
                //cout << "should_cancel、 ";
                flag = 0;
            }
        }
        else {
            if(if_hdir_increase(a, dir[i].first, dir[i].second, h_module).first) {
                //cout << "hdir increase、 ";
                flag = 0;
            }
        }
        if(boundary_check(move(a, dir[i].first, dir[i].second), c_x, c_y)) {
            //cout << "boundary_check、 ";
            flag = 0;
        }
        if(flag){
            out.push_back(dir_n[i]);
        }
        //cout << endl;
    }
    return out;
}

string dir_choose_for_h2(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y) {
    vector<pair<int, int> > dir{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    vector<char> dir_n{'u', 'd', 'l', 'r'};
    vector<bool> dir_flag{1, 1, 1, 1}; 
    string out = "";
    for(int i = 0; i < 4; i++) {
        // if(!dir_flag[i]) {
        //     continue;
        // }
        //cout << "dir_choose : " << dir_n[i] << endl;
        int flag = 1;
        if(cost2(move(a, dir[i].first, dir[i].second), s_module, h_module).first > cost2(a, s_module, h_module).first) { //check if over_s value increase
            //cout << "s value increase、 ";
            flag = 0;
        }
        // if(cost_count(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost_count(a, s_module, h_module).second) { //check if over_h # increase
        //     cout << "h # increase、 ";
        //     flag = 0;
        // }
        if(cost2(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost2(a, s_module, h_module).second) {  //check if over_h value increase with should_cancel
            //cout << "over_h value increase、 ";
            if(should_cancel_h(a, s_module, h_module, c_x, c_y, dir[i].first, dir[i].second, 1)) {
                //cout << "should_cancel、 ";
                flag = 0;
            }
        }
        else {
            if(if_hdir_increase(a, dir[i].first, dir[i].second, h_module).first) {
                //cout << "hdir increase、 ";
                flag = 0;
            }
        }
        if(boundary_check(move(a, dir[i].first, dir[i].second), c_x, c_y)) {
            //cout << "boundary_check、 ";
            flag = 0;
        }
        if(flag){
            out.push_back(dir_n[i]);
            dir_flag[i] = 0;
        }
        //cout << endl;
    }
    for(int i = 0; i < 4; i++) {
        if(!dir_flag[i]) {
            continue;
        }
        //cout << "dir_choose 2 : " << dir_n[i] << endl;
        int flag = 1;
        if(cost_count(move(a, dir[i].first, dir[i].second), s_module, h_module).first > cost_count(a, s_module, h_module).first) { //check if over_s # increase
            //cout << "s # increase、 ";
            flag = 0;
        }
        // if(cost_count(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost_count(a, s_module, h_module).second) { //check if over_h # increase
        //     cout << "h # increase、 ";
        //     flag = 0;
        // }
        if(cost2(move(a, dir[i].first, dir[i].second), s_module, h_module).second > cost2(a, s_module, h_module).second) {  //check if over_h value increase with should_cancel
            //cout << "over_h value increase、 ";
            if(should_cancel_h(a, s_module, h_module, c_x, c_y, dir[i].first, dir[i].second, 1)) {
                //cout << "should_cancel、 ";
                flag = 0;
            }
        }
        else {
            if(if_hdir_increase(a, dir[i].first, dir[i].second, h_module).first) {
                //cout << "hdir increase、 ";
                flag = 0;
            }
        }
        if(boundary_check(move(a, dir[i].first, dir[i].second), c_x, c_y)) {
            //cout << "boundary_check、 ";
            flag = 0;
        }
        if(flag){
            out.push_back(dir_n[i]);
            dir_flag[i] = 0;
        }
        //cout << endl;
    }
    return out;
}

bool check(Module a, map<string, Module> s_module, map<string, Module> h_module, int x, int y) {
    bool h_flag = (cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).second != cost_count(a, s_module, h_module).second), s_flag = cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).first != cost_count(a, s_module, h_module).first;
    // cout << "w & h :" << a.w << ", " << a.h << endl;
    // cout << "d x & y :" << x << ", " << y << endl;
    // cout << "b x & y :" << a.x << ", " << a.y << endl;
    // cout << "a x & y :" << wh_resize_with_cord(a, x, y).x << ", " << wh_resize_with_cord(a, x, y).y << endl;
    //cout << "s_f " << cost_count(a, s_module, h_module).first << ", " << cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).first << ", h_f" << cost_count(a, s_module, h_module).second << ", " << cost_count(wh_resize_with_cord(a, x, y), s_module, h_module).second << endl;
    // for(auto i : overlap_with_s(a, s_module)) {
    //     cout << i.name << ", " << i.x << ", " << i.y << "、";
    // }
    // cout << endl;
    // for(auto i : overlap_with_h(a, h_module)) {
    //     cout << i.name << ", " << i.x << ", " << i.y << "、";
    // }
    // cout << endl;
    if (h_flag || s_flag) {
        return true;
    }
    return false;
}

Module wh_resize(Module a, int x, int y) {
    int nx = 0, ny = 0;
    // if(y == 1 && a.h % 2 == 1) {ny = 1;}
    // else if(y == -1 && a.h % 2 == 0) {ny = -1;}
    // else if(x == 1 && a.w % 2 == 1) {nx = 1;}
    // else if(x == -1 && a.w % 2 == 0) {nx = -1;}
    Module b(a.name, a.x + nx, a.y + ny, a.w + x, a.h + y, a.area, a.overlapping, a.identity);

    return b;
}


Module reduce_size(Module a, int x, int y) {
    int nx = 0, ny = 0;
    if(y == -1 && a.h % 2 == 1) {ny = 1;}
    else if(y == 1 && a.h % 2 == 0) {ny = -1;}
    else if(x == -1 && a.w % 2 == 1) {nx = 1;}
    else if(x == 1 && a.w % 2 == 0) {nx = -1;}
    Module b(a.name, a.x + nx, a.y + ny, a.w - abs(x), a.h - abs(y), a.area, a.overlapping, -1);
    // cout << "w & h :" << b.w << ", " << b.h << endl;
    // cout << "x, y :" << b.x << ", " << b.y << endl;
    // cout << "{" << "x, y :" << x << ", " << y << "、 nx, ny :" << nx << ", " << ny << "}" << endl;
    // cout << b.x << " " << b.y << endl;
    // cout << b.x << " " << b.y << endl;
    return b;
}

void resize(Module &b, map<string, Module> &s_module, map<string, Module> h_module, int c_x, int c_y) {
    Module a(b.name, b.x, b.y, b.w, b.h, b.area, b.overlapping, b.identity);
    vector<pair<int, int> > dir{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    vector<char> dir_n{'u', 'd', 'l', 'r'};
    int max_v = -1, s_reduce = cost2(reduce_size(a, dir[0].first, dir[0].second), s_module, h_module).first;
    int max = sum_of_cost2(reduce_size(a, dir[0].first, dir[0].second), s_module, h_module);
    for(int i = 0; i < 4; i++) {
        //cout << dir_n[i] << " cost :" << sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module) << endl;
        if( (sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module)) <= max) {
            //cout << "pass1" << endl;
            if( (cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module).first) <= s_reduce)
                //cout << "pass2" << endl;
                max = sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module);
                max_v = i;
                s_reduce = cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module).first;
        }
    }
    //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //cout << "reduce dir:" << dir_n[max_v] << endl;
    //cout << "cost before reduce : " << sum_of_cost2(a, s_module, h_module) << ", after : " << sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) << endl;
    while(rule_check(wh_resize(a, (-1)*abs(dir[max_v].first), (-1)*abs(dir[max_v].second)), s_module, h_module) && sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) < sum_of_cost2(a, s_module, h_module)) {
        //cout << "b cost : " << sum_of_cost2(a, s_module, h_module) << ", a cost :" << sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) << endl;
        Module temp_a = reduce_size(a, dir[max_v].first, dir[max_v].second);
        a.h = temp_a.h;
        a.w = temp_a.w;
        a.x = temp_a.x;
        a.y = temp_a.y;
        //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h <<  " cost : " << sum_of_cost2(a, s_module, h_module) << endl;
        //cout << "moved " << endl;
    }
    //cout << "overlap : " << (sum_of_cost2(a, s_module, h_module)) << endl;
    if(((a.w * a.h) - sum_of_cost2(a, s_module, h_module)) < double(a.area)) {
        //cout << "not enough area" << endl;
        for(auto i : dir) {
            //cout << "cost :" << (cost(a, s_module, h_module).second == cost(move(a, i.first, i.second), s_module, h_module).second) << endl;
            //cout << "cost : " << ((sum_of_cost2(a, s_module, h_module)) == (sum_of_cost2(move(a, i.first, i.second), s_module, h_module))) << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
            while(!(if_hdir_increase(a, i.first, i.second, h_module).first) && ((sum_of_cost2(a, s_module, h_module)) == (sum_of_cost2(move(a, i.first, i.second), s_module, h_module))) && rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && !if_sdir_increase(a, i.first, i.second, s_module).first && !boundary_check(move(a, i.first, i.second), c_x, c_y)) {
                if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                    //cout << endl << "area satisfy after re-expand" << endl;
                    b = a;
                    return;
                }
                cord_move(a, i.first, i.second);
                a.w += abs(i.first);
                a.h += abs(i.second);
                //cout << "{" << i.first << "," << i.second << "}";
            }
        }
    }
    if(((a.w * a.h) - sum_of_cost2(a, s_module, h_module)) < double(a.area)) {
        //cout << "still not enough area" << endl;
        for(auto i : dir) {
            //cout << "cost :" << (cost(a, s_module, h_module).second == cost(move(a, i.first, i.second), s_module, h_module).second) << endl;
            //cout << "cost b: " << (cost2(a, s_module, h_module).first) << "cost a: " <<(cost2(move(a, i.first, i.second), s_module, h_module).first) << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
            while((cost2(a, s_module, h_module).first == cost2(move(a, i.first, i.second), s_module, h_module).first) && rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && !if_sdir_increase(a, i.first, i.second, s_module).first && !boundary_check(move(a, i.first, i.second), c_x, c_y)) {
                if(((a.w + abs(i.first)) * (a.h + abs(i.second))) - cost2(wh_resize_with_cord(a, i.first, i.second), s_module, h_module).second == (a.w * a.h) - cost2(a, s_module, h_module).second) {
                    break;
                }
                if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                    //cout << endl << "area satisfy after re-expand" << endl;
                    if(sum_of_cost2(a, s_module, h_module)) {
                        //cout << "but with overlapping " << endl;
                    }
                    b = a;
                    return;
                }
                cord_move(a, i.first, i.second);
                a.w += abs(i.first);
                a.h += abs(i.second);
                //cout << "{" << i.first << "," << i.second << "}";
            }
        }
    }
    //cout << "should legalize with cut, can't satisfy after reduce" << endl;
}

void resize_for_s(Module &b, map<string, Module> &s_module, map<string, Module> h_module, int c_x, int c_y) {
    Module a(b.name, b.x, b.y, b.w, b.h, b.area, b.overlapping, b.identity);
    vector<pair<int, int> > dir{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    vector<char> dir_n{'u', 'd', 'l', 'r'};
    int max_v = -1, s_reduce = cost2(reduce_size(a, dir[0].first, dir[0].second), s_module, h_module).first;
    int max = sum_of_cost2(reduce_size(a, dir[0].first, dir[0].second), s_module, h_module);
    for(int i = 0; i < 4; i++) {
        //cout << dir_n[i] << " cost :" << sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module) << endl;
        if( (sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module)) <= max) {
            //cout << "pass1" << endl;
            if( (cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module).first) <= s_reduce)
                //cout << "pass2" << endl;
                max = sum_of_cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module);
                max_v = i;
                s_reduce = cost2(reduce_size(a, dir[i].first, dir[i].second), s_module, h_module).first;
        }
    }
    //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //cout << "reduce dir:" << dir_n[max_v] << endl;
    //cout << "cost before reduce : " << sum_of_cost2(a, s_module, h_module) << ", after : " << sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) << endl;
    while(rule_check(wh_resize(a, (-1)*abs(dir[max_v].first), (-1)*abs(dir[max_v].second)), s_module, h_module) && sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) < sum_of_cost2(a, s_module, h_module)) {
        //cout << "b cost : " << sum_of_cost2(a, s_module, h_module) << ", a cost :" << sum_of_cost2(reduce_size(a, dir[max_v].first, dir[max_v].second), s_module, h_module) << endl;
        Module temp_a = reduce_size(a, dir[max_v].first, dir[max_v].second);
        a.h = temp_a.h;
        a.w = temp_a.w;
        a.x = temp_a.x;
        a.y = temp_a.y;
        //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h <<  " cost : " << sum_of_cost2(a, s_module, h_module) << endl;
        //cout << "moved " << endl;
    }
    //cout << "overlap : " << (sum_of_cost2(a, s_module, h_module)) << endl;
    if(((a.w * a.h) - sum_of_cost2(a, s_module, h_module)) < double(a.area)) {
        //cout << "not enough area" << endl;
        for(auto i : dir) {  //expand without over
            //cout << "cost :" << (cost(a, s_module, h_module).second == cost(move(a, i.first, i.second), s_module, h_module).second) << endl;
            //cout << "cost : " << ((sum_of_cost2(a, s_module, h_module)) == (sum_of_cost2(move(a, i.first, i.second), s_module, h_module))) << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
            while(!(if_hdir_increase(a, i.first, i.second, h_module).first) && ((sum_of_cost2(a, s_module, h_module)) == (sum_of_cost2(move(a, i.first, i.second), s_module, h_module))) && rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && !if_sdir_increase(a, i.first, i.second, s_module).first && !boundary_check(move(a, i.first, i.second), c_x, c_y)) {
                if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                    //cout << endl << "area satisfy after re-expand" << endl;
                    b = a;
                    if(cost2(a, s_module, h_module).first) {
                        b.identity = 2;
                    }
                    else{
                        b.identity = 1;
                    }
                    return;
                }
                cord_move(a, i.first, i.second);
                a.w += abs(i.first);
                a.h += abs(i.second);
                //cout << "{" << i.first << "," << i.second << "}";
            }
        }
    }
    if(((a.w * a.h) - sum_of_cost2(a, s_module, h_module)) < double(a.area)) {
        //cout << "still not enough area" << endl;
        for(auto i : dir) {  //expand with h over
            //cout << "cost :" << (cost(a, s_module, h_module).second == cost(move(a, i.first, i.second), s_module, h_module).second) << endl;
            //cout << "cost b: " << (cost2(a, s_module, h_module).first) << "cost a: " <<(cost2(move(a, i.first, i.second), s_module, h_module).first) << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
            while((cost2(a, s_module, h_module).first == cost2(move(a, i.first, i.second), s_module, h_module).first) && rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && !if_sdir_increase(a, i.first, i.second, s_module).first && !boundary_check(move(a, i.first, i.second), c_x, c_y)) {
                if(((a.w + abs(i.first)) * (a.h + abs(i.second))) - cost2(wh_resize_with_cord(a, i.first, i.second), s_module, h_module).second == (a.w * a.h) - cost2(a, s_module, h_module).second) {
                    break;
                }
                if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                    //cout << endl << "area satisfy after re-expand" << endl;
                    if(sum_of_cost2(a, s_module, h_module)) {
                        //cout << "but with overlapping " << endl;
                    }
                    if(cost2(a, s_module, h_module).first) {
                        b.identity = 2;
                    }
                    else{
                        b.identity = 1;
                    }
                    b = a;
                    return;
                }
                cord_move(a, i.first, i.second);
                a.w += abs(i.first);
                a.h += abs(i.second);
                //cout << "{" << i.first << "," << i.second << "}";
            }
        }
    }
    if(((a.w * a.h) - sum_of_cost2(a, s_module, h_module)) < double(a.area)) {
        //cout << "still still not enough area" << endl;
        for(auto i : dir) {  //expand with s & h over
            //cout << "cost :" << (cost(a, s_module, h_module).second == cost(move(a, i.first, i.second), s_module, h_module).second) << endl;
            //cout << "cost b: " << (cost2(a, s_module, h_module).first) << "cost a: " <<(cost2(move(a, i.first, i.second), s_module, h_module).first) << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
            while((cost2(a, s_module, h_module).second == cost2(move(a, i.first, i.second), s_module, h_module).second) && rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && !(if_hdir_increase(a, i.first, i.second, h_module).first) && !boundary_check(move(a, i.first, i.second), c_x, c_y)) {
                if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                    //cout << endl << "area satisfy after re-expand" << endl;
                    if(sum_of_cost2(a, s_module, h_module)) {
                        //cout << "but with overlapping " << endl;
                    }
                    b = a;
                    if(cost2(a, s_module, h_module).first) {
                        b.identity = 2;
                    }
                    else{
                        b.identity = 1;
                    }
                    return;
                }
                cord_move(a, i.first, i.second);
                a.w += abs(i.first);
                a.h += abs(i.second);
                //cout << "{" << i.first << "," << i.second << "}";
            }
        }
    }
    //cout << "should legalize using otherway, can't satisfy just resizing" << endl;
}

bool legalize(Module &a, map<string, Module> &s_module, map<string, Module> h_module, int c_x, int c_y) {
    //cout << "target :" << a.name << "cord: " << a.x << " " << a.y << endl;
    vector<Module> out;
    if(a.identity) {
        return true;
    }
    vector<pair<int, int> > dir{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    vector<char> dir_n{'u', 'd', 'l', 'r'};
    for(auto i : dir) {
        
        bool flag  = cost(a, s_module, h_module).second >= cost(move(a, i.first, i.second), s_module, h_module).second;
        //cout << "dir : {" << i.first << "," << i.second << "}" << endl << "flag: " << flag << endl;
            //cout << "hdir :" << !(if_hdir_increase(a, i.first, i.second, h_module).first) << endl;
            //cout << "sdir :" << !if_sdir_increase(a, i.first, i.second, s_module).first << endl;
            //cout << "boundary :" << !boundary_check(move(a, i.first, i.second), c_x, c_y) << endl;
            //cout << "rule :" << rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) << endl;
        while(rule_check(wh_resize(a, abs(i.first), abs(i.second)), s_module, h_module) && flag && !boundary_check(move(a, i.first, i.second), c_x, c_y) && !if_hdir_increase(a, i.first, i.second, h_module).first && !if_sdir_increase(a, i.first, i.second, s_module).first) {
            cord_move(a, i.first, i.second);
            a.w += abs(i.first);
            a.h += abs(i.second);
            //cout << "{" << i.first << "," << i.second << "}";
            flag  = cost(a, s_module, h_module).second >= cost(move(a, i.first, i.second), s_module, h_module).second;
            if(((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second) >= double(a.area)) {
                //cout << "sat" << endl;
                break;
            }
        }
        double over = ((a.w * a.h) - cost2(a, s_module, h_module).first - cost2(a, s_module, h_module).second);
        //cout << "w、h: "<< a.w << "、" << a.h << " over :" << over << ", area :" << a.area << endl;
        if(over >= double(a.area)) {
            a.identity = 1;
            resize(a, s_module, h_module, c_x, c_y);
            return true;
        }
    }

    cout << endl;

    int s_over = cost_count(a, s_module, h_module).first, h_over = cost_count(a, s_module, h_module).second;

    //cout << "over #:" << s_over << " " << h_over << endl;
    if(h_over && !s_over) { //overlap with h only
        //cout << "type 1 " << endl;
        string m_dir = dir_choose_for_h(a, s_module, h_module, c_x, c_y);
        //cout << "dir:" << m_dir << endl;
        if(m_dir.length() == 0) {
            m_dir = dir_choose_for_h2(a, s_module, h_module, c_x, c_y);
            //cout << "dir:" << m_dir << endl;
        }
        vector<int> flag_{0, 0, 0, 0};
        int flag = 0, only_dir = 0;
        if(m_dir.length() == 1) {
            only_dir = 1;
        }
        for(int i = 0; i < m_dir.length(); i++) {
            flag_[i] = 1;
            flag = 1;
        }
        //cout << "flag :" << flag << endl;
        while(flag) {
            
            for(int i = 0; i < m_dir.length(); i++) {
                if(m_dir[i] == 'n') {
                    continue;
                }
                //cout << "moving dir " << m_dir[i] << " " << endl;
                // cout << "move dir" << m_dir[i] << " ";
                if((m_dir[i] != 'n') && boundary_check(wh_resize_with_cord(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second)), c_x, c_y)) {
                    //cout << i << "dir : " << m_dir[i] << " finished" << endl;
                    m_dir[i] = 'n';
                    // int aaaaa = 0;
                    // cout << "cin" << endl;
                    // cin >> aaaaa;
                    flag_[i] = 0;
                    continue;
                }
                if(!rule_check(wh_resize(a, abs(dir_decide(m_dir[i]).first), abs(dir_decide(m_dir[i]).second)), s_module, h_module)) {
                    //cout << i << ": stucking... ";
                    // if(only_dir) {
                    //     return out;
                    // }
                    return false; //maybe wrong-----------------------------------
                }
                cord_move(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second));
                a.w += abs(dir_decide(m_dir[i]).first);
                a.h += abs(dir_decide(m_dir[i]).second);
                //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
                if((a.w * a.h) - cost2(a, s_module, h_module).second - cost2(a, s_module, h_module).first >= a.area) {
                    //cout << "area satisfy with " << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
                    a.identity = 1;
                    resize(a, s_module, h_module, c_x, c_y);
                    return true;
                }
                if(check(a, s_module, h_module, dir_decide(m_dir[i]).first, dir_decide(m_dir[i]).second)) {
                    //cout << i << "dir : " << m_dir[i] << "turn to n" << endl;
                    m_dir[i] = 'n';
                    int aaaaa = 0;
                    //cout << "cin" << endl;
                    cin >> aaaaa;
                    flag_[i] = 0;
                    // int temp = 0;
                    // for(auto j : m_dir) {
                    //     if(j != 'n') {
                    //         temp++;
                    //     }
                    // }
                    // if(temp == 1) {
                    //     only_dir = 1;
                    // }
                }
            }
            flag = (flag_[0] || flag_[1] || flag_[2] || flag_[3]); 

        }
    }
    // else if (!h_over && s_over){  //overlap with s only
    //     //cout << "type 2 " << endl;
    //     resize_for_s(a, s_module, h_module, c_x, c_y);
    //     if(a.identity) {
    //         vector<Module> name = overlap_with_s(a, s_module);
    //         for(auto &i : name) {
    //             i.identity = 1;
    //         }
    //         return true;
    //     }
    //     string m_dir = dir_choose_for_h2(a, s_module, h_module, c_x, c_y);
    //     //cout << "dir:" << m_dir << endl;
    //     vector<int> flag_{0, 0, 0, 0};
    //     // for(int i = 0; i < m_dir.length(); i++) {
    //     //     flag_[i] = 1;
    //     //     flag = 1;
    //     // }
    //     for(int i = 0; i < m_dir.length(); i++) {
    //         //cout << "moving dir " << m_dir[i] << " " << endl;
    //         bool flag = 1;
    //         while(flag) {
    //             if(boundary_check(wh_resize_with_cord(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second)), c_x, c_y)) {
    //                 //cout << i << "dir : " << m_dir[i] << " finished" << endl;
    //                 break;
    //             }
    //             if(!rule_check(wh_resize(a, abs(dir_decide(m_dir[i]).first), abs(dir_decide(m_dir[i]).second)), s_module, h_module)) {
    //                 //cout << ": stucking at " << m_dir[i];
    //                 // if(only_dir) {
    //                 //     return out;
    //                 // }
    //                 break;
    //             }
    //             cord_move(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second));
    //             a.w += abs(dir_decide(m_dir[i]).first);
    //             a.h += abs(dir_decide(m_dir[i]).second);
    //             //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //             if((a.w * a.h) - sum_of_cost2(a, s_module, h_module) >= a.area) {
    //                 //cout << "area satisfy with " << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //                 if(cost2(a, s_module, h_module).first) {
    //                     a.identity = 2;
    //                 }
    //                 else{
    //                     a.identity = 1;
    //                 }
    //                 vector<Module> name = overlap_with_s(a, s_module);
    //                 for(auto &i : name) {
    //                     i.identity = 1;
    //                 }
    //                 // resize(a, s_module, h_module, c_x, c_y);
    //                 return true;
    //             }
    //             if(check(a, s_module, h_module, dir_decide(m_dir[i]).first, dir_decide(m_dir[i]).second)) {
    //                 //cout << i << "dir : " << m_dir[i] << "touched" << endl;
    //                 break;
    //                 // int temp = 0;
    //                 // for(auto j : m_dir) {
    //                 //     if(j != 'n') {
    //                 //         temp++;
    //                 //     }
    //                 // }
    //                 // if(temp == 1) {
    //                 //     only_dir = 1;
    //                 // }
    //             }
    //         }
    //     }
    // }
    // else {  //overlap with h and s
    //     //cout << "type 3 " << endl;
    //     resize_for_s(a, s_module, h_module, c_x, c_y);
    //     if(a.identity) {
    //         vector<Module> name = overlap_with_s(a, s_module);
    //         for(auto &i : name) {
    //             i.identity = 1;
    //         }
    //         return true;
    //     }
    //     string m_dir = dir_choose_for_h2(a, s_module, h_module, c_x, c_y);
    //     //cout << "dir:" << m_dir << endl;
    //     vector<int> flag_{0, 0, 0, 0};
    //     // for(int i = 0; i < m_dir.length(); i++) {
    //     //     flag_[i] = 1;
    //     //     flag = 1;
    //     // }
    //     for(int i = 0; i < m_dir.length(); i++) {
    //         //cout << "moving dir " << m_dir[i] << " " << endl;
    //         bool flag = 1;
    //         while(flag) {
    //             if(boundary_check(wh_resize_with_cord(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second)), c_x, c_y)) {
    //                 //cout << i << "dir : " << m_dir[i] << " finished" << endl;
    //                 break;
    //             }
    //             if(!rule_check(wh_resize(a, abs(dir_decide(m_dir[i]).first), abs(dir_decide(m_dir[i]).second)), s_module, h_module)) {
    //                 //cout << ": stucking at " << m_dir[i];
    //                 // if(only_dir) {
    //                 //     return out;
    //                 // }
    //                 break;
    //             }
    //             cord_move(a, (dir_decide(m_dir[i]).first), (dir_decide(m_dir[i]).second));
    //             a.w += abs(dir_decide(m_dir[i]).first);
    //             a.h += abs(dir_decide(m_dir[i]).second);
    //             //cout << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //             if((a.w * a.h) - sum_of_cost2(a, s_module, h_module) >= a.area) {
    //                 //cout << "area satisfy with " << "xywh" << a.x << ", " << a.y << ", " << a.w << ", " << a.h << endl;
    //                 if(cost2(a, s_module, h_module).first) {
    //                     a.identity = 2;
    //                 }
    //                 else{
    //                     a.identity = 1;
    //                 }
    //                 vector<Module> name = overlap_with_s(a, s_module);
    //                 for(auto &i : name) {
    //                     i.identity = 1;
    //                 }
    //                 // resize(a, s_module, h_module, c_x, c_y);
    //                 return true;
    //             }
    //             if(check(a, s_module, h_module, dir_decide(m_dir[i]).first, dir_decide(m_dir[i]).second)) {
    //                 //cout << i << "dir : " << m_dir[i] << "touched" << endl;
    //                 break;
    //                 // int temp = 0;
    //                 // for(auto j : m_dir) {
    //                 //     if(j != 'n') {
    //                 //         temp++;
    //                 //     }
    //                 // }
    //                 // if(temp == 1) {
    //                 //     only_dir = 1;
    //                 // }
    //             }
    //         }
    //     }
    // }
    //cout << endl;
    return false;
}

double angle_cal(Module a, Module b) {
    int delta_x = b.x - a.x, delta_y = b.y - a.y, basis = 0;
    double len =  pow(delta_x, 2.0) + pow(delta_y, 2.0);
    len = sqrt(len);
    double normal_x = delta_x / len;
    bool flag = 0;
    if(delta_y > 0) {
        flag = 0;
    }
    else if (delta_y < 0) {
        flag = 1;
    }
    else {
        if(delta_x > 0)
            return 0.0;
        else
            return 180.0;
    }
    double result = acos(normal_x) * 180 / 3.141592;
    if(flag) {
        result = 360.0 + (result * -1);
    }
    return result;
}

void quadrant_cal(Module a, Module b, vector<int> &flag) { 
    int delta_x = b.x - a.x, delta_y = b.y - a.y, basis = 0;
    if(delta_x > 0 && delta_y > 0) {
        flag[0] += 1;
    }
    else if(delta_x < 0 && delta_y > 0) {
        flag[1] += 1;
    }
    else if(delta_x < 0 && delta_y < 0) {
        flag[2] += 1;
    }
    else if(delta_x > 0 && delta_y < 0) {
        flag[3] += 1;
    }
}

pair<int, int> modify_from(Module a, vector<int> flag) {
    int max = flag[0], max_i = 0;
    for(int i = 0; i < 4; i++) {
        if(flag[i] <= max){
            max = flag[i];
            max_i = i;
        }
    }
    if(max_i = 0) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y + a.h / 2 + a.h % 2);
    }
    else if(max_i = 1) {
        return make_pair(a.x - a.w / 2, a.y + a.h / 2 + a.h % 2);
    }
    else if(max_i = 2) {
        return make_pair(a.x - a.w / 2, a.y - a.h / 2);
    }
    else if(max_i = 3) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y - a.h / 2);
    }
    return make_pair(0, 0);
}

pair<int, int> start_from(Module a, double angle) {
    if(angle > 315.0 || angle <= 45) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y + a.h / 2 + a.h % 2);
    }
    else if(angle > 225.0) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y - a.h / 2);
    }
    else if(angle > 135.0) {
        return make_pair(a.x - a.w / 2, a.y - a.h / 2);
    }
    else if(angle > 45.0) {
        return make_pair(a.x - a.w / 2, a.y + a.h / 2 + a.h % 2);
    }
    return make_pair(0, 0);
}

pair<int, int> fin_point(Module a, double angle) {
    if(angle > 315.0) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y - a.h / 2);
    }
    else if(angle > 225.0) {
        return make_pair(a.x - a.w / 2, a.y - a.h / 2);
    }
    else if(angle > 135.0) {
        return make_pair(a.x - a.w / 2, a.y + a.h / 2 + a.h % 2);
    }
    else if(angle > 45.0) {
        return make_pair(a.x + a.w / 2 + a.w % 2, a.y + a.h / 2 + a.h % 2);
    }
    return make_pair(0, 0);
}

pair<int, int> first_point(Module a, Module b, int type, int dir) {
    if(type == 0) {
        if(dir == 1) {
            return make_pair(b.x - b.w / 2, a.y + a.h / 2 + a.h % 2);
        }
        else if(dir == -1) {
            return make_pair(a.x + a.w / 2 + a.w % 2, b.y + b.h / 2 + b.h % 2);
        }
        else if(dir == 2) {
            return make_pair(a.x - a.w / 2, b.y - b.h / 2);
        }
        else if(dir == -2) {
            return make_pair(b.x + b.w / 2 + b.w % 2, a.y - a.h / 2);
        }
    }
    else if(type == 1) {
        if(dir == 0) {
            return make_pair(a.x + a.w / 2 + a.w % 2, b.y + b.h / 2 + b.h % 2);
        }
        else if(dir == 1) {
            return make_pair(a.x - a.w / 2, b.y - b.h / 2);
        }
        else if(dir == 2) {
            return make_pair(b.x + b.w / 2 + b.w % 2, a.y - a.h / 2);
        }
        else if(dir == 3) {
            return make_pair(b.x - b.w / 2, a.y + a.h / 2 + a.h % 2);
        }
    }
    else if(type == 2) {
        if(dir == 0) {
            return make_pair(b.x - b.w / 2, a.y + a.h / 2 + a.h % 2);
        }
        else if(dir == 1) {
            return make_pair(b.x + b.w / 2 + b.w % 2, a.y - a.h / 2);
        }
        else if(dir == 2) {
            return make_pair(a.x + a.w / 2 + a.w % 2, b.y + b.h / 2 + b.h % 2);
        }
        else if(dir == 3) {
            return make_pair(a.x - a.w / 2, b.y - b.h / 2);
        }
    }
    return make_pair(0, 0);
}

void point_add(Module &a, Module b) {
    int x_lim = 0, y_lim = 0, x_o = abs(a.x - b.x), y_o = abs(a.y - b.y); //x_o is x dis's overlpaiing
    int delta_x = b.x - a.x, delta_y = b.y - a.y;
    int type = -1, dir = -1;
    if(a.x < b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h;
        }
    }
    else if (a.x > b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h;
        }
    }
    else 
    {
        if(a.y < b.y)
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        
    }

    if(x_o < x_lim) //type decide
    {
        // if(x_lim - x_o > a.w)
        // {
        //     x_o = a.w;
        //     type = 2;
        // }
        // else if (x_lim - x_o > b.w)
        // {
        //     x_o = b.w;
        //     if((a.x + a.w / 2 + a.w % 2 == b.x + b.w / 2 + b.w % 2) || (a.x - a.w / 2 == b.x - b.w / 2)) {
        //         type = 0;
        //     }
        //     type = 1;
        // }
        // else
        // {
        //     cout << "Hix" << endl;
        //     x_o = x_lim - x_o;
        //     type = 0;
        // }
        if((b.y + b.h / 2 + b.h % 2 >= a.y + a.h / 2 + a.h % 2) && (b.y - b.h / 2 <= a.y - a.h / 2))
        {
            x_o = x_lim - x_o;
            y_o = a.h;
            type = 2;
        }
        else if((b.y + b.h / 2 + b.h % 2 < a.y + a.h / 2 + a.h % 2) && (b.y - b.h / 2 > a.y - a.h / 2))
        {
            x_o = x_lim - x_o;
            y_o = b.h;
            type = 1;
        }
        else
        {
            x_o = x_lim - x_o;
            y_o = y_lim - y_o;
            type = 0;
        }
    }
    else 
    {
        x_o = 0;
    }

    if(y_o < y_lim) //type decide
    {
        // if(y_lim - y_o > a.h)
        // {
        //     y_o = a.h;
        //     type = 2;
        // }
        // else if (y_lim - y_o > b.h)
        // {
        //     y_o = b.h;
        //     if((a.y + a.h / 2 + a.h % 2 == b.y + b.h / 2 + b.h % 2) || (a.y - a.h / 2 == b.y - b.h / 2)) {
        //         type = 0;
        //     }
        //     type = 1;
        // }
        // else
        // {
        //     cout << "Hiy" << endl;
        //     y_o = y_lim - y_o;
        //     type = 0;
        // }
        if((b.x + b.w / 2 + b.w % 2 >= a.x + a.w / 2 + a.w % 2) && (b.x - b.w / 2 <= a.x - a.w / 2))
        {
            y_o = y_lim - y_o;
            x_o = a.w;
            type = 2;
        }
        else if((b.x + b.w / 2 + b.w % 2 < a.x + a.w / 2 + a.w % 2) && (b.x - b.w / 2 > a.x - a.w / 2))
        {
            y_o = y_lim - y_o;
            x_o = b.w;
            type = 1;
        }
        else
        {
            y_o = y_lim - y_o;
            x_o = x_lim - x_o;
            type = 0;
        }
    }
    else 
    {
        y_o = 0;
    }

    if(type == 2) {
        if(delta_x > 0) {
            dir = 0;
        }
        else if(delta_x < 0) {
            dir = 1;
        }
        else if(delta_y < 0) {
            dir = 2;
        }
        else if(delta_y > 0) {
            dir = 3;
        }
    }
    else if(type == 1) {
        if(delta_x > 0 && delta_y < 0) {
            if((b.x - b.w / 2 < a.x + a.w / 2 + a.x % 2) && (b.x + b.w / 2 + b.w % 2 > a.x + a.w / 2 + a.x % 2))
                dir = 0;
            else
                dir = 2;
        }
        else if(delta_x < 0 && delta_y > 0) {
            if((b.x + b.w / 2 + b.w % 2 > a.x - a.w / 2) && (b.x - b.w / 2 < a.x - a.w / 2))
                dir = 1;
            else
                dir = 3;
        }
        else if(delta_x < 0 && delta_y < 0) {
            if((b.x + b.w / 2 + b.w % 2 > a.x - a.w / 2) && (b.x - b.w / 2 < a.x - a.w / 2))
            dir = 1;
        }
        else if(delta_x > 0 && delta_y > 0) {
            if((b.x - b.w / 2 < a.x + a.w / 2 + a.x % 2) && (b.x + b.w / 2 + b.w % 2 > a.x + a.w / 2 + a.x % 2))
                dir = 0;
            else
                dir = 3;
        }
    }
    else if(type == 0) {
        if(delta_x > 0 && delta_y < 0) {
            dir = -1;
        }
        else if(delta_x < 0 && delta_y > 0) {
            dir = 2;
        }
        else if(delta_x < 0 && delta_y < 0) {
            dir = -2;
        }
        else if(delta_x > 0 && delta_y > 0) {
            dir = 1;
        }
    }
    cout << "Point adding, block name: " << b.name << endl;
    cout << "overlap block type: " << type << ", dir: " << dir << endl; 
    pair<int, int> f_p = first_point(a, b, type, dir);
    cout << "First point: x: " << f_p.first << ", y: " << f_p.second << endl;
    if(type == 0) {
        if(dir == 1) {
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == -1) {
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == -2) {
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 2) {
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
        }
    }
    else if(type == 1) {
        if(dir == 0) {
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 1) {
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 2) {
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 3) {
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
        }
    }
    else if(type == 2) {
        if(dir == 0) {
            a.detail_point.push_back(f_p);
            f_p.second -= y_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 1) {
            a.detail_point.push_back(f_p);
            f_p.second += y_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 2) {
            a.detail_point.push_back(f_p);
            f_p.first -= x_o;
            a.detail_point.push_back(f_p);
        }
        else if(dir == 3) {
            a.detail_point.push_back(f_p);
            f_p.first += x_o;
            a.detail_point.push_back(f_p);
        }
    }
}

int type_decide(Module a, Module b) {
    int x_lim = 0, y_lim = 0, x_o = abs(a.x - b.x), y_o = abs(a.y - b.y); //x_o is x dis's overlpaiing
    int delta_x = b.x - a.x, delta_y = b.y - a.y;
    int type = -1, dir = -1;
    if(a.x < b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h;
        }
    }
    else if (a.x > b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h;
        }
    }
    else 
    {
        if(a.y < b.y)
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        
    }

    if(x_o < x_lim) //type decide
    {
        if(x_lim - x_o > a.w)
        {
            x_o = a.w;
            type = 2;
        }
        else if (x_lim - x_o > b.w)
        {
            x_o = b.w;
            if((a.x + a.w / 2 + a.w % 2 == b.x + b.w / 2 + b.w % 2) || (a.x - a.w / 2 == b.x - b.w / 2)) {
                type = 0;
            }
            type = 1;
        }
        else
        {
            x_o = x_lim - x_o;
            type = 0;
        }
    }
    else 
    {
        x_o = 0;
    }

    if(y_o < y_lim) //type decide
    {
        if(y_lim - y_o > a.h)
        {
            y_o = a.h;
            type = 2;
        }
        else if (y_lim - y_o > b.h)
        {
            y_o = b.h;
            if((a.y + a.h / 2 + a.h % 2 == b.y + b.h / 2 + b.h % 2) || (a.y - a.h / 2 == b.y - b.h / 2)) {
                type = 0;
            }
            type = 1;
        }
        else
        {
            y_o = y_lim - y_o;
            type = 0;
        }
    }
    else 
    {
        y_o = 0;
    }
    return type;
}

int dir_decide(Module a, Module b) {
    int x_lim = 0, y_lim = 0, x_o = abs(a.x - b.x), y_o = abs(a.y - b.y); //x_o is x dis's overlpaiing
    int delta_x = b.x - a.x, delta_y = b.y - a.y;
    int type = -1, dir = -1;
    if(a.x < b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + a.w % 2;
            y_lim = a.h;
        }
    }
    else if (a.x > b.x) 
    {
        if(a.y < b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else if (a.y > b.y)
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        else
        {
            x_lim = a.w / 2 + b.w / 2 + b.w % 2;
            y_lim = a.h;
        }
    }
    else 
    {
        if(a.y < b.y)
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + a.h % 2;
        }
        else
        {
            x_lim = a.w;
            y_lim = a.h / 2 + b.h / 2 + b.h % 2;
        }
        
    }

    if(x_o < x_lim) //type decide
    {
        if(x_lim - x_o > a.w)
        {
            x_o = a.w;
            type = 2;
        }
        else if (x_lim - x_o > b.w)
        {
            x_o = b.w;
            if((a.x + a.w / 2 + a.w % 2 == b.x + b.w / 2 + b.w % 2) || (a.x - a.w / 2 == b.x - b.w / 2)) {
                type = 0;
            }
            type = 1;
        }
        else
        {
            x_o = x_lim - x_o;
            type = 0;
        }
    }
    else 
    {
        x_o = 0;
    }

    if(y_o < y_lim) //type decide
    {
        if(y_lim - y_o > a.h)
        {
            y_o = a.h;
            type = 2;
        }
        else if (y_lim - y_o > b.h)
        {
            y_o = b.h;
            if((a.y + a.h / 2 + a.h % 2 == b.y + b.h / 2 + b.h % 2) || (a.y - a.h / 2 == b.y - b.h / 2)) {
                type = 0;
            }
            type = 1;
        }
        else
        {
            y_o = y_lim - y_o;
            type = 0;
        }
    }
    else 
    {
        y_o = 0;
    }

    if(type == 0 || type == 2) {
        if(delta_x > 0) {
            dir = 0;
        }
        else if(delta_x < 0) {
            dir = 1;
        }
        else if(delta_y < 0) {
            dir = 2;
        }
        else if(delta_y > 0) {
            dir = 3;
        }
    }
    else if(type == 1) {
        if(delta_x > 0 && delta_y < 0) {
            dir = 0;
        }
        else if(delta_x < 0 && delta_y > 0) {
            dir = 1;
        }
        else if(delta_x < 0 && delta_y < 0) {
            dir = 2;
        }
        else if(delta_x > 0 && delta_y > 0) {
            dir = 3;
        }
    }
    return dir;
}

int quadrant_decide(Module a, pair<int, int> b) {
    int delta_x = b.first - a.x, delta_y = b.second - a.y;
    if(delta_x > 0 && delta_y > 0) {
        return 1;
    }
    else if(delta_x < 0 && delta_y > 0) {
        return 2;
    }
    else if(delta_x < 0 && delta_y < 0) {
        return 3;
    }
    else if(delta_x > 0 && delta_y < 0) {
        return 4;
    }
    return 0;
}

int quadrant_decide_for_degree(double a) {
    if(a == 45) {
        return 1;
    }
    else if(a == 135) {
        return 2;
    }
    else if(a == 225) {
        return 3;
    }
    else if(a == 315) {
        return 4;
    }
    return 0;
}

pair<int, int> corner_posi(Module a, double b) {
    if(b == 45) {
        return make_pair(a.x + (a.w / 2) + (a.w % 2), a.y + (a.h / 2)+ (a.h % 2));
    }
    else if(b == 135) {
        return make_pair(a.x - (a.w / 2), a.y + (a.h / 2)+ (a.h % 2));
    }
    else if(b == 225) {
        return make_pair(a.x - (a.w / 2), a.y - (a.h / 2));
    }
    else if(b == 315) {
        return make_pair(a.x + (a.w / 2) + (a.w % 2), a.y - (a.h / 2));
    }
    return make_pair(0, 0);
}

void point(map<string, Module> &s_module, map<string, Module> h_module) {
    vector<double> corner_degree{45, 135, 225, 315};
    vector<int> flag{0, 0, 0, 0};
    for(auto &i : s_module) {
        if(1 || (i.second .identity == 1 && (cost_count(i.second, s_module, h_module).first == 0)) || (i.second .identity == 2)) {//check identity -> cutting
            map<double, Module, greater<double> > around;
            //cout << "name: " << i.second.name << endl;
            for(auto j : overlap_with_s(i.second, s_module)) {
                around[angle_cal(i.second, j)] = j;
                quadrant_cal(i.second, j, flag);
                // if(type_decide(i.second, j) == 2) {
                //     //dir_decide
                // }
            }
            for(auto j : overlap_with_h(i.second, h_module)) {
                around[angle_cal(i.second, j)] = j;
                quadrant_cal(i.second, j, flag);
                // if(type_decide(i.second, j) == 2) {
                    
                // }
            }
            cout << "\nBlock name: " << i.second.name << " mid point: mx: " << i.second.x << ", my: " << i.second.y << endl;
            for(auto k : around)
                cout << "Overlap Block name: " << k.second.name << ", degree: " << k.first << ", x: " << k.second.x << ", y: " << k.second.y << endl;
            if(around.size() == 0) {
                continue;
            }
            pair<int, int> start_point = start_from(i.second, (*(around.begin())).first);
            cout << "Start point: " << start_point.first << ", " << start_point.second << endl;
            for(auto j = around.begin(); j != around.end(); j++) {
                point_add(i.second, (*j).second);
                for(int l = 0; l < 4; l++) {
                    int flag = 1;
                    auto temp = ++j;
                    if(corner_degree[l] < (*j).first && corner_degree[l] > (*(temp)).first) {
                        if( (type_decide(i.second, (*j).second) == 0) && (quadrant_decide(i.second, make_pair((*j).second.x, (*j).second.y)) == quadrant_decide_for_degree(corner_degree[l])) ) {
                            flag == 0;
                        }
                        if( (type_decide(i.second, (*(temp)).second) == 0) && (quadrant_decide(i.second, make_pair((*(temp)).second.x, (*(temp)).second.y)) == quadrant_decide_for_degree(corner_degree[l])) ) {
                            flag == 0;
                        }
                        if(flag) {
                            i.second.detail_point.push_back(corner_posi(i.second, corner_degree[l]));
                        }
                    }
                }
            }
            double f_point = (*(--(around.end()))).first;
            pair<int, int> finish_point = fin_point(i.second, f_point);

        }
        //cout << "\nBlock name: " << i.second.name << " mid point: mx: " << i.second.x << ", my: " << i.second.y << endl;
        for(auto k : i.second.detail_point)
            cout << "x: " << k.first << ", y: " << k.second << endl;
    }
}