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
                //cout << "Overlap block: " << i.first << ", area: " << cal_overlapping(i.second, a) << endl;
                num++;
            }
        }
    }

    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            total += cal_overlapping(i.second, a);
            //cout << "Overlap block: " << i.first << ", area: " << cal_overlapping(i.second, a) << endl;
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

pair<int, int> cost_count(Module a, map<string, Module> s_module, map<string, Module> h_module) {
    int s = 0, h = 0;
    for(auto i : s_module) 
    {
        if(a.name != i.first) 
        {    
            if(cal_overlapping(i.second, a))
            {
                s++;
            }
        }
    }

    for(auto i : h_module) 
    {
        if(cal_overlapping(i.second, a))
        {
            h++;
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
    else
        return make_pair(0, 1);
}

Module move(Module a, int x, int y) {
    Module b(a.name, a.x + x, a.y + y, a.w, a.h, a.area, a.overlapping, a.identity);
    return b;
}

pair<bool, int> if_hdir_increase(Module a, int x, int y, map<string, Module> h_module) {
    vector<Module> name = overlap_with_h(a, h_module);
    bool flag = 1;
    int value = 0;
    for(auto i : name) {
        int v = cal_overlapping(a, i) - cal_overlapping(a, i);
        if(v > 0) {
            flag = 0;
        }
        value += v;
    }
    return make_pair(flag, value);
}
pair<bool, int> if_sdir_increase(Module a, int x, int y, map<string, Module> s_module) {
    vector<Module> name = overlap_with_s(a, s_module);
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

bool boundary_check(Module a, int c_x, int c_y) {
    if(a.y + a.h / 2 + a.h % 2 >= c_y) {
        return true;
    }
    else if(a.y - a.h / 2 - a.h % 2 <= 0) {
        return true;
    }
    else if(a.x + a.w / 2 + a.w % 2 >= c_x) {
        return true;
    }
    else if(a.x - a.w / 2 - a.w % 2 <= 0) {
        return true;
    }
    return false;
}

bool should_cancel(Module a, map<string, Module> s_module, map<string, Module> h_module, int c_x, int c_y, int x, int y, int offset) {
    int now_h = cost_count(move(a, x*offset, y*offset), s_module, h_module).second;
    a.x += x;
    a.y += y;
    while(1) {
        if ((cost_count(move(a, x, y), s_module, h_module).second > now_h) || boundary_check(a, c_x, c_y)) {
            
            double temp = cost(a, s_module, h_module).second;
            //cout << x << "should_cancel " << a.name << ':' << boundary_check(a, c_x, c_y) << " " << temp << " " << now_h << " " << cost_count(move(a, x, y), s_module, h_module).second << endl;
            if(temp > 10.0) {
                return true;
            }
            else {
                return false;
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
                if (cost_count(move(a, dirx[i], diry[i]), s_module, h_module).first && if_sdir_increase(move(a, dirx[i]*offset, diry[i]*offset), dirx[i], diry[i], s_module).first) {
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
                if ((cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).first > cost_count(a, s_module, h_module).first) && if_sdir_increase(move(a, dirx[i]*offset, diry[i]*offset), dirx[i], diry[i], s_module).first) {
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
                if ((cost_count(move(a, dirx[i]*offset, diry[i]*offset), s_module, h_module).first > cost_count(a, s_module, h_module).first) && if_sdir_increase(move(a, dirx[i]*offset, diry[i]*offset), dirx[i], diry[i], s_module).first) {
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
// vector<int> relative_position(Module a, Module b)//up、down、left、right
// {
//     123;
// }

// void resize(Module a)
// {
//     123;
// }