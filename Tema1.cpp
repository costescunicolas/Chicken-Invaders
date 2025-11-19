#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <queue>
#include <iostream>
#include "lab_m1/Tema1/transform.h"
#include "lab_m1/Tema1/mesh2D.h"

#define MAX_NUMBER_BLOCKS 10
#define START_GRID_X 300
#define START_GRID_Y 100
#define GRID_SQUARE_SIDE 40
#define SPACE_SQUARES 10
#define GRID_ROWS 9
#define GRID_COLS 17
#define BLOCK_SQUARE_SIDE 50
#define TRIANGLE_MOTOR_HEIGHT 25
#define BASE_SCREEN_X 1280
#define BASE_SCREEN_Y 720
#define SCALE_FACTOR 0.75f
#define HEART_SPACING 30.0f

using namespace std;
using namespace m1;

Tema1::Tema1() {}
Tema1::~Tema1() {
    delete textRenderer;
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();

    // --- Virtual coordinate system (1280x720 base) ---
    baseWidth = BASE_SCREEN_X;
    baseHeight = BASE_SCREEN_Y;

    float windowAspect = (float)resolution.x / (float)resolution.y;
    float baseAspect = (float)BASE_SCREEN_X / (float)BASE_SCREEN_Y;

    auto camera = GetSceneCamera();

    if (windowAspect >= baseAspect) {
        viewHeight = baseHeight;
        viewWidth = baseHeight * windowAspect;
        camera->SetOrthographic(0, viewWidth, 0, viewHeight, 0.01f, 400);
    } else {
        viewWidth = baseWidth;
        viewHeight = baseWidth / windowAspect;
        camera->SetOrthographic(0, viewWidth, 0, viewHeight, 0.01f, 400);
    }

    hasGameStarted = false;
    scaleFactor = SCALE_FACTOR; 

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, viewWidth, viewHeight);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, "assets", "fonts", "Hack-Bold.ttf"), 30);

    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    GetCameraInput()->SetActive(false);
    glDisable(GL_DEPTH_TEST);
    

    // --- Initialize grid and blocks ---
    shipEditor = ShipEditor();

    // --- Create meshes (in logical world units) ---
    AddMeshToList(mesh2D::CreateSquare("square", glm::vec3(0, 0, 0), GRID_SQUARE_SIDE, glm::vec3(0, 0, 1), true));
    AddMeshToList(mesh2D::CreateSquare("topSquare", glm::vec3(0, 0, 0), BLOCK_SQUARE_SIDE, glm::vec3(0, 1, 0), true));
    AddMeshToList(mesh2D::CreateBlock("block", BLOCK_SQUARE_SIDE));
    AddMeshToList(mesh2D::CreateStartButton("startButton", BLOCK_SQUARE_SIDE, shipEditor.constraints() ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)));
    AddMeshToList(mesh2D::CreateMotor("motor", BLOCK_SQUARE_SIDE, TRIANGLE_MOTOR_HEIGHT));
    AddMeshToList(mesh2D::CreateTun("tun", BLOCK_SQUARE_SIDE));
    AddMeshToList(mesh2D::CreateSquare("blockContainer", glm::vec3(0, 0, 0), BASE_SCREEN_Y / 3, glm::vec3(1, 0, 0), false));
    AddMeshToList(mesh2D::CreateChicken("chicken", scaleFactor));
    AddMeshToList(mesh2D::CreateBullet("bullet", BLOCK_SQUARE_SIDE * scaleFactor, glm::vec3(1, 1, 0)));
    AddMeshToList(mesh2D::CreateEgg("egg", BLOCK_SQUARE_SIDE * 0.7f * scaleFactor, BLOCK_SQUARE_SIDE * scaleFactor));
    AddMeshToList(mesh2D::CreateHeartsContainer("heartsContainer"));
    AddMeshToList(mesh2D::CreateHeart("heartFull", 25.0f, glm::vec3(1, 0, 0), true));
    AddMeshToList(mesh2D::CreateHeart("heartEmpty", 25.0f, glm::vec3(1, 0, 0), false));
}

void Tema1::drawGrid()
{
    for (int i = 0; i < shipEditor.gridRows; i++) {
        for (int j = 0; j < shipEditor.gridCols; j++) {
            float x = shipEditor.startX + j * (shipEditor.squareSide + shipEditor.spacing);
            float y = shipEditor.startY + i * (shipEditor.squareSide + shipEditor.spacing);

            glm::mat3 modelMatrix = glm::mat3(1);
            modelMatrix *= transform::Translate(x, y);
            RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
        }
    }
}

