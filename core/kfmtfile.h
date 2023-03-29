#ifndef KFMTFILE_H
#define KFMTFILE_H

#include "core/kfmterror.h"
#include <QDir>
#include <QRegularExpression>
#include <QStringView>
#include <memory>

struct KFMTFileListEntry;

class KFMTFile
{
public:
    enum class ContainerType : uint8_t
    {
        Folder,            ///< Dummy folder container
        MIMList,           ///< List of MIM files (KF1 CHRX.MIM)
        MIX_HasSizes,      ///< MIX with file size before each file
        MIX_NoSizes,       ///< MIX with just TIMs/TMDs back-to-back
        T                  ///< KF2 T file
    };

    enum class FileFormat : uint8_t
    {
        Folder,  ///< Dummy file type to represent a folder for file list organization purposes
        MIMList, ///< KF1 MIM list (CHRX.MIM files)
        MIX,     ///< MIX container
        Raw,     ///< Normal file
        Root,    ///< Root of the FSMTFile tree
        T        ///< T container
    };

    enum class DataType : uint8_t
    {
        // Generic
        Container,
        GameDB,
        MapTile,
        MapTilemap,
        MapDB,
        MapScript,
        Model,
        MusicSequence,
        SoundBankBody,
        SoundBankHeader,
        TextureDB,
        Unknown,

        // KF1 specific
        KF1_ArmourParams,
        KF1_LevelCurve,
        KF1_MagicParams,
        KF1_WeaponParams,

        // KF2 specific
        KF2_ArmourParams,
        KF2_GameExec,
        KF2_LevelCurve,
        KF2_MagicParams,
        KF2_ModelPack_MO,
        KF2_ModelPack_TMD,
        KF2_ObjectClasses,
        KF2_SoundEffectParams,
        KF2_TileRenderParams,
        KF2_WeaponParams,

        // FSModTool specific
        Root
    };

    /*!
     * \brief Constructor for making files from raw data.
     * \param srcPath File path.
     * \param data File data.
     * \param parent Pointer to parent file (or root node).
     * \param fileType Type of file.
     * \param dataType Type of data contained in the file.
     * \param prettyName Pretty name for the file.
     */
    KFMTFile(const QString& name, const QByteArray& data, KFMTFile* const parent,
             const FileFormat fileType = FileFormat::Raw,
             const DataType dataType = DataType::Unknown, const QString& prettyName = "");

    /*!
     * \brief Method to extract files from a container onto a directory.
     * \param outDir Directory which to extract files to.
     */
    void extractTo(const QDir& outDir);

    /*!
     * \brief Returns child count for containers.
     */
    [[nodiscard]] inline uint32_t childCount() const { return static_cast<uint32_t>(m_subFiles.size()); }

    inline void createChild(const QString& name, const QByteArray& data,
                            const FileFormat fileType = FileFormat::Raw,
                            const DataType dataType = DataType::Unknown,
                            const QString& prettyName = "")
    {
        m_subFiles.emplace_back(name, data, this, fileType, dataType, prettyName);
    }

    /*!
     * \brief Gets this file's data type.
     */
    [[nodiscard]] inline DataType dataType() const { return m_dataType; }

    /*!
     * \brief Returns an appropriate extension for the file.
     */
    [[nodiscard]] const QString& extension() const;

    /*!
     * \brief Gets this file's format.
     */
    [[nodiscard]] inline FileFormat format() const { return m_fileType; }

    /*!
     * \brief Gets this file's name.
     */
    [[nodiscard]] inline const QString& name() const { return m_name; }

    /*!
     * \brief Gets this file's parent.
     */
    [[nodiscard]] inline const KFMTFile* parent() const { return m_parent; }

    [[nodiscard]] inline KFMTFile* parent() { return m_parent; }
    
    [[nodiscard]] inline const QString& prettyName() const
    {
        if (m_prettyName.isEmpty()) return m_name;
        else return m_prettyName;
    }

    inline void resetRoot()
    {
        fsmt_assert(dataType() == DataType::Root, "KFMTFile::resetRoot: Called for non-root file!");
        m_subFiles.clear();
    }

    /*!
     * \brief Setter for the data type.
     * \param dataType New data type.
     */
    inline void setDataType(const DataType dataType) { m_dataType = dataType; }

    /*!
     * \brief Setter for the file format.
     * \param fileFormat New file format.
     */
    inline void setFileFormat(const FileFormat fileFormat)
    {
        m_fileType = fileFormat;
        
        switch (fileFormat)
        {
            case FileFormat::MIMList: loadMIMList(); break;
            case FileFormat::MIX: loadMIX(); break;
            case FileFormat::T: loadT(); break;
            default: break;
        }
    }

    /*!
     * \brief Setter for the file name.
     * \param name New file name.
     */
    inline void setName(const QString& name) { m_name = name; }

    /*!
     * \brief Setter for the pretty name.
     * \param prettyName New pretty name.
     */
    inline void setPrettyName(const QString& prettyName) { m_prettyName = prettyName; }

    /*!
     * \brief Writes the file to a given output directory.
     * \param outDir Output directory
     */
    void writeFile(const QDir& outDir);

    [[nodiscard]] KFMTFile* operator[](size_t index)
    {
        if (index >= m_subFiles.size()) return nullptr;
        auto i = m_subFiles.begin();
        std::advance(i, index);
        return &*i;
    }

    [[nodiscard]] KFMTFile* operator[](const QStringView& name)
    {
        QList<QStringView> pathTree {name};
        if (name.contains('/'))
            pathTree = name.split('/');
        
        KFMTFile* result = this;
        for (auto& level : pathTree)
        {
            KFMTFile* old = result;
            for (auto& file : result->m_subFiles)
                if (file.m_name == level)
                {
                    result = &file;
                    break;
                }
            if (result == old) // If we didn't find this level, we bail
                return nullptr;
        }
        
        if (result == this)
            return nullptr;
        
        return result;
    }

private:
    QString m_name;

public:
    QByteArray m_data;

private:
    KFMTFile* const m_parent;
    QString m_prettyName;
    FileFormat m_fileType;
    DataType m_dataType;

private:
    /*!
     * \brief Calculates and writes the checksum to a file.
     * THIS SHOULD ONLY BE RUN FOR T FILE SUBFILES.
     */
    void recalculateChecksum();

    void loadMIMList();
    void loadMIX();
    void loadT();
    void writeMIMList(QFile& fileHandle);
    void writeMIX(QFile& fileHandle);
    void writeT(QFile& fileHandle);

    // Container specific stuff
    ContainerType m_containerType;
    // This used to be a std::vector but now that files have pointers to their parents, a realloc on
    // insertion would make a file's paren pointer stale. This was causing a crash as soon as one
    // would try to expand the CD folder in KF2J. Now we use list which should have a negligible
    // performance loss in this case and doesn't cause reallocs, keeping the parent pointers valid.
    std::list<KFMTFile> m_subFiles; ///< List of subfiles if this file is a container
    /*!
     * \brief Maps a T file's original file numbers to the true file numbers.
     * This is necessary because T files usually have many file numbers that point to the same
     * actual file. These are duplicates that we ignore, but the game might depend on. Therefore,
     * when rebuilding a T file it is necessary to rebuild the file offset table with the same
     * "duplicate order" as the original.
     */
    std::unique_ptr<std::map<uint16_t, uint16_t>> m_tFileMap;

    // FileListModel needs access to the subFiles member.
    friend class FileListModel;
    // KFMTCore needs to be able to force set data and file types.
    friend class KFMTCore;
};

#endif // KFMTFILE_H
