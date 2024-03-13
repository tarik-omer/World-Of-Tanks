#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/project2/game_objects.h"

#include <iostream>

using namespace std;
using namespace GameObjects3D;

RigidBody::RigidBody()
{
	mass = 1;
	hitbox_radius = 1;
	speed = 0;
	speed_vertical = 0;
	acceleration = 0;
	max_speed = 10;
	rotation_speed = 0;
	rotation = 0;
	x = 0;
	y = 0;
	z = 0;

	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(x, y, z));

	mesh_name = "sphere";
}

void RigidBody::Update(float deltaTimeSeconds)
{
	// update position
	speed += acceleration * deltaTimeSeconds;
	speed_vertical -= 9.8 * mass * deltaTimeSeconds;

	speed = glm::clamp(speed, -max_speed, max_speed);
	speed_vertical = glm::clamp(speed_vertical, -max_speed, max_speed);
	
	x -= speed * deltaTimeSeconds * sin(rotation);
	z -= speed * deltaTimeSeconds * cos(rotation);
	if (y + speed_vertical * deltaTimeSeconds < 0)
		y = 0;
	else 
		y += speed_vertical * deltaTimeSeconds;


	glm::vec3 position = glm::vec3(x, y, z);

	// update rotation
	rotation += rotation_speed * deltaTimeSeconds;

	// limit rotation to 0 - 2PI
	if (rotation > M_PI)
		rotation = -M_PI;
	else if (rotation < -M_PI)
		rotation = M_PI;

	// update model matrix
	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, position);
	model_matrix = glm::rotate(model_matrix, rotation, glm::vec3(0, 1, 0));
}

Tank::Tank()
{
	RigidBody();

	health = 3;
	max_health = 3;

	turret_rotation = 0;
	cannon_rotation = 0;

	turret_rotation_speed = 0;
	cannon_rotation_speed = 0;

	turret_model_matrix = glm::mat4(1);
	cannon_model_matrix = glm::mat4(1);

	hitbox_radius = 5.2;

	fire_rate = 2;
	time_since_last_shot = 0;
}

void Tank::Update(float deltaTimeSeconds)
{
	RigidBody::Update(deltaTimeSeconds);

	glm::vec3 position = glm::vec3(x, y, z);

	// update internal timer
	internalTimer += deltaTimeSeconds;

	// limit internal timer to 5 seconds
	if (internalTimer > 5)
		internalTimer = 0;

	// update time since last shot
	time_since_last_shot += deltaTimeSeconds;

	// limit time since last shot to 2 seconds
	if (time_since_last_shot > fire_rate)
		time_since_last_shot = fire_rate;

	// update turret rotation
	turret_rotation += turret_rotation_speed * deltaTimeSeconds;

	// limit turret rotation to 0 - 2PI
	if (turret_rotation > M_PI)
		turret_rotation = -M_PI;
	else if (turret_rotation < -M_PI)
		turret_rotation = M_PI;

	// update cannon rotation
	cannon_rotation += cannon_rotation_speed * deltaTimeSeconds;

	// update turret model matrix
	turret_model_matrix = glm::mat4(1);
	turret_model_matrix = glm::translate(turret_model_matrix, position);
	turret_model_matrix = glm::rotate(turret_model_matrix, turret_rotation, glm::vec3(0, 1, 0));

	// update cannon model matrix - rotate around OY and up and down based on cannon_rotation_upwards
	cannon_model_matrix = glm::mat4(1);
	cannon_model_matrix = glm::translate(cannon_model_matrix, position);
	cannon_model_matrix = glm::rotate(cannon_model_matrix, turret_rotation, glm::vec3(0, 1, 0));
	cannon_model_matrix = glm::rotate(cannon_model_matrix, cannon_rotation, glm::vec3(1, 0, 0));

	// rotate up
	cannon_model_matrix = glm::rotate(cannon_model_matrix, cannon_upwards_rotation, glm::vec3(1, 0, 0));
}

