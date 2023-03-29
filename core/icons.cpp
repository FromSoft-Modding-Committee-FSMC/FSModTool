#include "core/icons.h"

namespace Icons
{
QIcon armour;
QIcon container;
QIcon folder;
QIcon gameDb;
QIcon gameExe;
QIcon levelCurve;
QIcon magic;
QIcon map;
QIcon model;
QIcon objClass;
QIcon soundbankBody;
QIcon soundbankHeader;
QIcon textureDb;
QIcon unknown;
QIcon weapon;

void init()
{
    armour = QIcon(QStringLiteral(":/icons/armour_icon.png"));
    container = QIcon(QStringLiteral(":/icons/tfile_icon.png"));
    folder = QIcon(QStringLiteral(":/icons/folder_icon.png"));
    gameDb = QIcon(QStringLiteral(":/icons/db_icon.png"));
    gameExe = QIcon(QStringLiteral(":/icons/gameexe_icon.png"));
    levelCurve = QIcon(QStringLiteral(":/icons/levelcurve_icon.png"));
    magic = QIcon(QStringLiteral(":/icons/magic_icon.png"));
    map = QIcon(QStringLiteral(":/icons/map_icon.png"));
    model = QIcon(QStringLiteral(":/icons/3d_icon.png"));
    objClass = QIcon(QStringLiteral(":/icons/objclass_icon.png"));
    soundbankBody = QIcon(QStringLiteral(":/icons/sndbody_icon.png"));
    soundbankHeader = QIcon(QStringLiteral(":/icons/sndhead_icon.png"));
    textureDb = QIcon(QStringLiteral(":/icons/tex_icon.png"));
    unknown = QIcon(QStringLiteral(":/icons/unk_icon.png"));
    weapon = QIcon(QStringLiteral(":/icons/weapon_icon.png"));
}

} // namespace Icons
