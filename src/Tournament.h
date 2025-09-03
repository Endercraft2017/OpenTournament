#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QString>
#include <QDateTime>

class Tournament
{
public:
    // Tournament status enum
    enum Status
    {
        Setup,
        Active,
        Completed
    };

    // Constructors
    Tournament();
    Tournament(int id, const QString &name, const QDateTime &createdAt = QDateTime::currentDateTime());

    // Getters
    int getId() const;
    QString getName() const;
    QDateTime getCreatedAt() const;
    Status getStatus() const;
    QString getPairingSystem() const;
    QDateTime getStartedAt() const;
    QDateTime getCompletedAt() const;

    // Setters
    void setId(int id);
    void setName(const QString &name);
    void setCreatedAt(const QDateTime &createdAt);
    void setStatus(Status status);
    void setPairingSystem(const QString &pairingSystem);
    void setStartedAt(const QDateTime &startedAt);
    void setCompletedAt(const QDateTime &completedAt);

    // Helper methods
    QString getStatusString() const;
    void setStatusFromString(const QString &status);

    // Comparison operators
    bool operator==(const Tournament &other) const;
    bool operator!=(const Tournament &other) const;

private:
    int id;
    QString name;
    QDateTime createdAt;
    Status status;
    QString pairingSystem;
    QDateTime startedAt;
    QDateTime completedAt;
};

#endif // TOURNAMENT_H