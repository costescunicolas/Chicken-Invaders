#pragma once
#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include "lab_m1/Tema1/transform.h"
#include "lab_m1/Tema1/mesh2D.h"
#include "components/simple_scene.h"

enum class BlockType {
    EMPTY = 0,
    BLOCK = 1,
    MOTOR = 2,
    TUN = 3
};

struct BlockInfo {
    BlockType type;
    std::string meshName;
    glm::vec2 position;
    bool isDragged;
    bool isPlaced;
};

class ShipEditor {
public:
    ShipEditor();
    ~ShipEditor();

    // --- Logic ---
    bool isBlockUnderMotor();
    bool isBlockAboveTun();
    bool isShipConex();
    bool hasShipMaxTenBlocks();
    bool containsMotor();
    bool containsTun();
    bool constraints();


    // --- Coordinate conversions ---
    glm::vec2 fromPixelToWorld(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight);
    glm::ivec2 fromPixelToGridPos(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight);
    glm::ivec2 fromGridPosToPixel(glm::ivec2 pos);

    void OnMouseMove(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight);
    void OnMouseBtnPress(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight, int button, float baseHeight);
    void OnMouseBtnRelease(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight, int button);

    std::vector<BlockInfo> getGridBlocks() const;

    std::vector<std::vector<BlockType>> grid;
    std::vector<BlockInfo> blocks;
    std::vector<BlockInfo> gridBlocks;

    int numberBlocks;
    int gridRows;
    int gridCols;

    float squareSide;
    float spacing;
    float blockSide;
    float startX;
    float startY;

    BlockInfo draggedBlock;
};
