#include "gamescene.h"
#include "utils.h"
#include "fontmanager.h"
#include "pixmapmanager.h"
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QDir>
#include <QPainter>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
{
    onUserCreate();
}

GameScene::~GameScene()
{
    m_grassObjs.clear();
}

void GameScene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    while(m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;
        if (m_invulnerableMode && ((m_appStartTimer.elapsed() - m_invulnerableStartTime) > CONF::INVULNTIME)) {
            m_invulnerableMode = false;
        }

        for (int i = 0; i < m_queenObjs.size(); ++i) {
            Queen* queen = m_queenObjs.at(i);
            queen->m_x += queen->m_movex;
            queen->m_y += queen->m_movey;
            queen->m_bounce += 1;
            if (queen->m_bounce > queen->m_bouncerate) {
                queen->m_bounce = 0;
            }

            if (QRandomGenerator::global()->bounded(0, 100) < CONF::DIRCHANGEFREQ) {
                queen->m_movex = queen->getRandomVelocity();
                queen->m_movey = queen->getRandomVelocity();
            }
        }

        for (Grass* grass : m_grassObjs) {
            if (isOutsideActiveArea(grass)) {
                m_grassObjs.removeOne(grass);
            }
        }

        for (Queen* queen : m_queenObjs) {
            if (isOutsideActiveArea(queen)) {
                m_queenObjs.removeOne(queen);
            }
        }

        while (m_grassObjs.size() < CONF::NUMGRASS) {
            m_grassObjs.append(new Grass(m_camerax, m_cameray));
        }

        while (m_queenObjs.size() < CONF::NUMGRASS) {
            m_queenObjs.append(new Queen(m_camerax, m_cameray));
        }

        int playerCenterx = m_player->m_x + std::floor(m_player->m_size / 2);
        int playerCentery = m_player->m_y + std::floor(m_player->m_size / 2);
        if ( (m_camerax + SCREEN::PHYSICAL_SIZE.width() / 2) - playerCenterx > CONF::CAMERASLACK) {
            m_camerax = playerCenterx + CONF::CAMERASLACK - SCREEN::PHYSICAL_SIZE.width() / 2;
        }
        else if (playerCenterx - (m_camerax + SCREEN::PHYSICAL_SIZE.width()/2) > CONF::CAMERASLACK)
        {
            m_camerax = playerCenterx - CONF::CAMERASLACK - SCREEN::PHYSICAL_SIZE.width() / 2;
        }

        if ( (m_cameray + SCREEN::PHYSICAL_SIZE.height() / 2) - playerCentery > CONF::CAMERASLACK) {
            m_cameray = playerCentery + CONF::CAMERASLACK - SCREEN::PHYSICAL_SIZE.height() / 2;
        }
        else if (playerCentery - (m_cameray + SCREEN::PHYSICAL_SIZE.height()/2) > CONF::CAMERASLACK)
        {
            m_cameray = playerCentery - CONF::CAMERASLACK - SCREEN::PHYSICAL_SIZE.height() / 2;
        }

        handlePlayerInput();

        if (!m_gameOverMode) {
            //actually move the player
            if (m_moveLeft) {
                m_player->m_x -= CONF::MOVERATE;
            }
            if (m_moveRight) {
                m_player->m_x += CONF::MOVERATE;
            }
            if (m_moveUp) {
                m_player->m_y -= CONF::MOVERATE;
            }
            if (m_moveDown) {
                m_player->m_y += CONF::MOVERATE;
            }
            if ((m_moveDown || m_moveRight || m_moveLeft || m_moveUp) || m_player->m_bounce != 0) {
                m_player->m_bounce += 1;
            }
            if (m_player->m_bounce > CONF::BOUNCERATE) {
                m_player->m_bounce = 0;
            }

            for (int i = 0; i < m_queenObjs.size(); ++i) {
                Queen* queen = m_queenObjs.at(i);
                if (checkAABBCollision(m_player, queen) && !m_winMode) {
                    if (queen->m_width * queen->m_height <= m_player->m_size * m_player->m_size ) {
                        m_player->m_size += std::floor( std::pow(queen->m_width * queen->m_height, 0.2f) ) + 1;
                        m_queenObjs.removeOne(queen);
                        if (m_player->m_size > CONF::WINSIZE) {
                            m_winMode = true;
                        }
                    }
                    else if (!m_invulnerableMode) {
                        m_invulnerableMode = true;
                        m_invulnerableStartTime = m_appStartTimer.elapsed();
                        m_player->m_health -= 1;
                        if (m_player->m_health == 0) {
                            m_gameOverMode = true;
                            // turn on "game over mode"
                            m_gameOverStartTime = m_appStartTimer.elapsed();
                        }
                    }
                }
            }
        }

        clear();
        draw();
        resetStatus();
    }
}

