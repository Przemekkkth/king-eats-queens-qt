#include "fontmanager.h"
#include <QFontDatabase>

FontManager::FontManager()
{
    int id = QFontDatabase::addApplicationFont(QStringLiteral(":/assets/font/freesansbold.ttf"));
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont stringFont(family);
    m_fontMap[FontID::BASIC] = stringFont;
}

FontManager* FontManager::ptr = nullptr;

FontManager *FontManager::Instance()
{
    if(!ptr)
    {
        ptr = new FontManager();
    }
    return ptr;
}

QFont FontManager::getFont(FontID id) const
{
    return m_fontMap[id];
}
