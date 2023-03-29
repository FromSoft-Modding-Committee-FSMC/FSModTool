#ifndef KFMTCORE_H
#define KFMTCORE_H

#include "kfmtfile.h"
#include <optional>

class KFMTCore
{
public:
    // The King's Field values in both Game and VersionedGame use the Japanese numeration standard.

    /*!
     * \brief Enum that defines which game is currently loaded without version information.
     * This enum should be used when a simple, non-version dependent game identification is desired.
     */
    enum class SimpleGame
    {
        None, ///< No game loaded.
        AC,   ///< Armored Core
        KF1,  ///< King's Field I
        KF2,  ///< King's Field II
        KF3,  ///< King's Field III
        KFPS, ///< King's Field III: Pilot Style
        ST    /// Shadow Tower
    };

    /*!
     * \brief Retrieves the current game with no version info.
     * \return A value from the KFMTCore::Game enum.
     * This function uses a switch case statement to transform the complex VersionedGame info that
     * KFMTCore stores into the simpler Game enum for checks which don't rely on version-specific
     * details.
     */
    SimpleGame currentGame() const;

    /*!
     * \brief Enum that defines which game is currently loaded, including the specific version.
     */
    enum class VersionedGame
    {
        None,        ///< No game loaded.
        ACJDemoD2,   ///< Armored Core (Japan) Demo from Dengeki PS D2 [SLPM-80109]
        ACJDemoHR7,  ///< Armored Core (Japan) Demo from Hyper PS Remix Vol. 7 [SLPM-80113]
        ACJDemoT977, ///< Armored Core (Japan) Demo from Tech PS 97-7 [SLPM-80108]
        ACUDemoISV4, ///< Armored Core (USA) Demo from Interactive CD Sampler Volume 4 [SCUS-94418]
        ACUDemoPUG3, ///< Armored Core (USA) Demo from PS Underground Number 3 [SCUS-94191] and OPM Demo Disc 2 [SCUS-94198]
        ACUProto,    ///< Armored Core "Preview Only 7-3-97" Prototype
        ACU,         ///< Armored Core (USA) [SCUS-94182, SLUS-01323]
        KF,          ///< King's Field I (Japan) [SLPS 00017, SLPS 03578]
        KFJDemo, ///< King's Field I (Japan) Demo from DemoDemo PlayStation Soukan-gou Vol. 1 [PCPX 96003]
        KF2E,      ///< King's Field II (Europe) [SCES-00510]
        KF2Jv1_0,  ///< King's Field II (Japan) [SLPS-00069]
        KF2Jv1_7,  ///< King's Field II (Japan) (PSOne Classics) [NPJJ-00083]
        KF2Jv1_8A, ///< King's Field II (Japan) [SLPS-91003]
        KF2Jv1_8B, ///< King's Field II (Japan) [SLPS-03579, SLPS-91003, SLPS-91423]
        KF2U,      ///< King's Field (II) (USA) [SLUS-00158]
        KF3Jv1_3,  ///< King's Field III (Japan) [SLPS-00377]
        KF3Jv1_4,  ///< King's Field III (Japan) [SLPS-03580, SLPS-91089]
        KF3U,      ///< King's Field II (III) (USA) [SLUS-00255]
        KFPS,      ///< King's Field III: Pilot Style (Japan) [SLPM-80029]
        STJv1_4,   ///< Shadow Tower (Japan) [SLPS-01420]
        STJv2_2,   ///< Shadow Tower (Japan) [NPJJ-00088]
        STJDemo,   ///< Shadow Tower (Japan) Demo from Hyper PS Remix 1998 Vol. 9 [SLPM-80237]
        STU,       ///< Shadow Tower (USA) [SLUS-00863]
        STUDemo    ///< Shadow Tower (USA) Demo from PlayStation Underground v2.4 [SCUS-94298]
    };

    inline VersionedGame currentVersionedGame() const { return m_curGame; }
    QDir getSrcDir() const { return m_curSourceDirectory; }

    void loadFrom(const QDir& srcDir);
    void saveTo(const QDir& outDir);

    // This member breaks the naming standard on purpose.
    // The idea is that with this we can just use core.files["PATH/TO/FILE"].
    /*!
     * \brief Root of the file tree.
     */
    KFMTFile files {"/",
                    {},
                    nullptr,
                    KFMTFile::FileFormat::Root,
                    KFMTFile::DataType::Root,
                    QStringLiteral(u"Root")};

private:
    void detectGame(const QDir& srcDir);
    KFMTFile::DataType parseDataType(const QString& dataTypeStr);
    KFMTFile::FileFormat parseFileFormat(const QString& fileTypeStr);
    void loadFileList(const QString& fileList);

    VersionedGame m_curGame = VersionedGame::None;
    QDir m_curSourceDirectory;

    // FileListModel needs access to the files member.
    friend class FileListModel;
};

extern KFMTCore core;
extern const QString unknownString;

#endif // KFMTCORE_H
