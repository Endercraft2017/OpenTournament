#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QString>
#include <QDateTime>

class Tournament
{
public:
    // Constructors
    Tournament();
    Tournament(int id, const QString &name, const QDateTime &createdAt = QDateTime::currentDateTime());

    // Getters
    int getId() const;
    QString getName() const;
    QDateTime getCreatedAt() const;

    // Setters
    void setId(int id);
    void setName(const QString &name);
    void setCreatedAt(const QDateTime &createdAt);

    // Comparison operators
    bool operator==(const Tournament &other) const;
    bool operator!=(const Tournament &other) const;

private:
    int id;
    QString name;
    QDateTime createdAt;
};

#endif // TOURNAMENT_H