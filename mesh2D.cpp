#include "mesh2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* mesh2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* mesh2D::CreateBlock(const std::string &name, float length) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 colorGray = glm::vec3(0.5f, 0.5f, 0.5f); 

    
    glm::vec3 bottomLeft(0.0f, 0.0f, 0.0f);
    glm::vec3 center = bottomLeft + glm::vec3(length / 2.0f, length / 2.0f, 0.0f);

    
    vertices.push_back(VertexFormat(center, glm::vec3(1, 1, 1)));                  
    vertices.push_back(VertexFormat(bottomLeft, colorGray));              
    vertices.push_back(VertexFormat(bottomLeft + glm::vec3(length, 0, 0), colorGray)); 
    vertices.push_back(VertexFormat(bottomLeft + glm::vec3(length, length, 0), colorGray)); 
    vertices.push_back(VertexFormat(bottomLeft + glm::vec3(0, length, 0), colorGray));      

     
    indices = {
        0, 1, 2,  
        0, 2, 3,  
        0, 3, 4,  
        0, 4, 1   
    };

    Mesh* block = new Mesh(name);
    block->InitFromData(vertices, indices);
    return block;
}



Mesh* mesh2D::CreateMotor(const std::string& name, float squareSide, float triangleHeight) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    
    glm::vec3 colorSquare = glm::vec3(1.0f, 0.65f, 0.0f);   
    glm::vec3 colorFlame  = glm::vec3(1.0f, 0.27f, 0.0f);   

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), colorSquare));
    vertices.push_back(VertexFormat(glm::vec3(squareSide, 0, 0), colorSquare));
    vertices.push_back(VertexFormat(glm::vec3(squareSide, squareSide, 0), colorSquare));
    vertices.push_back(VertexFormat(glm::vec3(0, squareSide, 0), colorSquare));

    indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });

    float triBase = squareSide / 3.0f;
    for (int i = 0; i < 3; i++) {
        float startX = i * triBase;
        unsigned int startIndex = vertices.size();

        vertices.push_back(VertexFormat(glm::vec3(startX, 0, 0), colorFlame));                
        vertices.push_back(VertexFormat(glm::vec3(startX + triBase, 0, 0), colorFlame));       
        vertices.push_back(VertexFormat(glm::vec3(startX + triBase / 2, -triangleHeight, 0), colorFlame)); 

        indices.insert(indices.end(), { startIndex, startIndex + 1, startIndex + 2 });
    }

    Mesh* motor = new Mesh(name);
    motor->InitFromData(vertices, indices);
    return motor;
}

