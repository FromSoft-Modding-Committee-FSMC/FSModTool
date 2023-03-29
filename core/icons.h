#ifndef ICONS_H
#define ICONS_H

#include <QIcon>
/*!
 * Namespace for icons used by multiple different places.
 * This is a memory saving measure, and unfortunately it has to be done this way due to limitations
 * with how QIcon works. This might be overengineering, but for it pleases the speed freak part of
 * my brain.
 */
namespace Icons
{
extern QIcon armour;
extern QIcon container;
extern QIcon folder;
extern QIcon gameDb;
extern QIcon gameExe;
extern QIcon levelCurve;
extern QIcon magic;
extern QIcon map;
extern QIcon model;
extern QIcon objClass;
extern QIcon soundbankBody;
extern QIcon soundbankHeader;
extern QIcon textureDb;
extern QIcon unknown;
extern QIcon weapon;

void init();
} // namespace Icons

#endif // ICONS_H
