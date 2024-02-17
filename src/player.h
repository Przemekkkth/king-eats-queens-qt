#ifndef PLAYER_H
#define PLAYER_H
#include "utils.h"

struct Player {
    QString m_facing;
    int m_size;
    int m_x;
    int m_y;
    int m_bounce;
    int m_health;
    Player() {
        m_facing = CONF::LEFT;
        m_size = CONF::STARTSIZE;
        m_x = SCREEN::PHYSICAL_SIZE.width() / 2;
        m_y = SCREEN::PHYSICAL_SIZE.height() / 2;
        m_bounce = 0;
        m_health = CONF::MAXHEALTH;
    }
    int getBounceAmount() {
        return std::floor(std::sin( (3.14f / CONF::BOUNCERATE) * m_bounce) * CONF::BOUNCEHEIGHT);
    }
};

#endif // PLAYER_H
