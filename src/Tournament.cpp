#include "Tournament.h"

Tournament::Tournament() : id(0), name(""), createdAt(QDateTime::currentDateTime()) {}

Tournament::Tournament(int id, const QString &name, const QDateTime &createdAt)
    : id(id), name(name), createdAt(createdAt) {}

// Getters
int Tournament::getId() const
{
    return id;
}

QString Tournament::getName() const
{
    return name;
}

QDateTime Tournament::getCreatedAt() const
{
    return createdAt;
}

// Setters
void Tournament::setId(int id)
{
    this->id = id;
}

void Tournament::setName(const QString &name)
{
    this->name = name;
}

void Tournament::setCreatedAt(const QDateTime &createdAt)
{
    this->createdAt = createdAt;
}

// Comparison operators
bool Tournament::operator==(const Tournament &other) const
{
    return id == other.id && name == other.name && createdAt == other.createdAt;
}

bool Tournament::operator!=(const Tournament &other) const
{
    return !(*this == other);
}