#pragma once
#include "Entity.h"

class Block final : public Entity
{
public:
	explicit Block(b2World& world, b2Vec2 position);
};