Mesh* mesh2D::CreateTun(const std::string& name, float squareSide) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float width = squareSide;
    float baseHeight = 0.5f * squareSide;
    float radius = 0.5f * squareSide;
    float pipeHeight = 2.5f * squareSide;

    
    glm::vec3 colorBase = glm::vec3(0.9f, 0.9f, 0.9f);          
    glm::vec3 colorPipeBottom = glm::vec3(0.6f, 0.6f, 0.6f);    
    glm::vec3 colorPipeTop = glm::vec3(0.75f, 0.75f, 0.75f);   

    
    unsigned int baseStart = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), colorBase));
    vertices.push_back(VertexFormat(glm::vec3(width, 0.0f, 0.0f), colorBase));
    vertices.push_back(VertexFormat(glm::vec3(width, baseHeight, 0.0f), colorBase));
    vertices.push_back(VertexFormat(glm::vec3(0.0f, baseHeight, 0.0f), colorBase));

    indices.insert(indices.end(), {
        baseStart + 0, baseStart + 1, baseStart + 2,
        baseStart + 0, baseStart + 2, baseStart + 3
    });

    // ===== Pipe (barrel) =====
    unsigned int pipeStart = vertices.size();
    float pipeBottomY = baseHeight;
    float pipeTopY = pipeBottomY + pipeHeight;

    vertices.push_back(VertexFormat(glm::vec3(0.0f, pipeBottomY, 0.0f), colorPipeBottom));
    vertices.push_back(VertexFormat(glm::vec3(width, pipeBottomY, 0.0f), colorPipeBottom));
    vertices.push_back(VertexFormat(glm::vec3(width, pipeTopY, 0.0f), colorPipeTop));
    vertices.push_back(VertexFormat(glm::vec3(0.0f, pipeTopY, 0.0f), colorPipeTop));

    indices.insert(indices.end(), {
        pipeStart + 0, pipeStart + 1, pipeStart + 2,
        pipeStart + 0, pipeStart + 2, pipeStart + 3
    });

    unsigned int semiStart = vertices.size();
    int triangles = 30;
    glm::vec3 center = glm::vec3(width / 2.0f, baseHeight, 0.001f);

    vertices.push_back(VertexFormat(center, colorBase));

    for (int i = 0; i <= triangles; ++i) {
        float theta = M_PI * i / (float)triangles;
        float x = cos(theta) * radius + center.x;
        float y = sin(theta) * radius + center.y;
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0.001f), colorBase));
    }

    for (int i = 0; i < triangles; ++i) {
        indices.push_back(semiStart + 0);
        indices.push_back(semiStart + 1 + i);
        indices.push_back(semiStart + 1 + i + 1);
    }

    // --- Build mesh ---
    Mesh* tun = new Mesh(name);
    tun->InitFromData(vertices, indices);
    return tun;
}

Mesh* mesh2D::CreateStartButton(const std::string& name, float squareSide, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));                       
    vertices.push_back(VertexFormat(glm::vec3(squareSide, 0, 0), color));              
    vertices.push_back(VertexFormat(glm::vec3(0, squareSide, 0), color)); 
    vertices.push_back(VertexFormat(glm::vec3(squareSide, squareSide, 0), color));     

    
    indices = {
        0, 1, 2,    
        0, 3, 2     
    };

    Mesh* startButton = new Mesh(name);
    startButton->InitFromData(vertices, indices);
    return startButton;
}

