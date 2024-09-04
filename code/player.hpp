/**
 * @file player.h
 * @author yangboyang@jisuanke.com
 * @copyright jisuanke.com
 * @date 2021-07-01
 */

#include <string.h>
#include "../include/playerbase.h"
#include <stdio.h>
#include <limits.h> // For INT_MIN, INT_MAX
#include <stdlib.h>
#include <vector>

//抢角
//危险位
//稳定子
//边界位

using namespace std;

int totalSearchTimes = 0;
int maxSearchDepth = 0;
int searchDepth = 0;
bool IsFirst;

constexpr int step[8][2] = {0, 1, 0, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1, -1, -1, 1};

bool isFirst(struct Player *player) {
    int cnt = 0;
    for(int i = 0; i < player->row_cnt; i++) {
        for(int j = 0; j < player->col_cnt; j++) {
            if(player->mat[i][j] == 'o' || player->mat[i][j] == 'O')
                cnt++;
        }
    }
    if(cnt == 4) {
        return true;
    } else {
        return false;
    }
}


void init(struct Player *player) {
        // This function will be executed at the begin of each game, only once.
}
int is_valid(struct Player *player, int posx, int posy) {
    if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt) {
        return false;
    }
    if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O') {
        return false;
    }
    for (int dir = 0;  dir < 8; dir++) {
        int x = posx + step[dir][0];
        int y = posy + step[dir][1];
        if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt) {
            continue;
        }
        if (player->mat[x][y] != 'o') {
            continue;
        }
        while (true) {
            x += step[dir][0];
            y += step[dir][1];
            if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9')) {
                break;
            }
            if (player->mat[x][y] == 'O') {
                return true;
            }
        }
    }
    return false;
}

int is_valid_computer(struct Player *player, int posx, int posy) {
    if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt) {
        return false;
    }
    if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O') {
        return false;
    }
    for (int dir = 0;  dir < 8; dir++) {
        int x = posx + step[dir][0];
        int y = posy + step[dir][1];
        if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt) {
            continue;
        }
        if (player->mat[x][y] != 'O') {
            continue;
        }
        while (true) {
            x += step[dir][0];
            y += step[dir][1];
            if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9')) {
                break;
            }
            if (player->mat[x][y] == 'o') {
                return true;
            }
        }
    }
    return false;
}




bool haveCorner(struct Player *player) {
    int width = player->col_cnt;
    return player->mat[0][0] == 'O'
    || player->mat[0][width - 1] == 'O'
    || player->mat[width - 1][0] == 'O'
    || player->mat[width - 1][width - 1] == 'O';
}
bool isStable(struct Player *player, int posx, int posy, char playerSymbol, char opponentSymbol) {
    // 稳定子的定义：在当前位置放置棋子后，无论对手如何落子，该位置的棋子都不会被翻转。
    bool directionStable[4] = {false, false, false, false};
    // 检查四个方向（水平、垂直、两个对角线）是否稳定
    for (int i = 0; i < 8; i++) {
        int x = posx, y = posy;
        bool opponentFound = false;
        //没到边界  没找到对手
        while (true) {
            x += step[i][0];
            y += step[i][1];
            // 边界检查
            if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt) {
                directionStable[i/2] = true;
                break;             //跳出循环
            }
            char current = player->mat[x][y];
            if (current >= '1' && current <= '9') {
                // 空位或特殊标记，不稳定
                break;
            }
            if (current == opponentSymbol) {
                opponentFound = true;
            } else if (opponentFound && current == playerSymbol) {
                // 找到对手棋子后又找到自己的棋子，形成夹击，当前方向稳定
                directionStable[i/2] = true;
                break;
            } else {
                // 没有找到对手棋子，直接找到自己的棋子，当前方向不稳定
                break;
            }
        }

    }
    // 所有方向检查完毕，均稳定
    if(directionStable[0]
    && directionStable[1]
    && directionStable[2]
    && directionStable[3])
    return true;
}


//重新制定策略

