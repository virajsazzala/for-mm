#include <iostream>
#include <queue>
#include <utility>
#include <climits>
#include <algorithm>
#include <iomanip>
#include <cstdio>

#include "mms-api.h"
using namespace std;

#define MAZE_SIZE 16

int flood[MAZE_SIZE][MAZE_SIZE];                   // tracks distance to goal
int goal[4][2] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}}; // goal 2x2 (center)
int wall[MAZE_SIZE][MAZE_SIZE][4] = {};            // tracks wall locations (4 -> N E S W)
int dx[4] = {0, 1, 0, -1};                         // row -> dx, dy = dir (N E S W)
int dy[4] = {1, 0, -1, 0};                         // col -> dx, dy = dir
int opposite[] = {2, 3, 0, 1};                     // opposite (opp of N E S W)
int curr_dir = 0;
int last_dir;
std::pair<int, int> last_pos;
int mx = 0, my = 0;

void flood_fill();                                  // update distance values to goal
int next_move(int x, int y);                        // returns next dir to turn
bool update_walls(int x, int y, int dir);           // updates walls around a cell
void set_wall(int x, int y, int dir);               // sets a wall in a direction
bool at_goal(int x, int y, int (&curr_goal)[4][2]); // if at goal
void move_in_dir(int dir);                          // move in a dir

int main()
{
    while (!at_goal(mx, my, goal))
    {
        update_walls(mx, my, curr_dir);
        flood_fill();
        int dir = next_move(mx, my);
        move_in_dir(dir);
        curr_dir = dir;
    }

    std::fill(&goal[0][0], &goal[0][0] + 4 * 2, 0);
    while (!at_goal(mx, my, goal))
    {
        update_walls(mx, my, curr_dir);
        flood_fill();
        int dir = next_move(mx, my);
        move_in_dir(dir);
        curr_dir = dir;
    }

    int new_goal[4][2] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
    for (int m = 0; m < 4; m++)
    {
        for (int n = 0; n < 2; n++)
        {
            goal[m][n] = new_goal[m][n];
        }
    }
    while (!wasReset())
        ;
    flood_fill();
    while (!at_goal(mx, my, goal))
    {
        int did = update_walls(mx, my, curr_dir);
        if (did)
            flood_fill();
        setColor(mx, my, 'g');
        int dir = next_move(mx, my);
        move_in_dir(dir);
        curr_dir = dir;
    }

    return 0;
}

void move_in_dir(int dir)
{
    last_pos.first = mx;
    last_pos.second = my;
    last_dir = curr_dir;
    mx += dx[dir];
    my += dy[dir];
    int diff = (dir - curr_dir + 4) % 4;
    if (diff == 1)
        turnRight();
    if (diff == 3)
        turnLeft();
    if (diff == 2)
    {
        turnRight();
        turnRight();
    } // 180 turn
    // else dont turn
    moveForward();
}

int next_move(int x, int y)
{
    int min = INT_MAX;
    int best_dir = -1;

    for (int dir = 0; dir < 4; ++dir)
    {
        // curr pos + get direction = new cell direction
        int cx = x + dx[dir];
        int cy = y + dy[dir];

        int j = cx;
        int i = MAZE_SIZE - 1 - cy;
        int card_j = x;
        int card_i = MAZE_SIZE - 1 - y;

        if (cx >= 0 && cy >= 0 && cx < MAZE_SIZE && cy < MAZE_SIZE && card_i < MAZE_SIZE && card_j < MAZE_SIZE && !wall[card_i][card_j][dir])
        {
            // update shorter path
            int curr_val = flood[x][y];
            if (flood[cx][cy] < min)
            {
                min = flood[cx][cy];
                best_dir = dir;
            }
        }
    }

    return best_dir; // (n, e, s, w)
}

bool at_goal(int x, int y, int (&curr_goal)[4][2])
{
    for (int i = 0; i < 4; ++i)
    {
        if (curr_goal[i][0] == x && curr_goal[i][1] == y)
        {
            return true;
        }
    }

    return false;
}

bool update_walls(int x, int y, int face_dir)
{
    int front_dir = face_dir;
    int right_dir = (face_dir + 1) % 4;
    int back_dir = (face_dir + 2) % 4;
    int left_dir = (face_dir + 3) % 4;

    int wf = wallFront();
    int wr = wallRight();
    int wl = wallLeft();

    if (wf)
        set_wall(x, y, front_dir);
    if (wr)
        set_wall(x, y, right_dir);
    if (wl)
        set_wall(x, y, left_dir);

    return wf || wr || wl;
}

void set_wall(int x, int y, int dir)
{
    char cdir = "nesw"[dir];
    setWall(x, y, cdir);

    // normal side
    int j = x;
    int i = MAZE_SIZE - 1 - y;
    wall[i][j][dir] = 1;

    // opposite site
    int nx = x + dx[dir];
    int ny = y + dy[dir];

    if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE)
    {
        wall[MAZE_SIZE - 1 - ny][nx][opposite[dir]] = 1;
    }
}

void flood_fill()
{
    queue<pair<int, int>> q;

    // set all flood values to inf (or max)
    fill(&flood[0][0], &flood[0][0] + 16 * 16, INT_MAX);

    // set goal cell distance to 0
    for (int i = 0; i < 4; ++i)
    {
        flood[goal[i][0]][goal[i][1]] = 0;
        q.push({goal[i][0], goal[i][1]});
    }

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; ++dir)
        {
            // curr pos + get direction = new cell direction
            int cx = x + dx[dir];
            int cy = y + dy[dir];

            int j = cx;
            int i = MAZE_SIZE - 1 - cy;
            int card_j = x;
            int card_i = MAZE_SIZE - 1 - y;

            // in bounds && no wall
            if (cx >= 0 && cy >= 0 && cx < MAZE_SIZE && cy < MAZE_SIZE && card_i < MAZE_SIZE && card_i < MAZE_SIZE)
            {
                // update shorter path
                int curr_val = flood[x][y];
                if (flood[cx][cy] > curr_val + 1 && !wall[card_i][card_j][dir])
                {
                    flood[cx][cy] = curr_val + 1;
                    q.push({cx, cy});
                }
            }
        }
    }

    for (int k = 0; k < MAZE_SIZE; k++)
    {
        for (int l = 0; l < MAZE_SIZE; l++)
        {
            setText(k, l, to_string(flood[MAZE_SIZE - 1 - l][k]));
        }
    }
}
