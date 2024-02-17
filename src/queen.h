#ifndef QUEEN_H
#define QUEEN_H
#include <QPoint>
#include <QRandomGenerator>
#include <cmath>
#include "utils.h"

struct Queen {
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    int m_movex;
    int m_movey;
    int m_bounce;
    int m_bouncerate;
    int m_bounceheight;
    Queen(int camerax, int cameray) {
        int generalSize = QRandomGenerator::global()->bounded(5, 26); // Qt generates [5, 26)
        int multiplier =  QRandomGenerator::global()->bounded(1, 5); // Qt generates [1, 5)

        m_width  = std::floor(generalSize + QRandomGenerator::global()->bounded(1, 11) * multiplier);
        m_height = std::floor(generalSize + QRandomGenerator::global()->bounded(1, 11) * multiplier);

        QPoint pos = getRandomOffCameraPos(camerax, cameray);
        m_x = pos.x();
        m_y = pos.y();
        m_movex = getRandomVelocity();
        m_movey = getRandomVelocity();
        m_bounce = 0;
        m_bouncerate = QRandomGenerator::global()->bounded(10, 18);
        m_bounceheight = QRandomGenerator::global()->bounded(10, 50);
    }

    QPoint getRandomOffCameraPos(int camerax, int cameray) {
        int x = QRandomGenerator::global()->bounded(camerax - SCREEN::PHYSICAL_SIZE.width(), camerax + (2 * SCREEN::PHYSICAL_SIZE.width()));
        int y = QRandomGenerator::global()->bounded(cameray - SCREEN::PHYSICAL_SIZE.height(), camerax + (2 * SCREEN::PHYSICAL_SIZE.height()));
        //Check if the object is off-camera
        if (x + m_width < camerax || x > camerax + SCREEN::PHYSICAL_SIZE.width() || y + m_height < cameray || y > cameray + SCREEN::PHYSICAL_SIZE.height())
        {
            return QPoint(x, y);
        }
        else {
            return QPoint(0, 0);
        }
    }

    int getRandomVelocity() {
        int speed = QRandomGenerator::global()->bounded(CONF::SQUIRRELMINSPEED, CONF::SQUIRRELMAXSPEED);
        int direction = QRandomGenerator::global()->bounded(0, 2);
        if (!direction) {
            return speed;
        }
        else {
            return -speed;
        }
    }

    int getBounceAmount() {
        return std::floor(std::sin( (3.14f / m_bouncerate) * m_bounce) * m_bounceheight);
    }
};

#endif // QUEEN_H
