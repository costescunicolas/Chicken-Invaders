#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace mesh2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateBlock(const std::string &name, float length);
	Mesh* CreateMotor(const std::string &name, float squareSide, float triangleHeight);
	Mesh* CreateTun(const std::string &name, float squareSide);
    Mesh* CreateStartButton(const std::string &name, float squareSide, glm::vec3 color);
    Mesh* CreateChicken(const std::string& name, float scale);
    Mesh* CreateBullet(const std::string& name, float diameter, glm::vec3 color);
    Mesh* CreateEgg(const std::string& name, float width, float height);
    Mesh* CreateHeartsContainer(const std::string& name);
    Mesh* CreateHeart(const std::string& name, float size, glm::vec3 color, bool fill);
}
