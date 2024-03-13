#include "lab_m1/project2/project2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/project2/game_design.h"

#include <Core/Engine.h>
#include <Core/GPU/Mesh.h>

using namespace std;
using namespace m1;
using namespace GameObjects3D;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Project2::Project2()
{
}


Project2::~Project2()
{
}


void Project2::Init()
{
    // Load textures
	{
	    Texture2D* texture = new Texture2D();
        texture->Load2D((PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "project2", "textures", "ground_texture.jpg").c_str()), GL_REPEAT);
        mapTextures["ground"] = texture;
    }

    // Create a 1x1 cube for Obstacle
    {
        // set grey color for cube
        glm::vec3 color = glm::vec3(0.3f, 0.3f, 0.3f);

        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(-0.5f, 0.5f, -0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(0.5f, 0.5f, -0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(-0.5f, -0.5f, 0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(0.5f, -0.5f, 0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(-0.5f, 0.5f, 0.5f), color, glm::vec3(0), glm::vec2(0)),
            VertexFormat(glm::vec3(0.5f, 0.5f, 0.5f), color, glm::vec3(0), glm::vec2(0))
        };

        vector<unsigned int> indices = {
            0, 1, 2,	// indices for first triangle
            1, 3, 2,	// indices for second triangle
            1, 5, 3,
            5, 7, 3,
            5, 4, 7,
            4, 6, 7,
            4, 0, 6,
            0, 2, 6,
            2, 3, 6,
            4, 5, 0,
            5, 1, 0,
            3, 7, 6,
            7, 3, 2
        };

        meshes["cube_1"] = CreateMesh("cube_1", vertices, indices);
    }

    renderCameraTarget = false;
    // Set up camera
    camera = new CameraMovement::Camera();
    camera->Set(glm::vec3(0, 5, 8.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    
    string path = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "project2", "models");

    // Set up separate camera for minimap
    minimap_camera = new CameraMovement::Camera();
    minimap_camera->Set(glm::vec3(0, 100, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

    // get vertices and incides for tank body from file
    GameDesign::MeshInfo mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "tank_body_1.obj"));
    // set vertex color to dark green
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
		mesh_info.vertices[i].color = glm::vec3(0.0f, 0.1f, 0.0f);
	}
    meshes["tank_body"] = CreateMesh("tank_body", mesh_info.vertices, mesh_info.indices);

    // get vertices and incides for tank turret from file
    mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "tank_turret_1.obj"));
    // set vertex color to light green
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
        mesh_info.vertices[i].color = glm::vec3(0.0f, 0.15f, 0.0f);
    }
    meshes["tank_turret"] = CreateMesh("tank_turret", mesh_info.vertices, mesh_info.indices);

    // get vertices and incides for tank cannon from file
    mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "tank_cannon_1.obj"));
    // set vertex color to grey
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
		mesh_info.vertices[i].color = glm::vec3(0.5f, 0.5f, 0.5f);
	}
    meshes["tank_cannon"] = CreateMesh("tank_cannon", mesh_info.vertices, mesh_info.indices);

    // get vertices and incides for tank tracks from file
    mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "tank_tracks_1.obj"));
    // set vertex color to dark grey
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
        mesh_info.vertices[i].color = glm::vec3(0.2f, 0.2f, 0.2f);
    }
    meshes["tank_tracks"] = CreateMesh("tank_tracks", mesh_info.vertices, mesh_info.indices);

    // get vertices and incides for ground from file
    mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "ground_1.obj"));
    // set vertex color to brown
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
		mesh_info.vertices[i].color = glm::vec3(0.5f, 0.3f, 0.0f);
        // set texture coordinates
        mesh_info.vertices[i].text_coord = glm::vec2(mesh_info.vertices[i].position.x, mesh_info.vertices[i].position.z);
	}
    meshes["ground"] = CreateMesh("ground", mesh_info.vertices, mesh_info.indices);

    // Get vertices and indices for cannon ball
    mesh_info = GameDesign::GetMeshInfo(PATH_JOIN(path, "cannon_ball.obj"));
    // set vertex color to grey
    for (int i = 0; i < mesh_info.vertices.size(); i++) {
        mesh_info.vertices[i].color = glm::vec3(0.8f, 0.8f, 0.8f);
    }
    meshes["sphere"] = CreateMesh("sphere", mesh_info.vertices, mesh_info.indices);

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("ProjectShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "project2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "project2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    projectionMatrix = glm::perspective(fovy, aspect, near, far);

    // Create game scene
    game_scene = GameScene();

    // Create player
    game_scene.player = GameObjects3D::Player();

    // Set distance to target so that it is the same as 0, 0, 0
    camera->distanceToTarget = glm::distance(camera->position, glm::vec3(0, 0, 0));

    // Set initial player position up to test gravity and camera
    game_scene.player.x = 0;
    game_scene.player.y = 10;
    game_scene.player.z = 0;

    // add 10 enemy tanks to game scene at random positions within a 500x500 square
    for (int i = 0; i < 5; i++) {
		// Create enemy tank
	    game_scene.AddTank(glm::vec3(rand() % 500 - 250, 0, rand() % 500 - 250), glm::vec3(0));
    }

    // Add 10 obstacles to game scene at random positions within a 500x500 square
    for (int i = 0; i < 10; i++) {
        // Create obstacle
        // length is x bound
        // width is z bound
        // height is y bound
        float length = rand() % 30 + 25;
        float width = rand() % 30 + 25;
        float height = rand() % 30 + 30;

        Obstacle obstacle = Obstacle(rand() % 300 - 150, 0, rand() % 300 - 150, length, width, height);
        game_scene.obstacles.push_back(obstacle);
    }

}


