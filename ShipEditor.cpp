#include "lab_m1/Tema1/ShipEditor.h"


#define MAX_NUMBER_BLOCKS 10
#define START_GRID_X 300
#define START_GRID_Y 100
#define GRID_SQUARE_SIDE 40
#define SPACE_SQUARES 10
#define GRID_ROWS 9
#define GRID_COLS 17
#define BLOCK_SQUARE_SIDE 50
#define TRIANGLE_MOTOR_HEIGHT 25

using namespace std;

ShipEditor::ShipEditor() 
{
	numberBlocks = MAX_NUMBER_BLOCKS;
	gridRows = GRID_ROWS;
	gridCols = GRID_COLS;
	spacing = SPACE_SQUARES;
	startX = START_GRID_X;
	startY = START_GRID_Y;
	blockSide = BLOCK_SQUARE_SIDE;
	squareSide = GRID_SQUARE_SIDE;

    grid = std::vector<std::vector<BlockType>>(gridRows, std::vector<BlockType>(gridCols, BlockType::EMPTY));

    blocks.clear();
    blocks.push_back({ BlockType::BLOCK, "block", glm::vec2(0), false, false });
    blocks.push_back({ BlockType::MOTOR, "motor", glm::vec2(0), false, false });
    blocks.push_back({ BlockType::TUN,   "tun",   glm::vec2(0), false, false });

    gridBlocks.clear();


    draggedBlock = { BlockType::EMPTY, "", glm::vec2(0), false, false };
}

ShipEditor::~ShipEditor()
{
}

glm::vec2 ShipEditor::fromPixelToWorld(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight) 
{

    float worldX = (float)pixel.x * (viewWidth  / (float)resX);
    float worldY = (float)(resY - pixel.y) * (viewHeight / (float)resY);

    return glm::vec2(worldX, worldY);
}

glm::ivec2 ShipEditor::fromPixelToGridPos(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight) 
{
	glm::vec2 worldPos = fromPixelToWorld(pixel, resX, resY, viewWidth, viewHeight);

    float worldX = worldPos.x;
    float worldY = worldPos.y;

    float gridLeft = startX - spacing / 2.0f;
    float gridRight = startX + GRID_COLS * (squareSide + spacing) - spacing / 2.0f;
    float gridBottom = startY - spacing / 2.0f;
    float gridTop = startY + GRID_ROWS * (squareSide + spacing) - spacing / 2.0f;

    if (worldX >= gridLeft && worldX < gridRight && worldY >= gridBottom && worldY < gridTop) {
        int col = (int)((worldX - startX + spacing / 2.0f) / (squareSide + spacing));
        int row = (int)((worldY - startY + spacing / 2.0f) / (squareSide + spacing));
        return glm::ivec2(GRID_ROWS - row - 1, col);
    }

    return glm::ivec2(-1, -1);
}

glm::ivec2 ShipEditor::fromGridPosToPixel(glm::ivec2 pos)
{
	int col = pos.y;
    int row = gridRows - 1 - pos.x;

    float pixelX = startX - spacing / 2.0f + col * (squareSide + spacing);
    float pixelY = startY - spacing / 2.0f + row * (squareSide + spacing);

    return glm::ivec2((int)pixelX, (int)pixelY);
}

bool ShipEditor::isBlockUnderMotor()
{
	int motorRow = -1;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] == BlockType::MOTOR && motorRow == -1) {
                motorRow = i;
            } else if (grid[i][j] != BlockType::EMPTY && i > motorRow && motorRow != -1) {
                return true;
            }
        }
    }
    return false;
}

bool ShipEditor::isBlockAboveTun() {
    int tunRow = -1;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] == BlockType::TUN) {
                tunRow = i;
                break;
            }
        }
        if (tunRow != -1) break;
    }

    if (tunRow == -1) return false;

    for (int i = 0; i < tunRow; i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] != BlockType::EMPTY) {
                return true;
            }
        }
    }
    return false;
}

bool ShipEditor::containsMotor() {
    for (int i = 0; i < gridRows; i++) {
        for (int j = 0; j < gridCols; j++) {
            if (grid[i][j] == BlockType::MOTOR) {
                return true;
            }
        }
    }
    return false;
}

bool ShipEditor::containsTun() {
    for (int i = 0; i < gridRows; i++) {
        for (int j = 0; j < gridCols; j++) {
            if (grid[i][j] == BlockType::TUN) {
                return true;
            }
        }
    }
    return false;
}

bool ShipEditor::isShipConex() {
    vector<vector<bool>> visited(gridRows, vector<bool>(gridCols, false));
    int components = 0;

    for (int i = 0; i < gridRows; i++) {
        for (int j = 0; j < gridCols; j++) {
            if (grid[i][j] != BlockType::EMPTY && !visited[i][j]) {
                components++;
                if (components > 1) {
                    // More than one connected component → not connected
                    return false;
                }

                // BFS to mark all blocks in this component as visited
                queue<pair<int, int>> q;
                q.push({i, j});
                visited[i][j] = true;

                int dx[] = {0, -1, 0, 1};
                int dy[] = {-1, 0, 1, 0};

                while (!q.empty()) {
                    pair<int,int> cell = q.front();
                    q.pop();
                    int row = cell.first;
                    int col = cell.second;

                    int dx[] = {0, -1, 0, 1};
                    int dy[] = {-1, 0, 1, 0};

                    for (int k = 0; k < 4; k++) {
                        int x = row + dx[k];
                        int y = col + dy[k];
                        if (x >= 0 && x < gridRows && y >= 0 && y < gridCols
                            && grid[x][y] != BlockType::EMPTY && !visited[x][y]) {
                            visited[x][y] = true;
                            q.push({x, y});
                        }
                    }
                }
            }
        }
    }

    return components == 1;
}