void GameScene::onUserCreate()
{
    setBackgroundBrush(COLOR_STYLE::BACKGROUND);
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i] = new KeyStatus();
    }
    m_mouse = new MouseStatus();
    setSceneRect(0,0, SCREEN::PHYSICAL_SIZE.width(), SCREEN::PHYSICAL_SIZE.height());
    connect(&m_timer, &QTimer::timeout, this, &GameScene::loop);
    m_timer.start(int(1000.0f/FPS));
    m_elapsedTimer.start();

    for(int i = 0; i < 10; ++i) {
        Grass* grass = new Grass(m_camerax, m_cameray);
        grass->m_x = QRandomGenerator::global()->bounded(0, SCREEN::PHYSICAL_SIZE.width());
        grass->m_y = QRandomGenerator::global()->bounded(0, SCREEN::PHYSICAL_SIZE.height());
        m_grassObjs.push_back(grass);
    }
    m_player = new Player();
    m_appStartTimer.start();
    m_moveLeft = m_moveRight = m_moveUp = m_moveDown = false;
    m_invulnerableMode = false;
    m_invulnerableStartTime = m_appStartTimer.elapsed();

    m_bgMusic = new QSoundEffect(this);
    m_bgMusic->setSource(QUrl(QStringLiteral("qrc:/assets/music/11._jester_battle.wav")));
    m_bgMusic->setLoopCount(QSoundEffect::Infinite);
    m_bgMusic->setVolume(0.5f);
    m_bgMusic->play();
}

void GameScene::renderScene()
{
    static int index = 0;
    QString fileName = QDir::currentPath() + QDir::separator() + "screen" + QString::number(index++) + ".png";
    QRect rect = sceneRect().toAlignedRect();
    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter);
    image.save(fileName);
    qDebug() << "saved " << fileName;
}

void GameScene::handlePlayerInput()
{
    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Up]]->m_pressed)
    {
        m_moveDown = false;
        m_moveUp   = true;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Down]]->m_pressed)
    {
        m_moveDown = true;
        m_moveUp   = false;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Left]]->m_pressed)
    {
        m_moveLeft  = true;
        m_moveRight = false;
        m_player->m_facing = CONF::LEFT;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Right]]->m_pressed)
    {
        m_moveRight = true;
        m_moveLeft  = false;
        m_player->m_facing = CONF::RIGHT;
    }


    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Up]]->m_released)
    {
        m_moveUp   = false;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Down]]->m_released)
    {
        m_moveDown = false;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Left]]->m_released)
    {
        m_moveLeft  = false;
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Right]]->m_released)
    {
        m_moveRight = false;
    }
    else if (m_keys[KEYBOARD::KeysMapper[Qt::Key_R]]->m_released && m_winMode)
    {
        //reset()
    }
    else if (m_keys[KEYBOARD::KeysMapper[Qt::Key_R]]->m_released && m_gameOverMode)
    {
        //reset()
    }

    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Z]]->m_released)
    {
        //renderScene();//uncoment if want to make screenshots
    }
    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_A]]->m_released)
    {
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_D]]->m_released)
    {
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_S]]->m_released)
    {
    }
    else if(m_keys[KEYBOARD::KeysMapper[Qt::Key_W]]->m_released)
    {
    }
}

void GameScene::resetStatus()
{
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_released = false;
    }
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_pressed = false;
    }
    m_mouse->m_released = false;
}

