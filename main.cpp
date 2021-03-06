//
//  main.cpp
//
//
//  Created by Haruka IWAKI on 2016/06/19.
//
//
#include <iomanip>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#define BOARD_SIZE 10      // 盤面サイズ 10 * 10
#define STONE_SPACE 0      // 盤面にある石 なし
#define STONE_BLACK 1      // 盤面にある石 黒
#define STONE_WHITE 2      // 盤面にある石 白
#define MAX_TRIES 50000


using namespace std;
typedef pair<int,int> xy;

struct map_data {
    int map[BOARD_SIZE][BOARD_SIZE];
    int count;
    int player_flag;
};

struct Node {
    Node* parent;
    vector <Node*> child;
    xy move;
    vector <xy> untriedMove;
    double wins;
    int visits;
};



void display_map(map_data* data){
    cout << endl << data->count << "回目　display_board" << endl;
    cout << "  0   ";
    for (int i= 0; i<BOARD_SIZE; i++) {
        cout <<  setw(3) <<i;
    }
    cout << endl;
    for(int i = 0;i <BOARD_SIZE;i++){
        cout << setw(2)<< left << i ;
        for(int j= 0;j<BOARD_SIZE;j++){
            cout << "  ";
            if(data->map[i][j]==STONE_SPACE) cout <<"×";
            else if(data->map[i][j]==STONE_BLACK) cout <<"●";
            else cout <<"◯";
        }
        cout << endl;
    }
    cout << endl;
}



Node* select_child_node(Node* node) {
    Node* p;
    double c = sqrt(2);
    double max_child=0.0;
    for(auto itr = node->child.begin(); itr != node->child.end(); itr++) {
        Node* i = *itr;
        double value = i->wins/i->visits + c * sqrt(log(node->visits)/i->visits);
        if (value > max_child) {
            p = i;
            max_child = value;
        }
    }
    return p;
}



void updata_node(Node* node,double result) {
    node->wins += result;
    node->visits+= 1;
}

void back_propagate_node (Node* node,double result) {
    for (Node* i = node; i != nullptr; i = i->parent) {
        updata_node(i, result);
    }
}

void init_map_data (map_data* data){
    
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++){
            data->map[i][j]=STONE_SPACE;
        }
    }
    data->count=0;
    data->player_flag=STONE_BLACK;
}

void init_node (Node* node){
    node->parent = nullptr;
    node->wins = 0;
    node->visits = 0;
}

Node* new_node () {
    Node* node = new Node;
    init_node(node);
    return node;
}

Node* expand_child(Node* node) {
    random_device rnd;
    mt19937 mt(rnd());
    uniform_int_distribution<> rand(0,node->untriedMove.size()-1);
    int i = rand(mt);
    Node* child = new_node();
    child->parent = node;
    child->move = node->untriedMove[i];
    node->untriedMove.erase(node->untriedMove.begin() + i);
    node->child.push_back(child);
    return child;
}

bool input_map(map_data* data,int y, int x){
 if (y < 0 || y >= BOARD_SIZE || x < 0 || x >= BOARD_SIZE ||
     data->map[y][x] != STONE_SPACE){
     cout << "そのマスは打てません" << endl;
     return false;
 }
 return true;
}

void change_player_flag(map_data* data){
    if(data->player_flag == STONE_BLACK) data->player_flag =STONE_WHITE;
    else data->player_flag = STONE_BLACK;
}

void display_request_position(){
    cout <<"あなたのターンです"<< endl << "打つ場所 y x を入力してください" << endl;
}

void display_ai_position(int y, int x){
    cout << "相手のターンです"<< endl << y << " " << x << "に打ちました" << endl;
}



void input_point(map_data* data, int y, int x) {
    data->map[y][x] = data->player_flag;
}

void input_player_point(map_data* data){
    int y=-1,x=-1,my=-1,mx = -1;
    do{
        while(my == y && my == x)
        cin >> y >> x;
        my=y;mx=x;
    }while(!input_map(data, y, x));
    display_map(data);
    cout << y << " 　x=" << x << endl;
    input_point(data,y,x);
}
void create_untrie_Moves(map_data* data, Node* node) {
    for (int i=0; i <BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++) {
            if(data->map[i][j] == STONE_SPACE) {
                node->untriedMove.push_back(xy(i,j));
            }
        }
    }
}


Node* create_root_node(map_data* data){
    Node* node=new_node();
    create_untrie_Moves(data,node);
    return node;
}

