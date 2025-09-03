#include "Player.h"

Player::Player() : id(0), name(""), seed(0) {}

Player::Player(int id, const QString &name, int seed)
    : id(id), name(name), seed(seed) {}

// Getters
int Player::getId() const
{
    return id;
}

QString Player::getName() const
{
    return name;
}

int Player::getSeed() const
{
    return seed;
}

// Setters
void Player::setId(int id)
{
    this->id = id;
}

void Player::setName(const QString &name)
{
    this->name = name;
}

void Player::setSeed(int seed)
{
    this->seed = seed;
}

// Comparison operators
bool Player::operator==(const Player &other) const
{
    return id == other.id && name == other.name && seed == other.seed;
}

bool Player::operator!=(const Player &other) const
{
    return !(*this == other);
}