void Tank::ShootCannon(vector<Projectile>& projectiles, float deltaTimeSeconds, Tank& tank, float projectile_speed, bool is_enemy) {
	if (health <= 0)
		return;

	if (time_since_last_shot < fire_rate)
		return;

	time_since_last_shot = 0;

	// Calculate projectile position - tip of the cannon
	glm::mat4 projectile_model_matrix = glm::mat4(1);

	// Translate to tank position
	projectile_model_matrix = glm::translate(projectile_model_matrix, glm::vec3(tank.x, tank.y, tank.z));
	projectile_model_matrix = glm::translate(projectile_model_matrix, glm::vec3(0, -0.1f, 0));
	
	// Rotate to turret rotation
	projectile_model_matrix = glm::rotate(projectile_model_matrix, tank.turret_rotation, glm::vec3(0, 1, 0));
	
	// Translate to tip of the cannon
	float cannon_length = 7.0f;
	float up = sin(tank.cannon_upwards_rotation) * cannon_length;
	float forward = cos(tank.cannon_upwards_rotation) * cannon_length;
	projectile_model_matrix = glm::translate(projectile_model_matrix, glm::vec3(0, up, -forward));
	projectile_model_matrix = glm::translate(projectile_model_matrix, glm::vec3(0, 1.8, 0));

    // Get projectile position
    glm::vec3 projectile_position = glm::vec3(projectile_model_matrix[3][0], projectile_model_matrix[3][1], projectile_model_matrix[3][2]);

	// Calculate projectile angle
    float projectile_angle = tank.turret_rotation;
    float projectile_vertical_angle = tank.cannon_upwards_rotation;

	// Calculate projectile speed
	float speed = projectile_speed + tank.speed;

	// Add projectile to game objects
	tank.AddProjectile(projectile_position, projectile_angle, projectile_vertical_angle, speed, projectiles, is_enemy);
}

void Tank::RotateBody(float deltaTimeSeconds, float direction)
{
	rotation_speed = direction * 2;
}

void Tank::RotateTurret(float deltaTimeSeconds, float direction)
{
	turret_rotation_speed = direction * 2;
}

void Tank::MoveForward(float deltaTimeSeconds)
{
	acceleration = 5;
}

