#ifndef KFMTTREEWIDGETITEM_H
#define KFMTTREEWIDGETITEM_H

#include "gamedb.h"
#include "map.h"
#include "model.h"
#include "texturedb.h"
#include <QTreeWidgetItem>

enum class KFMTDataType
{
    KFMT_GAMEDB,
    KFMT_MAP,
    KFMT_MODEL,
    KFMT_TEXTUREDB
};

class KFMTTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit KFMTTreeWidgetItem(QTreeWidgetItem *parent, std::shared_ptr<GameDB> kfmtDB);
    explicit KFMTTreeWidgetItem(QTreeWidgetItem *parent, std::shared_ptr<Map> kfmtMap);
    explicit KFMTTreeWidgetItem(QTreeWidgetItem *parent, std::shared_ptr<Model> kfmtModel);
    explicit KFMTTreeWidgetItem(QTreeWidgetItem *parent, std::shared_ptr<TextureDB> kfmtTextureDB);

    std::shared_ptr<GameDB> getDB()
    {
        if (dataType == KFMTDataType::KFMT_GAMEDB)
            return dbPtr;
        else
            return nullptr;
    }

    std::shared_ptr<Map> getMap()
    {
        if (dataType == KFMTDataType::KFMT_MAP)
            return mapPtr;
        else
            return nullptr;
    }
    
    std::shared_ptr<Model> getModel()
    {
        if (dataType == KFMTDataType::KFMT_MODEL)
            return modelPtr;
        else
            return nullptr;
    }
    
    std::shared_ptr<TextureDB> getTextureDB()
    {
        if (dataType == KFMTDataType::KFMT_TEXTUREDB)
            return texDBPtr;
        else
            return nullptr;
    }

    KFMTDataType getType() { return dataType; }
    
    void writeChanges();

private:
    KFMTDataType dataType;
    std::shared_ptr<GameDB> dbPtr = nullptr;
    std::shared_ptr<Map> mapPtr = nullptr;
    std::shared_ptr<Model> modelPtr = nullptr;
    std::shared_ptr<TextureDB> texDBPtr = nullptr;
};

#endif // KFMTTREEWIDGETITEM_H
