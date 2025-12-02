// Grid.cpp
#include "Grid.h"
#include <cstring>

int grid[M][N];
int trailOwner[M][N];

// Initialize the gamegrid
void initGrid()
{
    for (int r = 0; r < M; r++)
    {
        for (int c = 0; c < N; c++)
        {
            // By default, entire map water
            grid[r][c] = 0;
            trailOwner[r][c] = -1;
        }
    }

    // Mark borders as ground
    for (int c = 0; c < N; c++)
    {
        grid[0][c] = 1;
        grid[M - 1][c] = 1;
    }
    for (int r = 0; r < M; r++)
    {
        grid[r][0] = 1;
        grid[r][N - 1] = 1;
    }
}

// Copy entire grid from src -> dest
void copyGrid(int src[M][N], int dest[M][N])
{
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            dest[r][c] = src[r][c];
}

// Replace global grid with a restored one
void applyGrid(int restored[M][N])
{
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            grid[r][c] = restored[r][c];
}

// Serialize current grid to array
void serializeGrid(int out[M][N])
{
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            out[r][c] = grid[r][c];
}

// ---------------------------------------------------
// Deserialize array into grid
// ---------------------------------------------------
void deserializeGrid(int in[M][N])
{
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            grid[r][c] = in[r][c];
}

// ---------------------------------------------------
// Clear trailOwner
// ---------------------------------------------------
void clearTrailOwner()
{
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            trailOwner[r][c] = -1;
}

// ---------------------------------------------------
// Flood-fill for area claiming
// ---------------------------------------------------
void floodFill(int x, int y, int gridLocal[M][N])
{
    // x = column (0..N-1), y = row (0..M-1)
    if (x < 0 || x >= N || y < 0 || y >= M) return;
    if (gridLocal[y][x] != 0) return; // only fill water cells

    gridLocal[y][x] = 4; // mark as visited water

    floodFill(x + 1, y, gridLocal);
    floodFill(x - 1, y, gridLocal);
    floodFill(x, y + 1, gridLocal);
    floodFill(x, y - 1, gridLocal);
}