//是否可抢角corner   return 100 return 0
//计算前沿棋子   return 100 * (myF - opF) / (myF + opF + 1);
//机动性差异mobility return 100 * (myMoveCount - opMoveCount) / (myMoveCount + opMoveCount + 1); 可行动力差异
//stability 稳定棋子数量差异   return 100 * (myS - opS) / (myS + opS + 1);
//评分差异 return myW - opW;
//棋子数量差异： return 100 * (mySC - opSC) / (mySC + opSC + 1);



//机动性
int mobility(struct Player *player){
    int width = player->col_cnt;
    int myMoveCount = 0;
    int opMoveCount = 0;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < width; j++){
            if(is_valid(player, i, j)){
                myMoveCount++;
            }
            if(is_valid_computer(player, i, j)){
                opMoveCount++;
            }
        }
    }
    return 100 * (myMoveCount - opMoveCount) / (myMoveCount + opMoveCount + 1);
}

//角
int evalCorner(struct Player *player){
    int width = player->col_cnt;
    int myCorner = 0;
    int opCorner = 0;
    for(int i = 0; i < width; i+=width-1){
        for(int j = 0; j < width; j+=width-1){
            if(player->mat[i][j] == 'O'){
                myCorner++;
            }
            if(player->mat[i][j] == 'o'){
                opCorner++;
            }
        }
    }
    return 100 * (myCorner - opCorner) / (myCorner + opCorner + 1);
}


int evalDiscDiff(struct Player *player){
    int width = player->col_cnt;
    int myDisc = 0;
    int opDisc = 0;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < width; j++){
            if(player->mat[i][j] == 'O'){
                myDisc++;
            }
            if(player->mat[i][j] == 'o'){
                opDisc++;
            }
        }
    }
    return 100 * (myDisc - opDisc) / (myDisc + opDisc + 1);
}

int evalFrontierDiscs(struct Player *player){
    int width = player->col_cnt;
    int height = player->row_cnt; // 假设您的Player结构体中也有行数信息
    int frontierDiscs = 0;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(player->mat[i][j] == 'O'){ // 如果是任一玩家的棋子
                // 检查周围8个位置
                for(int di = -1; di <= 1; di++){
                    for(int dj = -1; dj <= 1; dj++){
                        if(di == 0 && dj == 0) continue; // 跳过自己的位置
                        int ni = i + di;
                        int nj = j + dj;
                        // 检查新位置是否在棋盘内
                        if(ni >= 0 && ni < height && nj >= 0 && nj < width){
                            if(player->mat[ni][nj] == ' '){ // 如果周围有空位
                                frontierDiscs++; // 当前棋子是前沿棋子
                                goto nextDisc; // 找到空位后，跳到下一个棋子
                            }
                        }
                    }
                }
            }
            nextDisc:; // 标签用于跳出嵌套循环
        }
    }

    return frontierDiscs;
}

bool isCorner(struct Player *player, int i, int j){
    int width = player->col_cnt;
    return ((i == 0 || i == width - 1)
           && (j == 0 || j == width - 1));
}

bool isEdge(int i, int j, int width)
{
    return ((i == 0 || i == width - 1)
    && (j == 0 || j == width - 1));
}