Mesh* mesh2D::CreateChicken(const std::string& name, float scale) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 colorBody = glm::vec3(0.0f, 0.5f, 1.0f);   // blue
    glm::vec3 colorHead = glm::vec3(0.7f, 0.7f, 0.7f);   // light gray
    glm::vec3 colorWing = glm::vec3(0.6f, 0.6f, 0.6f);   // gray
    glm::vec3 colorBeak = glm::vec3(1.0f, 0.6f, 0.0f);   // orange
    glm::vec3 colorComb = glm::vec3(1.0f, 0.0f, 0.0f);   // red
    glm::vec3 colorFeet = glm::vec3(1.0f, 0.6f, 0.0f);   // orange
    glm::vec3 colorEyeWhite = glm::vec3(1.0f, 1.0f, 1.0f); // white
    glm::vec3 colorEyeBlack = glm::vec3(0.0f, 0.0f, 0.0f); // black

    float size = 50.0f * scale;

    float bodySide = size;
    unsigned int base = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(-bodySide / 2, 0, 0), colorBody));
    vertices.push_back(VertexFormat(glm::vec3(bodySide / 2, 0, 0), colorBody));
    vertices.push_back(VertexFormat(glm::vec3(bodySide / 2, bodySide, 0), colorBody));
    vertices.push_back(VertexFormat(glm::vec3(-bodySide / 2, bodySide, 0), colorBody));
    indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });

    int circleSegments = 30;
    float headRadius = 0.4f * size;
    glm::vec3 headCenter(0, bodySide + headRadius * 1.1f, 0.01f);
    unsigned int headStart = vertices.size();
    vertices.push_back(VertexFormat(headCenter, colorHead));

    for (int i = 0; i <= circleSegments; i++) {
        float angle = i * 2 * M_PI / circleSegments;
        float x = headCenter.x + cos(angle) * headRadius;
        float y = headCenter.y + sin(angle) * headRadius;
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0.01f), colorHead));
    }

    for (int i = 0; i < circleSegments; i++) {
        indices.push_back(headStart);
        indices.push_back(headStart + i + 1);
        indices.push_back(headStart + i + 2);
    }

    unsigned int beakStart = vertices.size();
    float beakLength = 0.6f * headRadius;
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x, headCenter.y, 0.02f), colorBeak));
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x + beakLength, headCenter.y - 0.1f * headRadius, 0.02f), colorBeak));
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x, headCenter.y - 0.2f * headRadius, 0.02f), colorBeak));
    indices.insert(indices.end(), { beakStart, beakStart + 1, beakStart + 2 });

    unsigned int combStart = vertices.size();
    float combWidth = headRadius / 1.5f;
    float combHeight = 0.6f * headRadius;
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x - combWidth, headCenter.y + headRadius * 0.8f, 0.02f), colorComb));
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x, headCenter.y + headRadius * 1.3f, 0.02f), colorComb));
    vertices.push_back(VertexFormat(glm::vec3(headCenter.x + combWidth, headCenter.y + headRadius * 0.8f, 0.02f), colorComb));
    indices.insert(indices.end(), { combStart, combStart + 1, combStart + 2 });

    float wingWidth = 1.2f * bodySide;
    float wingHeight = 0.8f * bodySide;
    unsigned int leftWing = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(-bodySide / 2, bodySide * 0.8f, 0.01f), colorWing));
    vertices.push_back(VertexFormat(glm::vec3(-bodySide / 2 - wingWidth, bodySide * 0.3f, 0.01f), colorWing));
    vertices.push_back(VertexFormat(glm::vec3(-bodySide / 2, bodySide * 0.3f, 0.01f), colorWing));
    indices.insert(indices.end(), { leftWing, leftWing + 1, leftWing + 2 });

    unsigned int rightWing = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(bodySide / 2, bodySide * 0.8f, 0.01f), colorWing));
    vertices.push_back(VertexFormat(glm::vec3(bodySide / 2 + wingWidth, bodySide * 0.3f, 0.01f), colorWing));
    vertices.push_back(VertexFormat(glm::vec3(bodySide / 2, bodySide * 0.3f, 0.01f), colorWing));
    indices.insert(indices.end(), { rightWing, rightWing + 1, rightWing + 2 });

    unsigned int feetStart = vertices.size();
    float footY = -0.3f * size;
    float footWidth = 0.4f * size;
    vertices.push_back(VertexFormat(glm::vec3(-0.25f * size, 0, 0), colorFeet));
    vertices.push_back(VertexFormat(glm::vec3(-0.25f * size - footWidth / 2, footY, 0), colorFeet));
    vertices.push_back(VertexFormat(glm::vec3(-0.25f * size + footWidth / 2, footY, 0), colorFeet));
    indices.insert(indices.end(), { feetStart, feetStart + 1, feetStart + 2 });

    unsigned int rightFoot = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(0.25f * size, 0, 0), colorFeet));
    vertices.push_back(VertexFormat(glm::vec3(0.25f * size - footWidth / 2, footY, 0), colorFeet));
    vertices.push_back(VertexFormat(glm::vec3(0.25f * size + footWidth / 2, footY, 0), colorFeet));
    indices.insert(indices.end(), { rightFoot, rightFoot + 1, rightFoot + 2 });

    int eyeSegments = 20;
    float eyeRadius = 0.12f * headRadius;
    float pupilRadius = 0.05f * headRadius;
    float eyeOffsetX = 0.18f * headRadius;
    float eyeOffsetY = 0.15f * headRadius;

    auto addCircle = [&](glm::vec3 center, float radius, glm::vec3 color) {
        unsigned int start = vertices.size();
        vertices.push_back(VertexFormat(center, color));
        for (int i = 0; i <= eyeSegments; i++) {
            float angle = i * 2 * M_PI / eyeSegments;
            float x = center.x + cos(angle) * radius;
            float y = center.y + sin(angle) * radius;
            vertices.push_back(VertexFormat(glm::vec3(x, y, center.z), color));
        }
        for (int i = 0; i < eyeSegments; i++) {
            indices.push_back(start);
            indices.push_back(start + i + 1);
            indices.push_back(start + i + 2);
        }
    };

    glm::vec3 leftEyeCenter = headCenter + glm::vec3(-eyeOffsetX, eyeOffsetY, 0.03f);
    addCircle(leftEyeCenter, eyeRadius, colorEyeWhite);
    addCircle(leftEyeCenter, pupilRadius, colorEyeBlack);

    glm::vec3 rightEyeCenter = headCenter + glm::vec3(eyeOffsetX, eyeOffsetY, 0.03f);
    addCircle(rightEyeCenter, eyeRadius, colorEyeWhite);
    addCircle(rightEyeCenter, pupilRadius, colorEyeBlack);

    Mesh* chicken = new Mesh(name);
    chicken->InitFromData(vertices, indices);
    return chicken;
}

