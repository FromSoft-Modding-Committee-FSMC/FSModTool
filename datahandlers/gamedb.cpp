#include "gamedb.h"
#include "utilities.h"

GameDB::GameDB(KFMTFile& file_) : KFMTDataHandler(file_)
{
    QDataStream stream(file.m_rawData);
    stream.setByteOrder(QDataStream::LittleEndian);
    uint32_t sectionSize;

    // Section 1 - Object Class Declarations
    stream >> sectionSize;
    objClasses = &Utilities::as<KF2::ObjectClass>(file.m_rawData, stream.device()->pos());
    stream.skipRawData(sectionSize);

    // Section 2 - Weapon Stats
    stream >> sectionSize;
    weaponStats = &Utilities::as<KF2::Weapon>(file.m_rawData, stream.device()->pos());
    stream.skipRawData(sectionSize);

    // Section 3 - Armor Stats
    stream >> sectionSize;
    armorStats = &Utilities::as<KF2::Armor>(file.m_rawData, stream.device()->pos());
    stream.skipRawData(sectionSize);

    // Section 4 - Player Level Data
    stream >> sectionSize;
    lvlData = &Utilities::as<KF2::PlayerLevel>(file.m_rawData, stream.device()->pos());
    stream.skipRawData(sectionSize);

    // Section 5 - Magic Data
    stream >> sectionSize;
    magicData = &Utilities::as<KF2::Magic>(file.m_rawData, stream.device()->pos());
    stream.skipRawData(sectionSize);
}
