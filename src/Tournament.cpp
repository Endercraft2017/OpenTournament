#include "Tournament.h"

Tournament::Tournament() : id(0), name(""), createdAt(QDateTime::currentDateTime()),
                           status(Setup), pairingSystem("round_robin"), startedAt(), completedAt() {}

Tournament::Tournament(int id, const QString &name, const QDateTime &createdAt)
    : id(id), name(name), createdAt(createdAt), status(Setup), pairingSystem("round_robin"), startedAt(), completedAt() {}

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

Tournament::Status Tournament::getStatus() const
{
    return status;
}

QString Tournament::getPairingSystem() const
{
    return pairingSystem;
}

QDateTime Tournament::getStartedAt() const
{
    return startedAt;
}

QDateTime Tournament::getCompletedAt() const
{
    return completedAt;
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

void Tournament::setStatus(Tournament::Status status)
{
    this->status = status;
}

void Tournament::setPairingSystem(const QString &pairingSystem)
{
    this->pairingSystem = pairingSystem;
}

void Tournament::setStartedAt(const QDateTime &startedAt)
{
    this->startedAt = startedAt;
}

void Tournament::setCompletedAt(const QDateTime &completedAt)
{
    this->completedAt = completedAt;
}

// Helper methods
QString Tournament::getStatusString() const
{
    switch (status)
    {
    case Setup:
        return "setup";
    case Active:
        return "active";
    case Completed:
        return "completed";
    default:
        return "setup";
    }
}

void Tournament::setStatusFromString(const QString &statusString)
{
    if (statusString == "active")
    {
        status = Active;
    }
    else if (statusString == "completed")
    {
        status = Completed;
    }
    else
    {
        status = Setup;
    }
}

// Comparison operators
bool Tournament::operator==(const Tournament &other) const
{
    return id == other.id && name == other.name && createdAt == other.createdAt &&
           status == other.status && pairingSystem == other.pairingSystem &&
           startedAt == other.startedAt && completedAt == other.completedAt;
}

bool Tournament::operator!=(const Tournament &other) const
{
    return !(*this == other);
}