double juege_len_check(map_data* data,int y,int x){
    int dx[] = { 0, 0,1,-1 ,1 ,-1, -1,1};
    int dy[] = { 1, -1,0,0 ,1 ,-1,1,-1};
    int len = 0;
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) len = 0;
        int j;
        for (j = 1; j <= 4; j++) {
            if(y+j*dy[i] >= BOARD_SIZE || x+j*dx[i] >= BOARD_SIZE) break;
            if (data->map[y][x] != data->map[y+j*dy[i]][x+j*dx[i]]) {
                break;
            }
            len++;
        }
        if (len >= 5)  return data->map[y][x];
    }
    return 0;
}

double juege(map_data* data, int y, int x){
    if (data->count == BOARD_SIZE * BOARD_SIZE) return 0.5;
    return juege_len_check(data, y, x);
}

double juege(map_data* data){
    if (data->count == BOARD_SIZE * BOARD_SIZE) return 0.5;
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++){
            if(data->map[i][j] != STONE_SPACE) {
                if (juege_len_check(data,i,j)){
                    return juege_len_check(data,i,j);
                }
            }
        }
    }
    return 0;
}

double simulate(map_data data, Node* node){
    random_device rnd;
    mt19937 mt(rnd());
    uniform_int_distribution<> rand(0,BOARD_SIZE-1);
    int game_player = data.player_flag;
    data.map[node->move.first][node->move.second] = game_player;
    change_player_flag(&data);
    game_player =data.player_flag;
    while(!juege(&data)){
        int counter=0;
        for (int i=0;i<BOARD_SIZE;i++){
            for(int j=0;j<BOARD_SIZE;j++) {
                if(data.map[i][j]==STONE_SPACE) counter++;
            }
        }
        uniform_int_distribution<> rand(0,counter);
        counter = rand(mt);
        int counter2 = 0;
        for (int i=0;i<BOARD_SIZE;i++){
            for(int j=0;j<BOARD_SIZE;j++) {
                if(data.map[i][j]==STONE_SPACE){
                    counter2++;
                    if(counter2==counter) {
                        data.count++;
                        data.map[i][j]=data.player_flag;
                        change_player_flag(&data);
                    }
                }
                
            }
        }
    }
   // display_map(&data);
    double ans = juege(&data);
    //cout << ans << " geme_player " << game_player << endl;
    if (ans == 0.5) return ans;
    else if(ans != game_player) return 1;
    return 0;
}

xy gomoku_ai(map_data* data){
    Node* root = create_root_node(data);
    for (int i= 0; i< MAX_TRIES;i++) {
        Node* node = root;
        while(node->untriedMove.size() == 0 && node->child.size() != 0)
            node = select_child_node(node);
        if (node->untriedMove.size() != 0)
            node = expand_child(node);
        double won = simulate(*data,node);
        back_propagate_node(node,won);
     //   cout << "node y= " << node->move.first << "  x= " << node->move.second << "  visits= " << node->visits << "  wins= " << node->wins <<  "won= " << won << endl;

    }
    int max_visits=0;
    Node* p;
    for(int i=0;i<root->child.size();i++){
        Node* child = root->child[i];
        if(child->visits > max_visits){
            p=child;
            max_visits = child->visits;
        }
     //   cout << "y= " << child->move.first << "  x= " << child->move.second << "  visits= " << child->visits << "  wins= " << child->wins << endl;

    }

    return p->move;
}
void display_winner(map_data* data){
    display_map(data);
    if(juege(data) == STONE_BLACK) cout << "●の勝利です" << endl;
    else if(juege(data) == STONE_WHITE) cout << "◯の勝利です" << endl;
    else cout << "引き分けです" << endl;
}

int main(void) {
    map_data* data = new map_data;
    init_map_data(data);
    cout << "先攻か後攻か選んでください\n先攻:1 後攻:2" << endl;
    int a=2;
    cin >> a;
    int game_player = STONE_BLACK;
    if (a==2)   game_player = STONE_WHITE;
    while(!juege(data)){
        display_map(data);
        if (data->player_flag == game_player){
            display_request_position();
            input_player_point(data);
        }
        else {
            xy tmp = gomoku_ai(data);
            input_point(data,tmp.first,tmp.second);
            display_ai_position(tmp.first,tmp.second);
        }
        change_player_flag(data);
        data->count++;
    }
    display_winner(data);
}