bool ShipEditor::hasShipMaxTenBlocks() {
    return numberBlocks >= 0;
}

bool ShipEditor::constraints()
{
    return ShipEditor::isShipConex() 
            && !ShipEditor::isBlockAboveTun() 
            && !ShipEditor::isBlockUnderMotor() 
            && ShipEditor::hasShipMaxTenBlocks()
            && ShipEditor::containsMotor()
            && ShipEditor::containsTun();
}

std::vector<BlockInfo> ShipEditor::getGridBlocks() const
{
    return gridBlocks;
}


void ShipEditor::OnMouseMove(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight)
{
     if (draggedBlock.isDragged) {
        glm::vec2 worldPos = fromPixelToWorld(glm::ivec2(mouseX, mouseY), resX, resY, viewWidth, viewHeight);
        glm::vec2 blockPosition = glm::vec2(worldPos.x - blockSide / 2, worldPos.y - blockSide / 2);
        draggedBlock.position = blockPosition;
    }
}

void ShipEditor::OnMouseBtnPress(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight, int button, float baseHeight)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = fromPixelToWorld(glm::ivec2(mouseX, mouseY), resX, resY, viewWidth, viewHeight);
        float containerSide = baseHeight / 3.0f;

        for (int i = 0; i < blocks.size(); i++) {
            
            if (worldPos.x >= 0 && worldPos.x <= containerSide 
            && worldPos.y >= containerSide * i && worldPos.y < containerSide * (i + 1)) {
                glm::vec2 blockPosition = glm::vec2(worldPos.x - blockSide / 2, worldPos.y - blockSide / 2);
                draggedBlock = blocks[i];
                draggedBlock.position = blockPosition;
                draggedBlock.isDragged = true;
                break;
            }
        }
    } else if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::ivec2 pos = fromPixelToGridPos(glm::ivec2(mouseX, mouseY), resX, resY, viewWidth, viewHeight);

        if (pos == glm::ivec2(-1, -1)) return;

        if (grid[pos.x][pos.y] == BlockType::EMPTY) return;

        int row = pos.x;
        int col = pos.y;

        if (grid[row][col] == BlockType::MOTOR || grid[row][col] == BlockType::BLOCK) {
            grid[row][col] = BlockType::EMPTY;
        } else {

            while (row > 0 && grid[row - 1][col] == BlockType::TUN) {
                glm::ivec2 pos = glm::ivec2(row - 1, col);
                glm::vec2 pixel = glm::vec2(fromGridPosToPixel(pos).x, fromGridPosToPixel(pos).y);
                auto it = find_if(gridBlocks.begin(), gridBlocks.end(),
                    [&](const BlockInfo& block) {
                        return block.position == pixel;
                    } 
                );
                if (it != gridBlocks.end()) break;
                row--;
            }
            for (int i = 0; i < 3; i++) {
                grid[row + i][col] = BlockType::EMPTY;
            }
            row += 2;
        }

        glm::ivec2 newPos = glm::ivec2(row, col);
        glm::vec2 blockPos = glm::vec2(fromGridPosToPixel(newPos).x, fromGridPosToPixel(newPos).y);

        gridBlocks.erase(
            std::remove_if(gridBlocks.begin(), gridBlocks.end(),
                [&](const BlockInfo& b) {
                    return b.position.x == blockPos.x && b.position.y == blockPos.y;        
                } 
            ),
            gridBlocks.end()
        );

        numberBlocks++;
    }
}

void ShipEditor::OnMouseBtnRelease(int mouseX, int mouseY, int resX, int resY, float viewWidth, float viewHeight, int button)
{
     if (button == GLFW_MOUSE_BUTTON_RIGHT && draggedBlock.isDragged) {
        glm::vec2 worldPos = fromPixelToWorld(glm::ivec2(mouseX, mouseY), resX, resY, viewWidth, viewHeight);
        glm::ivec2 gridPos = fromPixelToGridPos(glm::ivec2(mouseX, mouseY), resX, resY, viewWidth, viewHeight);

        if (gridPos != glm::ivec2(-1, -1) && grid[gridPos.x][gridPos.y] == BlockType::EMPTY) {
            glm::ivec2 blockPosition = fromGridPosToPixel(gridPos); 
            draggedBlock.position = glm::vec2(blockPosition.x, blockPosition.y);
            if (draggedBlock.meshName == "block" || draggedBlock.meshName == "motor") {
                draggedBlock.isPlaced = true;
                grid[gridPos.x][gridPos.y] = draggedBlock.type;
                gridBlocks.push_back(draggedBlock);
                numberBlocks--;

            } else if (draggedBlock.meshName == "tun" && gridPos.x >= 2) {
                draggedBlock.isPlaced = true;
                for (int i = 0; i < 3; i++) {
                    grid[gridPos.x - i][gridPos.y] = draggedBlock.type;
                }
                gridBlocks.push_back(draggedBlock);
                numberBlocks--;
            }
        }
        draggedBlock.isDragged = false;
        draggedBlock.isPlaced = false;
    }
}