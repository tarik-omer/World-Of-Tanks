#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#include <string>
#include <vector>

using namespace std;

namespace GameObjects3D
{
	class RigidBody
	{
	public:
		RigidBody();
		
		float mass;

		float speed;
		float max_speed;
		float acceleration;
		float rotation_speed;
		float rotation;
		float x, y, z;

		float speed_vertical;

		glm::mat4 model_matrix;

		string mesh_name;

		float hitbox_radius;

		void Update(float deltaTimeSeconds);
	};

	class Obstacle : public RigidBody
	{
	public:
		Obstacle(float x, float y, float z, float length, float width, float height);

		float length, width, height;
		float x, y, z;
	};

	class Projectile : public RigidBody
	{
	public:
		Projectile();

		float life_time;
		float max_life_time;
		float damage;
		bool is_enemy = false;

		void Update(float deltaTimeSeconds);
	};

	enum StateType
	{
		STATE_NONE = -1,
		STATE_PATROL = 1,
		STATE_ATTACK = 2,
		STATE_DEAD = 3,
		STATE_FOLLOW = 4
	};

	class Tank : public RigidBody
	{
		//    .--._____,
		// .-='=='==-._
		// (O_o_o_o_o_O)

		public:
			Tank();

			StateType state;
			
			// this modifies the difficulty of the game :)
			float fire_rate = 3;
			
			float time_since_last_shot = 0;
			float internalTimer = 0.0f;
			int current_direction_rotation = 0;

			float health;
			float max_health;

			float turret_rotation;
			float cannon_rotation;

			float cannon_upwards_rotation;

			float turret_rotation_speed;
			float cannon_rotation_speed;


			glm::mat4 turret_model_matrix;
			glm::mat4 cannon_model_matrix;

			int behaviour;

			vector<glm::vec3> path;

			void Update(float deltaTimeSeconds);
			void ShootCannon(vector<Projectile>& projectiles, float deltaTimeSeconds, Tank& tank, float projectile_speed, bool is_enemy);
			void AddProjectile(glm::vec3 position, float rotation, float vertical_angle, float speed, vector<GameObjects3D::Projectile>& projectiles, bool is_enemy);
			void RotateBody(float deltaTimeSeconds, float direction);
			void RotateTurret(float deltaTimeSeconds, float direction);
			void MoveForward(float deltaTimeSeconds);
			void EnemyBehaviour(float deltaTimeSeconds, Tank& player, vector<Projectile>& projectiles, vector<Obstacle> obstacles);
	};

	class Player : public Tank
	{
		public:
			Player();

			float score;
			float projectile_speed_launch = 50;
			float fire_rate = 2.0f;

			void Update(float deltaTimeSeconds);
	};



}   // namespace game_objects