Mesh* Project2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}


void Project2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Project2::UpdateScene(float deltaTimeSeconds) {
    // Update player
    game_scene.player.Update(deltaTimeSeconds);
    glm::vec3 player_pos = glm::vec3(game_scene.player.x, game_scene.player.y, game_scene.player.z);

    // Move minimap camera to player position
    minimap_camera->Set(glm::vec3(player_pos.x, 100, player_pos.z), glm::vec3(player_pos.x, 0, player_pos.z), glm::vec3(0, 0, -1));

    // Update cooldown for shooting
    game_scene.player.time_since_last_shot += deltaTimeSeconds;

    // Clamp cooldown to fire_rate
    if (game_scene.player.time_since_last_shot > game_scene.player.fire_rate) {
		game_scene.player.time_since_last_shot = game_scene.player.fire_rate;
	}

    // Update camera
    camera->FollowPlayer(player_pos);

    // Add collision detection for player - enemy tank using hitbox radius
    for (int i = 0; i < game_scene.tanks.size(); i++) {
		glm::vec3 tank_pos = glm::vec3(game_scene.tanks[i].x, game_scene.tanks[i].y, game_scene.tanks[i].z);
		float distance = glm::distance(player_pos, tank_pos);

        if (distance < game_scene.player.hitbox_radius + game_scene.tanks[i].hitbox_radius) {
	        // Move player - enemy tank away from each other
            glm::vec3 direction = glm::normalize(player_pos - tank_pos);
            game_scene.player.x += direction.x * (game_scene.player.hitbox_radius + game_scene.tanks[i].hitbox_radius - distance);
            game_scene.player.z += direction.z * (game_scene.player.hitbox_radius + game_scene.tanks[i].hitbox_radius - distance);

        }
	}

    // Add collision detection for projectile - enemy tank using hitbox radius
    for (int i = 0; i < game_scene.projectiles.size(); i++) {
        Projectile projectile = game_scene.projectiles[i];
        // Check if projectile is player or enemy
        if (projectile.is_enemy) {
            // Check collision with player
            float distance = glm::distance(glm::vec3(projectile.x, projectile.y, projectile.z), player_pos);

            if (distance < projectile.hitbox_radius + game_scene.player.hitbox_radius) {
                // Remove projectile from game objects
                game_scene.projectiles.erase(game_scene.projectiles.begin() + i);

                // Decrease player health
                game_scene.player.health -= 1;

                cout << "Player hit: " << game_scene.player.health << endl;

                // Check if player is dead
                if (game_scene.player.health <= 0) {
				    // End game
                    cout << "Game over!" << endl;
                    game_over = true;
                
                    // Set Player speed to 0
                    game_scene.player.speed = 0;
                    game_scene.player.acceleration = 0;
                    game_scene.player.rotation_speed = 0;
                    game_scene.player.turret_rotation_speed = 0;
                }
            }
        } else {
            // Check collision with enemy tanks
            for (int j = 0; j < game_scene.tanks.size(); j++) {
				glm::vec3 tank_pos = glm::vec3(game_scene.tanks[j].x, game_scene.tanks[j].y, game_scene.tanks[j].z);
				float distance = glm::distance(glm::vec3(projectile.x, projectile.y, projectile.z), tank_pos);

                if (distance < projectile.hitbox_radius + game_scene.tanks[j].hitbox_radius) {
					// Remove projectile from game objects
					game_scene.projectiles.erase(game_scene.projectiles.begin() + i);
					// Decrease enemy tank health
                    game_scene.tanks[j].health -= 1;

                    // debug print hit
                    cout << "Enemy tank hit: " << game_scene.tanks[j].health << endl;

					// Check if enemy tank is dead
                    if (game_scene.tanks[j].health <= 0) {
                        // Stop tank movement
                        game_scene.tanks[j].speed = 0;
                        game_scene.tanks[j].acceleration = 0;
                        game_scene.tanks[j].rotation_speed = 0;
                        game_scene.tanks[j].turret_rotation_speed = 0;

                        cout << "Enemy tank " << j << " destroyed!" << endl;
					}
				}
			}
        }

        // Check for collision for enemy tank - enemy tank
        for (int j = 0; j < game_scene.tanks.size(); j++) {
            for (int k = 0; k < game_scene.tanks.size(); k++) {
                if (j != k) {
					glm::vec3 tank_pos_1 = glm::vec3(game_scene.tanks[j].x, game_scene.tanks[j].y, game_scene.tanks[j].z);
					glm::vec3 tank_pos_2 = glm::vec3(game_scene.tanks[k].x, game_scene.tanks[k].y, game_scene.tanks[k].z);
					float distance = glm::distance(tank_pos_1, tank_pos_2);

                    if (distance < game_scene.tanks[j].hitbox_radius + game_scene.tanks[k].hitbox_radius) {
						// Move enemy tanks away from each other
						glm::vec3 direction = glm::normalize(tank_pos_1 - tank_pos_2);
						game_scene.tanks[j].x += direction.x * (game_scene.tanks[j].hitbox_radius + game_scene.tanks[k].hitbox_radius - distance);
						game_scene.tanks[j].z += direction.z * (game_scene.tanks[j].hitbox_radius + game_scene.tanks[k].hitbox_radius - distance);
					}
				}
			}
        }
    
    }

    // Collision detection for player - obstacle
    for (int i = 0; i < game_scene.obstacles.size(); i++) {
        Obstacle obstacle = game_scene.obstacles[i];

        // Check collision with player
        if (game_scene.player.x + game_scene.player.hitbox_radius > obstacle.x - obstacle.length / 2 &&
            game_scene.player.x - game_scene.player.hitbox_radius < obstacle.x + obstacle.length / 2 &&
            game_scene.player.z + game_scene.player.hitbox_radius > obstacle.z - obstacle.width / 2 &&
            game_scene.player.z - game_scene.player.hitbox_radius < obstacle.z + obstacle.width / 2) {

            // Calculate the overlap along both X and Z axes
            float overlapX = min((game_scene.player.x + game_scene.player.hitbox_radius) - (obstacle.x - obstacle.length / 2),
                (obstacle.x + obstacle.length / 2) - (game_scene.player.x - game_scene.player.hitbox_radius));
            float overlapZ = min((game_scene.player.z + game_scene.player.hitbox_radius) - (obstacle.z - obstacle.width / 2),
                (obstacle.z + obstacle.width / 2) - (game_scene.player.z - game_scene.player.hitbox_radius));

            // Resolve collision by moving the player away from the obstacle
            if (overlapX < overlapZ) {
                // Move along X axis
                game_scene.player.x += (game_scene.player.x > obstacle.x) ? overlapX : -overlapX;
            }
            else {
                // Move along Z axis
                game_scene.player.z += (game_scene.player.z > obstacle.z) ? overlapZ : -overlapZ;
            }
        }
    }

    // Collision detection for enemy tank - obstacle
    for (int i = 0; i < game_scene.obstacles.size(); i++)
    {
        Obstacle obstacle = game_scene.obstacles[i];
        // Check collision with enemy tanks
        for (int j = 0; j < game_scene.tanks.size(); j++) {
            // Check if tank is colliding with obstacle
            // Get tank position
            glm::vec3 tank_pos = glm::vec3(game_scene.tanks[j].x, game_scene.tanks[j].y, game_scene.tanks[j].z);
            float hitbox_radius = game_scene.tanks[j].hitbox_radius;

            if (tank_pos.x + hitbox_radius > obstacle.x - obstacle.length / 2 &&
                tank_pos.x - hitbox_radius < obstacle.x + obstacle.length / 2 &&
                tank_pos.z + hitbox_radius > obstacle.z - obstacle.width / 2 &&
                tank_pos.z - hitbox_radius < obstacle.z + obstacle.width / 2) {
                // Calculate the overlap along both X and Z axes
                float overlapX = min((tank_pos.x + hitbox_radius) - (obstacle.x - obstacle.length / 2),
                    					(obstacle.x + obstacle.length / 2) - (tank_pos.x - hitbox_radius));
                float overlapZ = min((tank_pos.z + hitbox_radius) - (obstacle.z - obstacle.width / 2),
                                        (obstacle.z + obstacle.width / 2) - (tank_pos.z - hitbox_radius));

                // Resolve collision by moving the tank away from the obstacle
                if (overlapX < overlapZ) {
					// Move along X axis
					game_scene.tanks[j].x += (tank_pos.x > obstacle.x) ? overlapX : -overlapX;
				} else {
					// Move along Z axis
					game_scene.tanks[j].z += (tank_pos.z > obstacle.z) ? overlapZ : -overlapZ;
				}
            }
        }
    }

    // Collision detection for projectile - obstacle
    for (int i = 0; i < game_scene.obstacles.size(); i++) {
		Obstacle obstacle = game_scene.obstacles[i];

		// Check collision with player
        for (int j = 0; j < game_scene.projectiles.size(); j++) {
			Projectile projectile = game_scene.projectiles[j];

            if (projectile.x + projectile.hitbox_radius > obstacle.x - obstacle.length / 2 &&
                projectile.x - projectile.hitbox_radius < obstacle.x + obstacle.length / 2 &&
                projectile.z + projectile.hitbox_radius > obstacle.z - obstacle.width / 2 &&
                projectile.z - projectile.hitbox_radius < obstacle.z + obstacle.width / 2) {

				// Remove projectile from game objects
				game_scene.projectiles.erase(game_scene.projectiles.begin() + j);
			}
		}
	}

    // Update projectiles
    for (int i = 0; i < game_scene.projectiles.size(); i++) {
        game_scene.projectiles[i].Update(deltaTimeSeconds);

        // Keep projectile above ground level
        if (game_scene.projectiles[i].y < 1.75) {
            game_scene.projectiles[i].y = 1.75;
        }

        // Check if projectile is out of bounds or life time is over
        if (game_scene.projectiles[i].life_time > game_scene.projectiles[i].max_life_time) {
			// Remove projectile from game objects
			game_scene.projectiles.erase(game_scene.projectiles.begin() + i);
            break;
		}
    }

    // Update enemy tanks
    for (int i = 0; i < game_scene.tanks.size(); i++) {
		game_scene.tanks[i].Update(deltaTimeSeconds);
        game_scene.tanks[i].EnemyBehaviour(deltaTimeSeconds, game_scene.player, game_scene.projectiles, game_scene.obstacles);
    }

}