void Tema1::drawTopSquares()
{
    float x = shipEditor.startX;
    float y = shipEditor.startY + shipEditor.gridRows * (shipEditor.squareSide + shipEditor.spacing) + 50; 

    for (int i = 0; i < shipEditor.numberBlocks; i++) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(x + i * (shipEditor.blockSide + 25), y);

        RenderMesh2D(meshes["topSquare"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawStartButton()
{
    float x = shipEditor.startX;
    float y = shipEditor.startY + shipEditor.gridRows * (shipEditor.squareSide + shipEditor.spacing) + 50;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform::Translate(x + MAX_NUMBER_BLOCKS * (shipEditor.blockSide + 25), y);
    RenderMesh2D(meshes["startButton"], shaders["VertexColor"], modelMatrix);
}


void Tema1::drawBlockContainers()
{
    float containerSide = baseHeight / 3.0f;

    for (int i = 0; i < shipEditor.blocks.size(); i++) {
        float posY = i * containerSide;

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(0, posY);

        RenderMesh2D(meshes["blockContainer"], shaders["VertexColor"], modelMatrix);
        shipEditor.blocks[i].position = glm::vec2(containerSide / 2.0f, posY + containerSide / 2.0f);
    }
}

void Tema1::drawBlocks()
{
    for (auto& block : shipEditor.blocks) {
        glm::mat3 modelMatrix = glm::mat3(1);

        float offsetX = BLOCK_SQUARE_SIDE / 2.0f;
        float offsetY = BLOCK_SQUARE_SIDE / 2.0f;

        if (block.meshName == "tun")
            offsetY += BLOCK_SQUARE_SIDE;

        modelMatrix *= transform::Translate(block.position.x - offsetX, block.position.y - offsetY);
        RenderMesh2D(meshes[block.meshName], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawGridBlocks() {
    for (auto& block : shipEditor.gridBlocks) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(block.position.x, block.position.y);
        RenderMesh2D(meshes[block.meshName], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawDraggedBlock() {
     if (shipEditor.draggedBlock.isDragged || shipEditor.draggedBlock.isPlaced) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(shipEditor.draggedBlock.position.x, shipEditor.draggedBlock.position.y);
        RenderMesh2D(meshes[shipEditor.draggedBlock.meshName], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawSpaceShip()
{
    if (game.shipBlocks.empty()) return;
    
   
    for (const auto& block : game.shipBlocks) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(block.position.x, block.position.y);
        modelMatrix *= transform::Scale(scaleFactor, scaleFactor);
        
        RenderMesh2D(meshes[block.meshName], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawChickens()
{
    for (auto& c : game.chickens) {
        if (!c.alive) continue;

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(c.position.x, c.position.y);

        RenderMesh2D(meshes["chicken"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::drawBullets()
{
     for (auto& b : game.bullets) {
        if (!b.active) continue;

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(b.position.x, b.position.y);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawEggs()
{   
    for (auto& e : game.eggs) {
        if (!e.active) continue;
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform::Translate(e.position.x, e.position.y);
        RenderMesh2D(meshes["egg"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::drawHeartsContainer()
{   

    glm::mat3 modelMatrix = glm::mat3(1);
    
    modelMatrix *= transform::Translate(viewWidth - 130, viewHeight - 35);
    RenderMesh2D(meshes["heartsContainer"], shaders["VertexColor"], modelMatrix);

    float heartSpacing = HEART_SPACING;
    float startX = viewWidth - 120;

    for (int i = 0; i < game.maxLives; i++) {
        string meshName = (i < game.maxLives - game.currentLives) ? "heartEmpty" : "heartFull";
        glm::mat3 heartMatrix = glm::mat3(1);
        heartMatrix *= transform::Translate(startX + i * heartSpacing, viewHeight - 30);
        RenderMesh2D(meshes[meshName], shaders["VertexColor"], heartMatrix);
    }
}

void Tema1::renderScoreText()
{
    string scoreText = "Score: " + to_string(game.score);
    textRenderer->RenderText(scoreText, 0.0f, 0.0f, 1.0f, glm::vec3(1, 1, 1));
}

void Tema1::renderRoundText()
{
    string roundText = "Round: " + to_string(game.round);
    textRenderer->RenderText(roundText, 190.0f, 0.0f, 1.0f, glm::vec3(1, 1, 1));
}

void Tema1::renderFinalScore()
{
    string loseText = "You LOST!";
    string scoreText = "Your final score is " + to_string(game.score);
    string editorText = "Press SPACE to go back to editor";
    textRenderer->RenderText(loseText, viewWidth / 2 - 300, viewHeight / 2 - 200, 3.0f, glm::vec3(1, 0, 0));
    textRenderer->RenderText(scoreText, viewWidth / 2 - 400, viewHeight / 2 - 100, 2.0f, glm::vec3(1, 0, 0));
    textRenderer->RenderText(editorText, viewWidth / 2 - 600, viewHeight / 2, 2.0f, glm::vec3(1, 0, 0));
}

// --- Convert pixel (window) → grid position (in world space) ---
void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
    if (!hasGameStarted) {
        drawGrid();
        drawTopSquares();
        meshes["startButton"] = mesh2D::CreateStartButton(
            "startButton",
            BLOCK_SQUARE_SIDE,
            shipEditor.constraints() ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)
        );
        drawStartButton();
        drawBlockContainers();
        drawBlocks();
        drawDraggedBlock();
        drawGridBlocks();
    } else if (!game.gameEnded()){
        renderScoreText();
        renderRoundText();
        game.updateBullets(deltaTimeSeconds, viewHeight);
        drawSpaceShip();
        drawChickens();
        game.updateChickens(deltaTimeSeconds);
        drawBullets();
        drawEggs();
        game.updateEggs(deltaTimeSeconds);
        drawHeartsContainer();
        game.resetChickens(deltaTimeSeconds); 
    } else {
        renderFinalScore();
    }
}

void Tema1::FrameEnd() {}

void Tema1::OnInputUpdate(float deltaTime, int mods) 
{
    if (window->KeyHold(GLFW_KEY_LEFT)) game.moveShip(deltaTime, GLFW_KEY_LEFT, viewWidth, viewHeight);
    if (window->KeyHold(GLFW_KEY_RIGHT)) game.moveShip(deltaTime, GLFW_KEY_RIGHT, viewWidth, viewHeight);
    if (window->KeyHold(GLFW_KEY_UP)) game.moveShip(deltaTime, GLFW_KEY_UP, viewWidth, viewHeight);
    if (window->KeyHold(GLFW_KEY_DOWN)) game.moveShip(deltaTime, GLFW_KEY_DOWN, viewWidth, viewHeight);
}
void Tema1::OnKeyPress(int key, int mods) 
{
    if (key == GLFW_KEY_SPACE && hasGameStarted) {
        if (game.gameEnded()) {
            hasGameStarted = false;
        } else {
            game.shoot();
        }
    } 
}
void Tema1::OnKeyRelease(int key, int mods) {}
void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) 
{
    shipEditor.OnMouseMove(mouseX, mouseY, window->GetResolution().x, window->GetResolution().y, viewWidth, viewHeight);
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    glm::vec2 worldPos = shipEditor.fromPixelToWorld(glm::ivec2(mouseX, mouseY),
                                                     window->GetResolution().x,
                                                     window->GetResolution().y,
                                                     viewWidth, viewHeight);

    float startButtonX = shipEditor.startX + MAX_NUMBER_BLOCKS * (shipEditor.blockSide + 25);
    float startButtonY = shipEditor.startY + shipEditor.gridRows * (shipEditor.squareSide + shipEditor.spacing) + 50;

    float buttonSize = BLOCK_SQUARE_SIDE;

    if (!hasGameStarted &&
        worldPos.x >= startButtonX && worldPos.x <= startButtonX + buttonSize &&
        worldPos.y >= startButtonY && worldPos.y <= startButtonY + buttonSize)
    {
        game = ChickenInvaders(shipEditor.gridBlocks, scaleFactor);
        hasGameStarted = true;
        return; 
    }

    shipEditor.OnMouseBtnPress(mouseX, mouseY, window->GetResolution().x, window->GetResolution().y, viewWidth, viewHeight, button, baseHeight);
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) 
{
    shipEditor.OnMouseBtnRelease(mouseX, mouseY, window->GetResolution().x, window->GetResolution().y, viewWidth, viewHeight, button);
}
void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Tema1::OnWindowResize(int width, int height)
{
    float windowAspect = (float)width / (float)height;
    float baseAspect = (float)BASE_SCREEN_X / (float)BASE_SCREEN_Y;

    auto camera = GetSceneCamera();

    if (windowAspect >= baseAspect) {
        viewHeight = baseHeight;
        viewWidth  = baseHeight * windowAspect;
        camera->SetOrthographic(0, viewWidth, 0, viewHeight, 0.01f, 400);
    } else {
        viewWidth  = baseWidth;
        viewHeight = baseWidth / windowAspect;
        camera->SetOrthographic(0, viewWidth, 0, viewHeight, 0.01f, 400);
    }

    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    glViewport(0, 0, width, height);
}
