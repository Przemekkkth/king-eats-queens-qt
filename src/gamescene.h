#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QElapsedTimer>
#include <QTimer>
#include <QImage>
#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QSoundEffect>
#include "grass.h"
#include "player.h"
#include "queen.h"

struct KeyStatus
{
    bool m_pressed = false;
    bool m_held = false;
    bool m_released = false;
};

struct MouseStatus
{
    float m_x = 0.0f;
    float m_y = 0.0f;
    bool m_released = false;
    bool m_pressed = false;
};

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();
private slots:
    void loop();

private:
    void onUserCreate();
    void renderScene();
    void handlePlayerInput();
    void resetStatus();
    void draw();
    KeyStatus* m_keys[256];
    MouseStatus* m_mouse;
    const int FPS = CONF::FPS;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    float m_deltaTime = 0.0f, m_loopTime = 0.0f;
    const float m_loopSpeed = int(1000.0f/FPS);

    // set up variables for the start of a new game
    bool m_invulnerableMode = false; //  -- if the player is invulnerable
    bool m_gameOverMode = false;      // -- if the player has lost
    bool m_winMode = false;           //-- if the player has won
    int m_camerax = 0;
    int m_cameray = 0;
    QList<Grass*> m_grassObjs;
    QList<Queen*> m_queenObjs;
    Player* m_player;
    QElapsedTimer m_appStartTimer;
    qint64 m_gameOverStartTime = 0, m_invulnerableStartTime = 0;
    bool isOutsideActiveArea(Queen*);
    bool isOutsideActiveArea(Grass*);
    bool m_moveLeft, m_moveRight, m_moveUp, m_moveDown;
    bool checkAABBCollision(Player* player, Queen* queen);
    void drawHealthMeter();
    QSoundEffect *m_bgMusic;
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};


#endif // GAMESCENE_H