void Project2::DrawScene(float deltaTimeSeconds, bool minimap) {
    // Based on minimap flag, set camera to minimap camera or normal camera
    CameraMovement::Camera* current_camera = minimap ? minimap_camera : camera;

    // Draw all projectiles
    for (int i = 0; i < game_scene.projectiles.size(); i++) {
		RenderSimpleMesh(meshes["sphere"], shaders["ProjectShader"], game_scene.projectiles[i].model_matrix, false, false, false, -1, current_camera);
	}

    // Draw all obstacles
    for (int i = 0; i < game_scene.obstacles.size(); i++) {
        RenderSimpleMesh(meshes["cube_1"], shaders["ProjectShader"], game_scene.obstacles[i].model_matrix, false, false, false, -1, current_camera);
    }

    // Draw all enemy tanks
    for (int i = 0; i < game_scene.tanks.size(); i++) {
		game_scene.tanks[i].Update(deltaTimeSeconds);
        int tank_health = game_scene.tanks[i].health;
		RenderSimpleMesh(meshes["tank_body"], shaders["ProjectShader"], game_scene.tanks[i].model_matrix, false, true, false, tank_health, current_camera);
		RenderSimpleMesh(meshes["tank_turret"], shaders["ProjectShader"], game_scene.tanks[i].turret_model_matrix, false, true, false, tank_health, current_camera);
		RenderSimpleMesh(meshes["tank_cannon"], shaders["ProjectShader"], game_scene.tanks[i].cannon_model_matrix, false, false, false, -1, current_camera);
		RenderSimpleMesh(meshes["tank_tracks"], shaders["ProjectShader"], game_scene.tanks[i].model_matrix, false, false, false, -1, current_camera);
	}

    // Draw player
    int player_health = game_scene.player.health;
    RenderSimpleMesh(meshes["tank_body"], shaders["ProjectShader"], game_scene.player.model_matrix, false, true, true, player_health, current_camera);
    RenderSimpleMesh(meshes["tank_turret"], shaders["ProjectShader"], game_scene.player.turret_model_matrix, false , true, true, player_health, current_camera);
    RenderSimpleMesh(meshes["tank_cannon"], shaders["ProjectShader"], game_scene.player.cannon_model_matrix, false , false, false, -1, current_camera);
    RenderSimpleMesh(meshes["tank_tracks"], shaders["ProjectShader"], game_scene.player.model_matrix, false, false, false, -1, current_camera);

    // Draw ground
    RenderSimpleMesh(meshes["ground"], shaders["ProjectShader"], glm::mat4(1), true, false, false, -1, current_camera);
}


