#ifndef KFMTFILE_H
#define KFMTFILE_H

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
        MIX_HasSizes,      ///< MIX with file size before each file
        MIX_NoSizes,       ///< MIX with just TIMs/TMDs back-to-back
        MIX_MIMCollection, ///< MIX for MIM files with 06 or 07 at the beginning
        T                  ///< KF2 T file
    };

    enum class FileType : uint8_t
    {
        Folder, ///< Dummy file type to represent a folder for file list organization purposes
        MIX,    ///< MIX container
        Raw,    ///< Normal file
        T       ///< T container
    };

    enum class DataType : uint8_t
    {
        // Generic
        Container,
        GameDB,
        GameEXE,
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
        KF2_LevelCurve,
        KF2_MagicParams,
        KF2_ObjectClasses,
        KF2_WeaponParams
    };

    /*!
     * \brief Standard KFMTFile Constructor
     * \param srcDir Path to the source directory for the game files.
     * \param path Path to this file in the source directory.
     * \param fileType Type of file (MIX, Raw or T).
     * \param dataType Optional specifier for the file's data type. If left unspecified, the data type will be automatically detected.
     */
    KFMTFile(const QString& path, const KFMTFileListEntry* fileListEntry);

    /*!
     * \brief Constructor for making files from raw data.
     * \param srcPath Path to the container for this file (incl. source directory) + file number.
     * \param data Byte array with data for this file.
     * \param dataType Optional specifier for the file's data type. If left unspecified, the data type will be automatically detected.
     */
    KFMTFile(const QString& path, const QByteArray& data, const KFMTFileListEntry* fileListEntry)
        : m_path(path), m_fileListEntry(fileListEntry), m_rawData(data)
    {}

    /*!
     * \brief Method to extract files from a container onto a directory.
     * \param outDir Directory which to extract files to.
     */
    void extractTo(const QDir& outDir);

    [[nodiscard]] DataType dataType() const;
    [[nodiscard]] const QString& fileExtension() const;
    [[nodiscard]] auto fileName() const;
    [[nodiscard]] auto filePath() const;
    [[nodiscard]] FileType fileType() const;
    [[nodiscard]] uint32_t indexInContainer();
    [[nodiscard]] inline KFMTFile* operator[](size_t index)
    {
        return index < m_subFiles.size() ? &m_subFiles[index] : nullptr;
    }
    [[nodiscard]] size_t subFileCount() const { return m_subFiles.size(); }

    void writeFile(const QDir& outDir);

private:
    const QString m_path;
    const KFMTFileListEntry* const m_fileListEntry;

public:
    QByteArray m_rawData;

private:
    /*!
     * \brief Calculates and writes the checksum to a file.
     * THIS SHOULD ONLY BE RUN FOR T FILE SUBFILES.
     */
    void recalculateChecksum();

    void loadMIX(QFile& fileHandle);
    void loadT(QFile& fileHandle);
    void writeMIX(QFile& fileHandle);
    void writeT(QFile& fileHandle);

    uint32_t m_mixMimSignature; ///< Signature to write in case type == Type::MIMCollection

    // Container specific stuff
    ContainerType containerType;
    std::vector<KFMTFile> m_subFiles; ///< Vector of subfiles if this file is a container
    /*!
     * \brief Maps a T file's original file numbers to the true file numbers.
     * This is necessary because T files usually have many file numbers that point to the same
     * actual file. These are duplicates that we ignore, but the game might depend on. Therefore,
     * when rebuilding a T file it is necessary to rebuild the file offset table with the same
     * "duplicate order" as the original.
     */
    std::unique_ptr<std::map<uint32_t, uint32_t>> m_tFileMap;

    // FileListModel needs access to the subFiles member.
    friend class FileListModel;
    // KFMTCore needs to be able to force set data and file types.
    friend class KFMTCore;
};

struct KFMTFileListEntry
{
    constexpr KFMTFileListEntry(const QStringView path, const QStringView prettyName,
                                const KFMTFile::FileType fileType,
                                const KFMTFile::DataType dataType)
        : m_path(path), m_prettyName(prettyName), m_fileType(fileType), m_dataType(dataType)
    {}

    constexpr KFMTFileListEntry(const QStringView path, const KFMTFile::FileType fileType)
        : m_path(path), m_prettyName(), m_fileType(fileType),
          m_dataType(KFMTFile::DataType::Container)
    {
        if (fileType == KFMTFile::FileType::Raw)
            throw "KFMTFileListEntry container-specific constructor used for non-container file.";
    }

    const QStringView m_path;
    const QStringView m_prettyName;
    const KFMTFile::FileType m_fileType;
    const KFMTFile::DataType m_dataType;
    [[nodiscard]] inline auto prettyName() const
    {
        return (m_prettyName.empty()) ? m_path : m_prettyName;
    }
};

inline KFMTFile::DataType KFMTFile::dataType() const
{
    return m_fileListEntry->m_dataType;
}

inline auto KFMTFile::fileName() const
{
    return m_fileListEntry->prettyName();
}

inline auto KFMTFile::filePath() const
{
    return m_fileListEntry->prettyName();
}

inline KFMTFile::FileType KFMTFile::fileType() const
{
    return m_fileListEntry->m_fileType;
}

#endif // KFMTFILE_H
