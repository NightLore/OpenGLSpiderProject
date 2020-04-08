#include "Entity.h"



Entity::Entity()
{
}


Entity::~Entity()
{
}

void Entity::reset(int type)
{
	location = vec3(0);
	direction = 0;
	this->type = type;
	size = 1;
	material = 4;
	timeFrame = 0;
}

void Entity::spawn(int new_type, vec3 max, vec3 min, float max_size, float min_size)
{
	vec3 boundingSize = max - min;
	location.x = rand() % (int)boundingSize.x + min.x;
	location.y = 0; // rand() % (int)boundingSize.y + min.y;
	location.z = rand() % (int)boundingSize.z + min.z;
	type = new_type;
	direction = rand() % 4 * M_PI_2; // entities[i].type == TYPE_CRATE ? 0 : rand() % 360 / 180 * M_PI;
	size = rand() % (int)(max_size - min_size) + min_size;
	timeFrame = rand() / (float)RAND_MAX;
	maxBounds *= size;
	minBounds *= size;
}

void Entity::move(vec3 dir, Entity *entities, int num_entities)
{
	location.x += dir.x;
	if (hasCollided(entities, num_entities))
		location.x -= dir.x;
	location.y += dir.y;
	if (hasCollided(entities, num_entities))
		location.y -= dir.y;
	location.z += dir.z;
	if (hasCollided(entities, num_entities))
		location.z -= dir.z;
}

bool Entity::hasCollided(Entity b)
{
	vec3 points[8];
	extractOuterPoints(points);
	for (int i = 0; i < 8; ++i)
	{
		if (isInsideBounds(points[i] + location, b.maxBounds + b.location, b.minBounds + b.location)) {
			return true;
		}
	}

	return false;
}

bool Entity::hasCollided(Entity *entities, int num_entities)
{
	for (int i = 0; i < num_entities; ++i)
	{
		if (hasCollided(entities[i])) {
			return true;
		}
	}
	return false;
}

void Entity::extractOuterPoints(vec3 points[])
{
	vec3 bounds[2] = { minBounds, maxBounds };
	//
	//points[0].x = bounds[0].x; points[0].y = bounds[0].y; points[0].z = bounds[0].z;
	//points[1].x = bounds[0].x; points[1].y = bounds[0].y; points[1].z = bounds[1].z;
	//points[2].x = bounds[0].x; points[2].y = bounds[1].y; points[2].z = bounds[0].z;
	//points[3].x = bounds[0].x; points[3].y = bounds[1].y; points[3].z = bounds[1].z;
	//points[4].x = bounds[1].x; points[4].y = bounds[0].y; points[4].z = bounds[0].z;
	//points[5].x = bounds[1].x; points[5].y = bounds[0].y; points[5].z = bounds[1].z;
	//points[6].x = bounds[1].x; points[6].y = bounds[1].y; points[6].z = bounds[0].z;
	//points[7].x = bounds[1].x; points[7].y = bounds[1].y; points[7].z = bounds[1].z;

	// The formula for the above:
	for (int i = 0; i < 8; ++i)
	{
		points[i].x = bounds[i / 4].x;
		points[i].y = bounds[i / 2 % 2].y;
		points[i].z = bounds[i % 2].z;
	}
}

bool Entity::isInsideBounds(vec3 pt, vec3 max, vec3 min)
{
	return min.x < pt.x && pt.x < max.x
		&& min.y < pt.y && pt.y < max.y
		&& min.z < pt.z && pt.z < max.z;
}
