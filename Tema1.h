#pragma once
#include <vector>
#include <string>
#include "components/simple_scene.h"
#include "lab_m1/Tema1/ShipEditor.h"
#include "lab_m1/Tema1/ChickenInvaders.h"
#include "components/text_renderer.h"
namespace m1
{
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

    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void drawGrid();
        void drawTopSquares();
        void drawStartButton();
        void drawBlockContainers();
        void drawBlocks();
        void drawDraggedBlock();
        void drawGridBlocks();

        void drawSpaceShip();
        void drawChickens();
        void drawBullets();
        void drawEggs();
        void drawHeartsContainer();
        void renderScoreText();
        void renderRoundText();
        void renderFinalScore();
       
        float baseHeight;
        float baseWidth;

        float viewWidth;
        float viewHeight;

        ShipEditor shipEditor;
        ChickenInvaders game;        

        bool hasGameStarted;
        float scaleFactor;

        gfxc::TextRenderer* textRenderer;

    };
}