//评估函数
int evaluate_map(struct Player *player)
{
        int width = player->col_cnt;
        int score = 0;
    int weight_DiscDiff = 10;
    int weight_FrontierDiscs = -10;
    int weight_Mobility = 10;
    int weight_Corner = 100;
    int weight_Stability = 80;
    int weight_edge = 20;

    int count = 0;

    int my_stable = 0;  //稳定点
    int op_stable = 0;
    int my_corner = 0;   //角
    int op_corner = 0;
    int my_mobility = 0;   //机动性
    int op_mobility = 0;
    int my_edge = 0;  //边
    int op_edge = 0;
    int my_weight = 0;   //权         暂时未处理
    int op_weight = 0;
    int my_frontier = 0;  //前沿子
    int op_frontier = 0;
    int my_num = 0;      //棋子
    int op_num = 0;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if(isStable(player, i, j, 'O', 'o'))
            {
                my_stable++;
            }
            if(isStable(player, i, j, 'o', 'O'))
            {
                op_stable++;
            }
            if(player->mat[i][j] == 'O')
            {
                count++;
                my_num++;
                if(isCorner(player, i, j))
                {
                    my_corner++;
                }
                if(isEdge(i, j, width))
                {
                    my_edge++;
                }
                for(int dx = -1; dx <= -1; dx++)
                {
                    for(int dy = -1; dy <= 1; dy++)
                    {
                        if(dx == 0 && dy == 0) continue;
                        int nx = i + dx;
                        int ny = j + dy;
                        if(nx >= 0 && nx < width && ny >= 0 && ny < width)
                        {
                            if(player->mat[nx][ny] <= '9' && player->mat[nx][ny] >= '1')
                            {
                                my_frontier++;
                            }
                        }
                    }
                }
            }
            if(player->mat[i][j] == 'o')
            {
                count++;
                op_num++;
                if(isCorner(player, i, j))
                {
                    op_corner++;
                }
                if(isEdge(i, j, width))
                {
                    op_edge++;
                }
                for(int dx = -1; dx <= -1; dx++)
                {
                    for(int dy = -1; dy <= 1; dy++)
                    {
                        if(dx == 0 && dy == 0) continue;
                        int nx = i + dx;
                        int ny = j + dy;
                        if(nx >= 0 && nx < width && ny >= 0 && ny < width)
                        {
                            if(player->mat[nx][ny] <= '9' && player->mat[nx][ny] >= '1')
                            {
                                op_frontier++;
                            }
                        }
                    }
                }
            }
            if(is_valid(player, i, j))
            {
                my_mobility++;
            }
            if(is_valid_computer(player, i, j))
            {
                op_mobility++;
            }
        }
    }
    int frontier =  100 * (my_frontier - op_frontier) / (my_frontier + op_frontier + 1);  //前沿子
    int corner =  100 * (my_corner - op_corner);               //角
    int stablity =  100 * (my_stable - op_stable) / (my_stable + op_stable + 1);  //稳定点
    int edge =  100 * (my_edge - op_edge) / (my_edge + op_edge + 1);   //边界位
    int mobility =  100 * (my_mobility - op_mobility) / (my_mobility + op_mobility + 1);  //可行动力差异
    int num =  100 * (my_num - op_num) / (my_num + op_num + 1);                 //棋子数量差异
    if(count < width * width / 3)
    {
        score =  10 * frontier + 30 * corner + 10 * stablity + 10 * edge + 40 * mobility;
    }else if(count < width * width * 2 / 3)
    {
        score =  20 * frontier + 25 * corner + 15 * stablity + 10 * edge + 30 * mobility;
    }else
    {
        if(IsFirst){
            score =  25 * frontier + 25 * corner + 20 * stablity + 10 * edge + 20 * mobility + 20 * num;  }  //fixhere for number
        else {
            score =  25 * frontier + 25 * corner + 20 * stablity + 10 * edge + 20 * mobility;
        }
    }
    return score;
}







int max(int a, int b){
        return a > b ? a : b;
}

int min(int a, int  b){
        return a < b ? a : b;
}

struct Move {
    int x, y;
    char prevVal;
};

struct MoveHistory {
    struct Move *moves;
    int size;
    int capacity;
};




void initMoveHistory(struct MoveHistory& moveHistory) {
    moveHistory.moves = (struct Move*)malloc(sizeof(struct Move) * 100); // 初始容量
    moveHistory.size = 0;
    moveHistory.capacity = 100;
}

void pushMove(int x, int y, char prevVal, struct MoveHistory& moveHistory) {
    if (moveHistory.size == moveHistory.capacity) {
        moveHistory.capacity *= 2;
        moveHistory.moves = (struct Move*)realloc(moveHistory.moves, sizeof(struct Move) * moveHistory.capacity);
    }
    moveHistory.moves[moveHistory.size++] = (struct Move){x, y, prevVal};
}