Mesh* mesh2D::CreateBullet(const std::string& name, float diameter, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float radius = diameter / 2.0f;

    int segments = 40;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));

    for (int i = 0; i <= segments; i++) {
        float angle = i * 2 * M_PI / segments;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
    }

    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* bullet = new Mesh(name);
    bullet->InitFromData(vertices, indices);
    return bullet;
}

Mesh* mesh2D::CreateEgg(const std::string& name, float width, float height) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 colorEgg = glm::vec3(0.96f, 0.91f, 0.76f); 

    float a = width / 2.0f;   // axa orizontală
    float b = height / 2.0f;  // axa verticală

    int segments = 50;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), colorEgg));

    for (int i = 0; i <= segments; i++) {
        float angle = i * 2 * M_PI / segments;
        float x = cos(angle) * a;
        float y = sin(angle) * b;
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), colorEgg));
    }

    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* egg = new Mesh(name);
    egg->InitFromData(vertices, indices);
    return egg;
}

Mesh* mesh2D::CreateHeartsContainer(const std::string& name) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 colorGray = glm::vec3(0.5f, 0.5f, 0.5f);

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), colorGray));
    vertices.push_back(VertexFormat(glm::vec3(130, 0, 0), colorGray));
    vertices.push_back(VertexFormat(glm::vec3(130, 35, 0), colorGray));
    vertices.push_back(VertexFormat(glm::vec3(0, 35, 0), colorGray));

    indices = {
        0, 1, 2,
        0, 2, 3
    };

    Mesh* container = new Mesh(name);
    container->InitFromData(vertices, indices);
    return container;
}

Mesh* mesh2D::CreateHeart(const std::string& name, float size, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    
    float w = size;     
    float h = size;   

    vertices = {
        VertexFormat(glm::vec3(0.5f * w, 0.0f, 0), color),     // bottom point
        VertexFormat(glm::vec3(0.0f, 0.5f * h, 0), color),     // left mid
        VertexFormat(glm::vec3(0.25f * w, 0.9f * h, 0), color),// left top
        VertexFormat(glm::vec3(0.5f * w, 0.75f * h, 0), color),// center top
        VertexFormat(glm::vec3(0.75f * w, 0.9f * h, 0), color),// right top
        VertexFormat(glm::vec3(w, 0.5f * h, 0), color)         // right mid
    };

    indices = { 
        0, 1, 2, 
        0, 2, 3,
        0, 3, 4,
        0, 4, 5
    };

    Mesh* heart = new Mesh(name);

    if (!fill) {
        heart->SetDrawMode(GL_LINE_LOOP);
    }

    heart->InitFromData(vertices, indices);
    return heart;
}