void Tank::EnemyBehaviour(float deltaTimeSeconds, Tank& player, vector<Projectile>& projectiles, vector<Obstacle> obstacles)
{
	// Check for dead state
	if (health <= 0)
		return;

	// Calculate distance to player
	float distance_to_player = glm::distance(glm::vec3(x, y, z), glm::vec3(player.x, player.y, player.z));

	// Check if player is in sight
	bool player_in_sight = false;
	if (distance_to_player < 40)
		player_in_sight = true;

	bool player_out_of_range_and_in_sight = false;
	if (distance_to_player > 40 && distance_to_player < 100)
		player_out_of_range_and_in_sight = true;

	// Update state
	if (player_in_sight)
		state = STATE_ATTACK;
	else if (player_out_of_range_and_in_sight)
		state = STATE_FOLLOW;
	else
		state = STATE_PATROL;

	// if player is dead, just patrol
	if (player.health <= 0)
		state = STATE_PATROL;

	// Update behaviour
	if (state == STATE_ATTACK)
	{
		// Rotate turret towards player
		// Calculate turret rotation difference between current rotation and rotation towards player
		float turret_rotation_difference = M_PI - turret_rotation - atan2(player.z - z, player.x - x);
		
		// offset by 90 degrees
		turret_rotation_difference -= M_PI / 2;
		
		if (turret_rotation_difference > M_PI)
			turret_rotation_difference -= 2 * M_PI;
		else if (turret_rotation_difference < -M_PI)
			turret_rotation_difference += 2 * M_PI;

		if (turret_rotation_difference < 0.2f)
			RotateTurret(deltaTimeSeconds, 1);
		else if (turret_rotation_difference > -0.2f)
			RotateTurret(deltaTimeSeconds, -1);
		else
			RotateTurret(deltaTimeSeconds, 0);

		// No need to rotate body
		RotateBody(deltaTimeSeconds, 0);

		// Move forward
		MoveForward(deltaTimeSeconds);

		// Shoot cannon
		ShootCannon(projectiles, deltaTimeSeconds, *this, 50, true);

	} else if (state == STATE_PATROL) {
		// No need to rotate turret
		RotateTurret(deltaTimeSeconds, 0);

		// Go forward
		MoveForward(deltaTimeSeconds);

		// Every 5 seconds, rotate body randomly
		if (internalTimer < 0.01f) {
			current_direction_rotation = rand() % 3 - 1;
		}

		if (internalTimer < 1.0f)
			RotateBody(deltaTimeSeconds, current_direction_rotation);
		else
			RotateBody(deltaTimeSeconds, 0);

	}
	else if (state == STATE_FOLLOW) {
		// rotate only the body towards the player
		// Calculate body rotation difference between current rotation and rotation towards player
		float body_rotation_difference = M_PI - rotation - atan2(player.z - z, player.x - x);

		// offset by 90 degrees
		body_rotation_difference -= M_PI / 2;

		if (body_rotation_difference > M_PI)
			body_rotation_difference -= 2 * M_PI;
		else if (body_rotation_difference < -M_PI)
			body_rotation_difference += 2 * M_PI;

		if (body_rotation_difference < 0.1f)
			RotateBody(deltaTimeSeconds, 1);
		else if (body_rotation_difference > -0.1f)
			RotateBody(deltaTimeSeconds, -1);
		else
			RotateBody(deltaTimeSeconds, 0);

		// No need to rotate turret
		RotateTurret(deltaTimeSeconds, 0);

		// Go forward
		MoveForward(deltaTimeSeconds);
	}


}

Projectile::Projectile()
{
	life_time = 0;
	max_life_time = 10;
	damage = 1;
	speed = 10;
	speed_vertical = 0;
	mass = 1;
}

void Projectile::Update(float deltaTimeSeconds)
{
	RigidBody::Update(deltaTimeSeconds);

	// update life time
	life_time += deltaTimeSeconds;
}

Player::Player()
{
	Tank();

	acceleration = 10;
	max_speed = 20;

	health = 3;
	max_health = 3;
}

void Player::Update(float deltaTimeSeconds)
{
	Tank::Update(deltaTimeSeconds);
}

void Tank::AddProjectile(glm::vec3 position, float rotation, float vertical_angle, float speed, vector<GameObjects3D::Projectile>& projectiles, bool is_enemy) {
	// Create projectile
	GameObjects3D::Projectile projectile = GameObjects3D::Projectile();

	// Set projectile position
	projectile.x = position.x;
	projectile.y = position.y;
	projectile.z = position.z;

	// Set projectile rotation
	projectile.rotation = rotation;

	// Set projectile vertical speed based on vertical angle
	projectile.speed_vertical = speed * sin(vertical_angle);

	// Set projectile speed and max speed
	projectile.speed = speed;
	projectile.max_speed = speed;

	// Set projectile acceleration
	projectile.acceleration = 0;

	// Set projectile rotation speed
	projectile.rotation_speed = 0;

	// Set projectile hitbox radius
	projectile.hitbox_radius = 0.5f;

	// Set is_enemy flag
	if (is_enemy)
		projectile.is_enemy = true;

	// Add projectile to projectiles
	projectiles.push_back(projectile);
}

Obstacle::Obstacle(float x, float y, float z, float length, float width, float height)
{
	RigidBody();

	this->x = x;
	this->y = y;
	this->z = z;

	this->length = length;
	this->width = width;
	this->height = height;

	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(x, y, z));
	model_matrix = glm::scale(model_matrix, glm::vec3(length, height, width));
}