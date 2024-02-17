#include "pixmapmanager.h"

PixmapManager* PixmapManager::ptr = nullptr;

PixmapManager *PixmapManager::Instance()
{
    if(!ptr)
    {
        ptr = new PixmapManager();
    }
    return ptr;
}

QPixmap& PixmapManager::getPixmap(TextureID id)
{
    return m_textures.get(id);
}

PixmapManager::PixmapManager()
{
    m_textures.load(TextureID::KING_L, QStringLiteral(":/assets/sprite/king_l.png"));
    m_textures.load(TextureID::KING_R, QStringLiteral(":/assets/sprite/king_r.png"));
    m_textures.load(TextureID::QUEEN_L, QStringLiteral(":/assets/sprite/queen_l.png"));
    m_textures.load(TextureID::QUEEN_R, QStringLiteral(":/assets/sprite/queen_r.png"));
    m_textures.load(TextureID::GRASS1, QStringLiteral(":/assets/sprite/grass1.png"));
    m_textures.load(TextureID::GRASS2, QStringLiteral(":/assets/sprite/grass2.png"));
    m_textures.load(TextureID::GRASS3, QStringLiteral(":/assets/sprite/grass3.png"));
    m_textures.load(TextureID::GRASS4, QStringLiteral(":/assets/sprite/grass4.png"));
}