void Project2::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    // Calculate projection matrix
    projectionMatrix = glm::perspective(fovy, aspect, near, far);

    // Update Scene
    UpdateScene(deltaTimeSeconds);

    // Set viewport to full screen
    glViewport(0, 0, resolution.x, resolution.y);

    // Draw scene
    DrawScene(deltaTimeSeconds, false);

    // Calculate projection matrix for minimap
    projectionMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near, far);
    
    // Set viewport to bottom left corner
    glViewport(0, 0, resolution.x / 4, resolution.y / 4);

    // Draw minimap
    DrawScene(deltaTimeSeconds, true);
}


void Project2::FrameEnd()
{
}


void Project2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, bool isGround, bool isTank, bool isPlayer, int health, CameraMovement::Camera* current_camera)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Get shader location for uniform mat4 "Model"
    int location_model = glGetUniformLocation(shader->program, "Model");

    // Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Get shader location for uniform mat4 "View"
    int location_view = glGetUniformLocation(shader->program, "View");

    // Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = current_camera->GetViewMatrix();
    glUniformMatrix4fv(location_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Get shader location for uniform mat4 "Projection"
    int location_projection = glGetUniformLocation(shader->program, "Projection");

    // Set shader uniform "Projection" to projectionMatrix
    glUniformMatrix4fv(location_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Get shader location for uniform bool "isGround"
    int location_isGround = glGetUniformLocation(shader->program, "isGround");

    // Set shader uniform "isGround" to isGround
    glUniform1i(location_isGround, isGround);

    // Get shader location for uniform texture "texture"
    int location_texture = glGetUniformLocation(shader->program, "texture");

    // Set shader uniform "texture" to texture unit 0
    glUniform1i(location_texture, 0);

    // Bind texture location to texture unit 0
    glActiveTexture(GL_TEXTURE0);

    // Get shader location for uniform bool "isTank"
    int location_isTank = glGetUniformLocation(shader->program, "isTank");

    // Set shader uniform "isTank" to isTank
    glUniform1i(location_isTank, isTank);

    // Get shader location for uniform bool "isPlayer"
    int location_isPlayer = glGetUniformLocation(shader->program, "isPlayer");

    // Set shader uniform "isPlayer" to isPlayer
    glUniform1i(location_isPlayer, isPlayer);

    // Get shader location for uniform int "health"
    int location_health = glGetUniformLocation(shader->program, "health");

    // Set shader uniform "health" to health
    glUniform1i(location_health, health);

    // Set shader uniforms for light & material properties
    // Set light position uniform
    int location_light = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(location_light, 1, glm::value_ptr(lightPosition));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = current_camera->position;
    int location_eye = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(location_eye, 1, glm::value_ptr(eyePosition));

    // Set material property uniforms (shininess, kd, ks, object color) 
    int location_material_sh = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(location_material_sh, materialShininess);

    int location_material_kd = glGetUniformLocation(shader->program, "material_kd");  // diffuse light
    glUniform1f(location_material_kd, materialKd);

    int location_material_ks = glGetUniformLocation(shader->program, "material_ks");  // specular light
    glUniform1f(location_material_ks, materialKs);

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, mapTextures["ground"]->GetTextureID());

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Project2::OnInputUpdate(float deltaTime, int mods)
{
    if (game_over)
        return;

    // Modify player projectile speed
    if (window->KeyHold(GLFW_KEY_UP)) {
        game_scene.player.projectile_speed_launch += deltaTime * 10;
        cout << game_scene.player.projectile_speed_launch << endl;
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        game_scene.player.projectile_speed_launch -= deltaTime * 10;
        cout << game_scene.player.projectile_speed_launch << endl;
        if (game_scene.player.projectile_speed_launch < 5) {
            game_scene.player.projectile_speed_launch = 5;
        }
    }

    // Player movement
    float acceleration = 10;
    float angular_speed = 1.5f;

    // Move the player fwd / backwd (W, S keys)
    if (window->KeyHold(GLFW_KEY_W)) {
        if (game_scene.player.speed < 0) {
            // Apply brake
            game_scene.player.acceleration = 5 * acceleration;
        } else {
			game_scene.player.acceleration = acceleration;
		}
    } else if (window->KeyHold(GLFW_KEY_S)) {
        if (game_scene.player.speed > 0) {
            // Apply brake
			game_scene.player.acceleration = -5 * acceleration;
		}
        else {
			game_scene.player.acceleration = -acceleration;
		}
    } else if (game_scene.player.speed < -0.1f || game_scene.player.speed > 0.1f) {
        game_scene.player.acceleration = -2.5f * acceleration * glm::sign(game_scene.player.speed);
    } else {
        game_scene.player.acceleration = 0;
        game_scene.player.speed = 0;
    }

    // Rotate the player (A, D keys)
    if (window->KeyHold(GLFW_KEY_A)) {
        game_scene.player.rotation_speed = angular_speed;
        // Rotate camera
        camera->RotateThirdPerson_OY(angular_speed * deltaTime);
        // Rotate turret
        game_scene.player.turret_rotation_speed = angular_speed;
    } else if (window->KeyHold(GLFW_KEY_D)) {
		game_scene.player.rotation_speed = -angular_speed;
        // Rotate camera
        camera->RotateThirdPerson_OY(-angular_speed * deltaTime);
        // Rotate turret
        game_scene.player.turret_rotation_speed = -angular_speed;
    } else {
		game_scene.player.rotation_speed = 0;
        game_scene.player.turret_rotation_speed = 0;
	}

    // Lift turret (Q and E keys)
    if (window->KeyHold(GLFW_KEY_Q)) {
        game_scene.player.cannon_upwards_rotation += deltaTime;
    } else if (window->KeyHold(GLFW_KEY_E)) {
        game_scene.player.cannon_upwards_rotation -= deltaTime;
	}

    // Clamp turret lift to 30 degrees up and 0 degrees down
    float max_cannon_upwards_rotation = 30 * M_PI / 180;

    if (game_scene.player.cannon_upwards_rotation > max_cannon_upwards_rotation) {
		game_scene.player.cannon_upwards_rotation = max_cannon_upwards_rotation;
	}
	else if (game_scene.player.cannon_upwards_rotation < 0) {
		game_scene.player.cannon_upwards_rotation = 0;
	}

    // Shoot (SPACE key)
    if (window->KeyHold(GLFW_KEY_SPACE)) {
        float speed = game_scene.player.projectile_speed_launch;
        game_scene.player.ShootCannon(game_scene.projectiles, deltaTime, game_scene.player, speed, false);
	}
}


void Project2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Project2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Project2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Move the turret only if MOUSE_RIGHT button is not pressed
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !game_over)
    {
		float sensivityOY = 0.005f;

        // Rotate the turret
        game_scene.player.turret_rotation -= sensivityOY * deltaX;
    }

    // Add mouse move event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;
        
        // rotate camera in 3rd person if angle is not too big
        if (camera->angleOX <= 0.65 && camera->angleOX >= -0.42) {
			camera->RotateThirdPerson_OX(-2 * sensivityOX * deltaY);
        } else if (camera->angleOX > 0.65) {
            // rotate camera back to 0.65
            camera->RotateThirdPerson_OX(-0.001f);
        } else if (camera->angleOX < -0.42) {
			// rotate camera back to -0.42
			camera->RotateThirdPerson_OX(0.001f);
		}
        camera->RotateThirdPerson_OY(-2 * sensivityOY * deltaX);
    }
}


