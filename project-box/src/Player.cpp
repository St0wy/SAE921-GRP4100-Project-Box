#include "Player.h"

#include "VecUtils.h"
#include "SensorType.h"

Player::Player(b2World& world)
	:Entity(), isFacingRight_(true), moveDirection_(0), footContactsCounter_(0)
{
	b2BodyDef entityBodyDef;
	entityBodyDef.type = b2_dynamicBody;
	entityBodyDef.position.Set(0.0f, 10.0f);
	entityBodyDef.userData.pointer = reinterpret_cast<std::uintptr_t>(this);

	b2PolygonShape collisionShape;
	collisionShape.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &collisionShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	SetBody(world.CreateBody(&entityBodyDef));
	GetBody()->CreateFixture(&fixtureDef);

	b2PolygonShape footPolygonShape;
	footPolygonShape.SetAsBox(0.3f, 0.3f, b2Vec2(0, -2), 0);

	b2FixtureDef footFixtureDef;
	footFixtureDef.isSensor = true;
	footFixtureDef.userData.pointer = static_cast<uintptr_t>(SensorType::PlayerFootSensor);
	footFixtureDef.shape = &footPolygonShape;
	GetBody()->CreateFixture(&fixtureDef);
}

Player::Player(b2World& world, const b2BodyDef& bodyDef, const b2PolygonShape& collisionShape)
	: Entity(world, bodyDef, collisionShape), isFacingRight_(true), moveDirection_(0), footContactsCounter_(0)
{
}

bool Player::IsGrounded() const
{
	return footContactsCounter_ > 1;
}

void Player::Update(const sf::Time deltaTime)
{
	const bool isInputingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
	const bool isInputingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
	const bool isInputingJump = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

	const b2Vec2& vel = GetBody()->GetLinearVelocity();

	// Compute move direction
	if ((isInputingLeft || isInputingRight) && (IsGrounded() || std::abs(vel.x) > 0.01f))
	{
		moveDirection_ = isInputingLeft ? -1 : 1;
	}
	else
	{
		if (IsGrounded() || Magnitude(vel) < 0.01f)
		{
			moveDirection_ = 0;
		}
	}

	// Compute facing direction
	if (moveDirection_ != 0)
	{
		const sf::Vector2f& scale = getScale();
		if (moveDirection_ > 0 && !isFacingRight_)
		{
			isFacingRight_ = true;
			setScale(std::abs(scale.x), scale.y);
		}
		else if (moveDirection_ < 0 && isFacingRight_)
		{
			isFacingRight_ = false;
			setScale(-std::abs(scale.x), scale.y);
		}
	}

	// Apply move
	GetBody()->SetLinearVelocity(b2Vec2(static_cast<float>(moveDirection_) * MAX_SPEED, vel.y));

	// Jumping
	if (isInputingJump && IsGrounded())
	{
		GetBody()->SetLinearVelocity(b2Vec2(vel.x, JUMP_HEIGHT));
	}

	Entity::Update(deltaTime);
}

void Player::StartContact()
{
	footContactsCounter_ += 1;
}

void Player::EndContact()
{
	footContactsCounter_ -= 1;
}
