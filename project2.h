#pragma once

#include <vector>

#include "lab_m1/project2/camera_movement.h"
#include "components/simple_scene.h"
#include "lab_m1/project2/game_objects.h"
#include "lab_m1/project2/game_design.h"

using namespace std;

namespace m1
{
    class GameScene
    {
    public:
        vector<GameObjects3D::Obstacle> obstacles;
        vector<GameObjects3D::Projectile> projectiles;
        vector<GameObjects3D::Projectile> enemy_projectiles;
        vector<GameObjects3D::Tank> tanks;

        void AddTank(glm::vec3 position, glm::vec3 rotation);
        void AddObstacle(glm::vec3 position, glm::vec3 rotation);


        GameObjects3D::Player player;
    };

    class Project2 : public gfxc::SimpleScene
    {
     public:
         GameScene game_scene;

         Project2();
        ~Project2();

        string models_path = "lab_m1\\project2\\models\\";


        void Init() override;

        Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

     private:
         void Project2::UpdateScene(float deltaTimeSeconds);
         void Project2::DrawScene(float deltaTimeSeconds, bool minimap);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, bool isGround, bool isTank, bool isPlayer, int health, CameraMovement::Camera* current_camera);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        float clock;
        bool game_over = false;

        std::unordered_map<std::string, Texture2D*> mapTextures;
    
    protected:
        CameraMovement::Camera* camera;
        CameraMovement::Camera* minimap_camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float near = 0.1f;
        float far = 200.0f;
        float fovy = RADIANS(60);
        float aspect = window->props.aspectRatio;
        bool isPerspective = true;
        float lr = aspect;
        float tb = 1.0f;

        glm::vec3 lightPosition = glm::vec3(0, 20, 0);
        unsigned int materialShininess = 50;
        float materialKd = 0.9f;
        float materialKs = 0.9f;
    };
}   // namespace m1