void Project2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Project2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Project2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Project2::OnWindowResize(int width, int height)
{
}

void GameScene::AddTank(glm::vec3 position, glm::vec3 rotation) {
    // Create enemy tank
    Tank tank = Tank();

    // Set tank position
    tank.x = position.x;
    tank.y = position.y;
    tank.z = position.z;

    tank.rotation = rotation.y;
    tank.turret_rotation = rotation.y;
    tank.cannon_rotation = rotation.y;
    tank.cannon_upwards_rotation = 0;
    tank.speed = 0;
    tank.acceleration = 0;
    tank.rotation_speed = 0;
    tank.turret_rotation_speed = 0;
    tank.cannon_rotation_speed = 0;
    tank.fire_rate = 3;
    tank.time_since_last_shot = 0;
    tank.health = 3;
    tank.max_health = 3;
    tank.max_speed = 3;
    tank.behaviour = 1;
    
    // Set tank model matrix
    tank.model_matrix = glm::mat4(1);
    tank.model_matrix = glm::translate(tank.model_matrix, position);
    tank.model_matrix = glm::rotate(tank.model_matrix, rotation.y, glm::vec3(0, 1, 0));

    // Set tank turret model matrix
    tank.turret_model_matrix = glm::mat4(1);
    tank.turret_model_matrix = glm::translate(tank.turret_model_matrix, position);
    tank.turret_model_matrix = glm::rotate(tank.turret_model_matrix, rotation.y, glm::vec3(0, 1, 0));

    // Set tank cannon model matrix
    tank.cannon_model_matrix = glm::mat4(1);
    tank.cannon_model_matrix = glm::translate(tank.cannon_model_matrix, position);
    tank.cannon_model_matrix = glm::rotate(tank.cannon_model_matrix, rotation.y, glm::vec3(0, 1, 0));

    // Add tank to game objects
    tanks.push_back(tank);
}

void GameScene::AddObstacle(glm::vec3 position, glm::vec3 rotation) {

}