void GameScene::draw()
{
    for (Grass* grass : m_grassObjs)
    {
        QPixmap pixmap(grass->m_grassImgPath);
        QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(pixmap);
        pItem->setX(grass->m_x - m_camerax);
        pItem->setY(grass->m_y - m_cameray);
        addItem(pItem);
    }

    const int QUEEN_IMG_WIDTH  = 16;
    const int QUEEN_IMG_HEIGHT = 16;
    for (Queen* queen : m_queenObjs) {
        qreal scaleX  = qreal(queen->m_width) / qreal(QUEEN_IMG_WIDTH);
        qreal scaleY = qreal(queen->m_height) / qreal(QUEEN_IMG_HEIGHT);
        int xPos = queen->m_x - m_camerax;
        int yPos = queen->m_y - m_cameray - queen->getBounceAmount();
        QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem();
        if (queen->m_movex > 0) {
            pItem->setPixmap(PixmapManager::Instance()->getPixmap(PixmapManager::TextureID::QUEEN_R));
        }
        else {
            pItem->setPixmap(PixmapManager::Instance()->getPixmap(PixmapManager::TextureID::QUEEN_L));
        }
        pItem->setScale((scaleX + scaleY) / 2);
        pItem->setPos(xPos, yPos);
        addItem(pItem);
    }


    static int flashIsOn = 1;
    flashIsOn++;
    const int KING_IMG_WIDTH  = 16;
    const int KING_IMG_HEIGHT = 16;
    if (!m_gameOverMode && !(m_invulnerableMode && (flashIsOn % 2) )) {
        qreal scaleX  = qreal(m_player->m_size) / qreal(KING_IMG_WIDTH);
        qreal scaleY = qreal(m_player->m_size) / qreal(KING_IMG_HEIGHT);
        int xPos = m_player->m_x - m_camerax;
        int yPos = m_player->m_y - m_cameray - m_player->getBounceAmount();
        QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem();
        if (m_player->m_facing == CONF::RIGHT) {
            pItem->setPixmap(PixmapManager::Instance()->getPixmap(PixmapManager::TextureID::KING_R));
        }
        else {
            pItem->setPixmap(PixmapManager::Instance()->getPixmap(PixmapManager::TextureID::KING_L));
        }
        pItem->setScale((scaleX + scaleY) / 2);
        pItem->setPos(xPos, yPos);
        addItem(pItem);
    }

    drawHealthMeter();

    if (m_gameOverMode) {
        QGraphicsSimpleTextItem* tItem = new QGraphicsSimpleTextItem();
        QFont font = FontManager::Instance()->getFont(FontManager::FontID::BASIC);
        font.setPixelSize(32);
        tItem->setFont(font);
        tItem->setText(CONF::GAMEOVERTXT);
        tItem->setBrush(Qt::white);
        tItem->setPos(SCREEN::PHYSICAL_SIZE.width() / 2 - tItem->boundingRect().width() / 2,
                      SCREEN::PHYSICAL_SIZE.height() / 2 - tItem->boundingRect().height() / 2);
        addItem(tItem);
    }
    else if (m_winMode) {
        QGraphicsSimpleTextItem* tItem = new QGraphicsSimpleTextItem();
        QFont font = FontManager::Instance()->getFont(FontManager::FontID::BASIC);
        font.setPixelSize(32);
        tItem->setFont(font);
        tItem->setText(QStringLiteral("You win !"));
        tItem->setBrush(Qt::white);
        tItem->setPos(SCREEN::PHYSICAL_SIZE.width() / 2 - tItem->boundingRect().width() / 2,
                      SCREEN::PHYSICAL_SIZE.height() / 2 - tItem->boundingRect().height() / 2);
        addItem(tItem);
    }
}

bool GameScene::isOutsideActiveArea(Queen* queen)
{
    int boundsLeftEdge    = m_camerax - SCREEN::PHYSICAL_SIZE.width();
    int boundsTopEdge     = m_cameray - SCREEN::PHYSICAL_SIZE.height();
    int boundsRightEdge   = 2 * SCREEN::PHYSICAL_SIZE.width();
    int boundsBottomEdge  = 2 * SCREEN::PHYSICAL_SIZE.height();

    return !(queen->m_x + queen->m_width > boundsLeftEdge &&
             queen->m_x < boundsRightEdge &&
             queen->m_y + queen->m_height > boundsTopEdge &&
             queen->m_y < boundsBottomEdge
             );
}

bool GameScene::isOutsideActiveArea(Grass* grass)
{
    int boundsLeftEdge    = m_camerax - SCREEN::PHYSICAL_SIZE.width();
    int boundsTopEdge     = m_cameray - SCREEN::PHYSICAL_SIZE.height();
    int boundsRightEdge   = 2 * SCREEN::PHYSICAL_SIZE.width();
    int boundsBottomEdge  = 2 * SCREEN::PHYSICAL_SIZE.height();

    return !(grass->m_x + grass->m_width > boundsLeftEdge &&
             grass->m_x < boundsRightEdge &&
             grass->m_y + grass->m_height > boundsTopEdge &&
             grass->m_y < boundsBottomEdge
             );
}

bool GameScene::checkAABBCollision(Player *player, Queen *queen)
{
    return  queen->m_x                      < player->m_x + player->m_size  &&
            queen->m_x + queen->m_width  > player->m_x                   &&
            queen->m_y                      < player->m_y  + player->m_size &&
            queen->m_y + queen->m_height > player->m_y;
}

void GameScene::drawHealthMeter()
{
    for (int i = 0; i < m_player->m_health; ++i) {
        QGraphicsRectItem* rItem = new QGraphicsRectItem(0,0, 20,10);
        QPen pen(QBrush(Qt::white), 2);
        rItem->setPen(pen);
        QBrush brush(Qt::red);
        rItem->setBrush(brush);
        rItem->setPos(15, 5 + (10 * CONF::MAXHEALTH) - i * 10);
        addItem(rItem);
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
        }
        else
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held    = false;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(!event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_released = true;
        }

    }
    QGraphicsScene::keyReleaseEvent(event);
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = true;
    QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    QGraphicsScene::mouseMoveEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = false;
    m_mouse->m_released = true;
    QGraphicsScene::mouseReleaseEvent(event);
}
