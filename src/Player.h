#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    // Constructors
    Player();
    Player(int id, const QString &name, int seed = 0);

    // Getters
    int getId() const;
    QString getName() const;
    int getSeed() const;

    // Setters
    void setId(int id);
    void setName(const QString &name);
    void setSeed(int seed);

    // Comparison operators
    bool operator==(const Player &other) const;
    bool operator!=(const Player &other) const;

private:
    int id;
    QString name;
    int seed;
};

#endif // PLAYER_H