void undoMove(struct Player *player,struct MoveHistory& moveHistory) {
    if (moveHistory.size == 0) return;
        // i = moveHistory.moves[0].x;
    // 逆转最后一次移动的所有变化
    for (int k = moveHistory.size - 1; k >= 0; k--) {
        int x = moveHistory.moves[k].x;
        int y = moveHistory.moves[k].y;
        char prevVal = moveHistory.moves[k].prevVal;
        player->mat[x][y] = prevVal; // 恢复之前的值
    }

    // 重置历史记录
    // moveHistory.size = 0;
        free(moveHistory.moves);
}

// 记得在适当的时候调用 initMoveHistory() 初始化历史记录栈

void makeMove(struct Player *player, int posx, int posy, char playerSymbol, struct MoveHistory& moveHistory) {
        initMoveHistory(moveHistory);
        pushMove(posx, posy, player->mat[posx][posy], moveHistory);        //将放棋子前位置上的分数push进栈
    // 放置棋子
    player->mat[posx][posy] = playerSymbol; //放
        // pushMove(posx, posy, )
    int step[8][2] = {0, 1, 0, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1, -1, -1, 1};   //8个方向
    for (int dir = 0; dir < 8; dir++) {
        int x = posx + step[dir][0];   //
        int y = posy + step[dir][1];   //相邻坐标
        bool valid = false;  //可翻转？

        // 检查是否有棋子可以翻转
        while (x >= 0 && x < player->row_cnt && y >= 0 && y < player->col_cnt) {
            if (player->mat[x][y] == playerSymbol) {
                valid = true;
                break;
            } else if (player->mat[x][y] == ' ' || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9')) {
                break;
            }
            x += step[dir][0];
            y += step[dir][1];
        }

        // 翻转棋子
        if (valid) {
            x = posx + step[dir][0];
            y = posy + step[dir][1];
            while (player->mat[x][y] != playerSymbol) {
                pushMove(x, y, player->mat[x][y], moveHistory);        //将被反转的棋子push进栈
                player->mat[x][y] = playerSymbol;
                x += step[dir][0];
                y += step[dir][1];
            }
        }
    }
}


//模拟自己和对手的走棋
int minimax(struct Player *player, int depth, bool isMax, int alpha, int beta) {
    if (depth == 0) {
        totalSearchTimes++;
        return evaluate_map(player);
    }
    totalSearchTimes++;
    if(totalSearchTimes > maxSearchDepth){
        return evaluate_map(player);
    }

    if (isMax) {
        int maxEval = INT_MIN;
        // 遍历所有可能的移动
        for (int i = 0; i < player->row_cnt; i++) {
            for (int j = 0; j < player->col_cnt; j++) {
                if (is_valid(player, i, j)) {

                                        struct MoveHistory moveHistory;
                    makeMove(player, i, j, 'O', moveHistory); // 假设 'O' 是当前玩家
                    int eval = minimax(player, depth - 1, false, alpha, beta);  //递归调用
                    undoMove(player,moveHistory); // 撤销移动
                    maxEval = max(maxEval, eval);
                    alpha = max(alpha, eval);
                    if (beta <= alpha) {
                        break; // alpha-beta 剪枝
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        // 遍历所有可能的移动
        for (int i = 0; i < player->row_cnt; i++) {
            for (int j = 0; j < player->col_cnt; j++) {
                if (is_valid_computer(player, i, j)) {
                    struct MoveHistory moveHistory;
                    makeMove(player, i, j, 'o', moveHistory); // 假设 'o' 是对手
                    int eval = minimax(player, depth - 1, true, alpha, beta);
                    undoMove(player, moveHistory); // 撤销移动
                    minEval = min(minEval, eval);
                    beta = min(beta, eval);
                    if (beta <= alpha) {
                        break; // alpha-beta 剪枝
                    }
                }
            }
        }
        return minEval;
    }
}




//int depth = 2;
int cnt = 0;

struct Point place(struct Player *player) {
    vector<struct Point> validMoves;
    if(cnt == 0) {
        IsFirst = isFirst(player);
    }
    if(player->row_cnt == 8 && cnt == 0) {
        maxSearchDepth = 10000;
        searchDepth = 5;
        cnt++;
    } else if(player->row_cnt == 10) {
        if(!IsFirst){
            maxSearchDepth = 3300;
            searchDepth = 2;
            cnt++;
        } else {
            maxSearchDepth = 3700;
            searchDepth = 5;
            cnt++;
        }

    } else {
        if(!IsFirst)
        {
            maxSearchDepth = 3800;
            searchDepth = 2;
            cnt++;
        } else {
            maxSearchDepth = 2900;
            searchDepth = 3;
            cnt++;
        }
    }
    //int idx = 0;
    struct Point bestMove = {-1, -1};  //最佳移动
    int bestValue = INT_MIN;       //胜率
    // if(player->row_cnt == 8){
    //     depth = 3;
    // }
    // if(player->row_cnt == 12) {
    //     idx = random() % 4 + 3;
    // }
//己方先模拟一步，然后对手模拟一步，然后己方模拟一步，然后对手模拟一步，直到模拟到最大深度，然后评估当前局面
//朝所有合法位置模拟己方
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {

            if (is_valid(player, i, j)) {
                validMoves.push_back(initPoint(i, j));
                // cnt++;

            // if(cnt == idx){

            // }
                struct MoveHistory moveHistory;
                makeMove(player, i, j, 'O', moveHistory); // 假设 'O' 是当前玩家



//轮到对手
                int moveVal = minimax(player, searchDepth, false, INT_MIN, INT_MAX); // 假设最大深度为 5

                undoMove(player, moveHistory); // 撤销移动
                if (moveVal > bestValue) {
                    bestMove = {i, j};   //最佳移动
                    // printf("bestMove %d %d\n", bestMove.X, bestMove.Y);
                    bestValue = moveVal;        //最佳胜率
                }
            }
        }
    }
    if(validMoves.size() != 0 && bestMove.X == -1 && bestMove.Y == -1){
        int index = rand() % validMoves.size();
        bestMove = validMoves[index];
    }
    totalSearchTimes = 0;
// printf("bestMove %d %d\n", bestMove.X, bestMove.Y);
    return bestMove;   //最佳胜率
}





// struct Point place(struct Player *player) {
//      struct Point *ok_points = (struct Point *)malloc((player->row_cnt * player->col_cnt) * sizeof(struct Point));


//      return initPoint(-1, -1);   // give up
// }


            // if(player->mat[i][j] == 'O'){  //中心控制
            //     if(i == width/2 && j == width/2
            //     || i == width/2 && j == width/2 + 1
            //     || i == width/2 + 1 && j == width/2
            //     || i == width/2 + 1 && j == width/2 + 1){
            //         score += 3;
            //     }
            //     if(isStable(player, i, j, 'O')){
            //         score += 20;   //稳定点
            //     }
            //     if(flag){
            //             if(width == 8)
            //             {
            //                 score += weight[i][j];
            //             }else if(width == 10)
            //             {
            //                 score += wieght_2[i][j];
            //             }else if(width == 12)
            //             {
            //                 score += weight_3[i][j];
            //             }
                // }
            // }
    // mob = 100*(myMoveCount - opMoveCount) / (myMoveCount + opMoveCount + 1);
    // dif = 100*(myDisc - opDisc) / (myDisc + opDisc + 1);
    // corner = 100*(myCorner - opCorner) / (myCorner + opCorner + 1);
    // score = 2*mob + dif;
    // int safe = isEdgeSafe(player);
    // score += safe*5;


                        // if(haveCorner(player) || enemyStar(player)){
                    //     return INT_MAX;
                    // }
                    // if(haveStar(player) && !haveCorner(player)){
                    //     return -100;
                    // }
                    // if(enemyCorner(player)){
                    //     return -500;
                    // }


                    // bool isStar(struct Player *player, int i, int j){
//     int width = player->col_cnt;
//     return (i == 0 && j == 1)
//     || (i == 1 && j == 0)
//     || (i == 1 && j == 1)
//     || (i == 0 && j == width-2)
//     || (i == 1 && j == width-1)
//     || (i == 1 && j == width-2)
//     || (i == width-2 && j == 0)
//     || (i == width-1 && j == 1)
//     || (i == width-2 && j == 1)
//     || (i == width-1 && j == width-2)
//     || (i == width-2 && j == width-1)
//     || (i == width-2 && j == width-2);
// }

// bool enemyStar(struct Player *player){
//     int width = player->col_cnt;
//     return (player->mat[0][1] == 'o'
//     || player->mat[1][0] == 'o'
//     || player->mat[1][1] == 'o'
//     || player->mat[0][width - 2] == 'o'
//     || player->mat[1][width - 1] == 'o'
//     || player->mat[1][width - 2] == 'o'
//     || player->mat[width - 2][0] == 'o'
//     || player->mat[width - 1][1] == 'o'
//     || player->mat[width - 2][1] == 'o'
//     || player->mat[width - 1][width-2] == 'o'
//     || player->mat[width - 2][width - 1] == 'o'
//     || player->mat[width - 2][width - 2] == 'o');
// }

// bool enemyCorner(struct Player *player){
//     int width = player->col_cnt;
//     return (player->mat[0][0] == 'o'
//     || player->mat[0][width - 1] == 'o'
//     || player->mat[width - 1][0] == 'o'
//     || player->mat[width - 1][width - 1] == 'o');
// }

// bool haveStar(struct Player *player){
//     int width = player->col_cnt;
//     return (player->mat[0][1] == 'O'
//     || player->mat[1][0] == 'O'
//     || player->mat[1][1] == 'O'
//     || player->mat[0][width - 2] == 'O'
//     || player->mat[1][width - 1] == 'O'
//     || player->mat[1][width - 2] == 'O'
//     || player->mat[width - 2][0] == 'O'
//     || player->mat[width - 1][1] == 'O'
//     || player->mat[width - 2][1] == 'O'
//     || player->mat[width - 1][width-2] == 'O'
//     || player->mat[width - 2][width - 1] == 'O'
//     || player->mat[width - 2][width - 2] == 'O');
// }

//边上危险位
// int isEdgeSafe(struct Player* play){
//     int safety = 0;
//     for(int i = 2; i < play->row_cnt - 2; i++){
//         if((play->mat[0][i-1] == 'o'
//         && play->mat[0][i] == 'O'
//         && play->mat[0][i+1] >= '1'
//         && play->mat[0][i+1] <= '9')
//         || (play->mat[0][i+1] == 'o'
//         && play->mat[0][i] == 'O'
//         && play->mat[0][i-1] >= '1'
//         && play->mat[0][i-1] <= '9')
//         || (play->mat[play->row_cnt-1][i-1] == 'o')
//         && play->mat[play->row_cnt-1][i] == 'O'
//         && play->mat[play->row_cnt-1][i+1] >= '1'
//         && play->mat[play->row_cnt-1][i+1] <= '9'
//         || (play->mat[play->row_cnt-1][i+1] == 'o'
//         && play->mat[play->row_cnt-1][i] == 'O'
//         && play->mat[play->row_cnt-1][i-1] >= '1'
//         && play->mat[play->row_cnt-1][i-1] <= '9')
//         || (play->mat[i-1][0] == 'o'
//         && play->mat[i][0] == 'O'
//         && play->mat[i+1][0] >= '1'
//         && play->mat[i+1][0] <= '9')
//         || (play->mat[i+1][0] == 'o'
//         && play->mat[i][0] == 'O'
//         && play->mat[i-1][0] >= '1'
//         && play->mat[i-1][0] <= '9')
//         || (play->mat[i-1][play->col_cnt-1] == 'o'
//         && play->mat[i][play->col_cnt-1] == 'O'
//         && play->mat[i+1][play->col_cnt-1] >= '1'
//         && play->mat[i+1][play->col_cnt-1] <= '9')
//         || (play->mat[i+1][play->col_cnt-1] == 'o'
//         && play->mat[i][play->col_cnt-1] == 'O'
//         && play->mat[i-1][play->col_cnt-1] >= '1'
//         && play->mat[i-1][play->col_cnt-1] <= '9')){
//             safety--;
//         }
//     }
//     return safety;
// }
