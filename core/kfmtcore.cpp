#include "kfmtcore.h"
#include "kf1/filelist.h"
#include "kfmterror.h"
#include "kfmtfile.h"
#include <QCryptographicHash>

// Declaring the global
KFMTCore core;
// Declaring the unknown string
const QString unknownString = QStringLiteral("???");

KFMTCore::SimpleGame KFMTCore::currentGame() const
{
    switch (curGame)
    {
        case VersionedGame::None: return SimpleGame::None;
        case VersionedGame::ACU: [[fallthrough]];
        case VersionedGame::ACDemoD2:  [[fallthrough]];
        case VersionedGame::ACDemoHR7: [[fallthrough]];
        case VersionedGame::ACDemoISV4: [[fallthrough]];
        case VersionedGame::ACDemoPUG3: [[fallthrough]];
        case VersionedGame::ACDemoT977: [[fallthrough]];
        case VersionedGame::ACProto: return SimpleGame::AC;
        case VersionedGame::KF: [[fallthrough]];
        case VersionedGame::KFDemo: return SimpleGame::KF1;
        case VersionedGame::KF2E: [[fallthrough]];
        case VersionedGame::KF2Jv1_0: [[fallthrough]];
        case VersionedGame::KF2Jv1_7: [[fallthrough]];
        case VersionedGame::KF2Jv1_8A: [[fallthrough]];
        case VersionedGame::KF2Jv1_8B: [[fallthrough]];
        case VersionedGame::KF2U: return SimpleGame::KF2;
        case VersionedGame::KF3Jv1_3: [[fallthrough]];
        case VersionedGame::KF3Jv1_4: [[fallthrough]];
        case VersionedGame::KF3U: return SimpleGame::KF3;
        case VersionedGame::KFPS: return SimpleGame::KFPS;
        case VersionedGame::ST: [[fallthrough]];
        case VersionedGame::STDemo: return SimpleGame::ST;
        default: KFMTError::fatalError(QStringLiteral("KFMTCore::currentGame: Unhandled game!"));
    }
}

KFMTFile* KFMTCore::getFile(const QStringView& fileName, std::optional<size_t> subFile)
{
    for (auto& f : files)
        if (f.m_path.endsWith(fileName))
            return subFile ? f[subFile.value()] : &f;

    return nullptr;
}

void KFMTCore::loadFrom(const QDir& srcDir)
{
    detectGame(srcDir);
    files.clear();
    if (curGame == VersionedGame::None)
        return;
    curSourceDirectory = srcDir;
    const KFMTFileListEntry* fileList = nullptr;
    uint32_t fileCount = 0;
    switch (curGame)
    {
        case VersionedGame::KF:
            fileList = KF1::fileList;
            fileCount = sizeof(KF1::fileList) / sizeof(KFMTFileListEntry);
            break;
        default:
            KFMTError::error(QStringLiteral("KFMTCore::loadFrom: Unhandled Game value %1")
                                 .arg(static_cast<int>(curGame)));
            break;
    }

    for (auto* entry = fileList; entry < fileList + fileCount; entry++)
    {
        // We shouldn't attempt to load subfiles.
        if (entry->m_path.contains(':')) continue;

        QString path {entry->m_path.data()};
#ifdef __MINGW32__
        path->replace(QChar('/'), QDir::separator());
#endif
        if (!QFile(curSourceDirectory.absoluteFilePath(path)).exists())
            continue;

        files.emplace_back(curSourceDirectory.path() + QDir::separator() + path, entry);
    }

    if (curGame != VersionedGame::KF2E && curGame != VersionedGame::KF2Jv1_0
        && curGame != VersionedGame::KF2Jv1_8A && curGame != VersionedGame::KF2Jv1_8B
        && curGame != VersionedGame::KF2U)
        KFMTError::warning(QStringLiteral("Support for the game you loaded is INCOMPLETE. Things "
                                          "will be broken, and names will be incorrect."));
}

void KFMTCore::saveTo(const QDir& outDir)
{
    for (auto& file : files) file.writeFile(outDir);
}

void KFMTCore::detectGame(const QDir& srcDir)
{
    if (srcDir.exists("SCUS_941.82") || srcDir.exists("SLUS_013.23"))
        curGame = VersionedGame::ACU;
    else if (srcDir.exists("STR") && srcDir.exists("AC.EXE"))
        curGame = VersionedGame::ACDemoD2;
    else if (srcDir.exists("STR") && srcDir.exists("DEMO1"))
        curGame = VersionedGame::ACDemoHR7;
    else if (srcDir.exists("USA10.EXE"))
    {
        if (srcDir.exists(QStringLiteral("COM%1DEMO00.XA").arg(QDir::separator())))
            curGame = VersionedGame::ACDemoISV4;
        else if (srcDir.exists(QStringLiteral("COM%1DEMO01.XA").arg(QDir::separator())))
            curGame = VersionedGame::ACDemoPUG3;
    }
    else if (!srcDir.exists("STR") && srcDir.exists("AC.EXE"))
        curGame = VersionedGame::ACDemoT977;
    else if (srcDir.exists("SLPS_009.00"))
        curGame = VersionedGame::ACProto;
    else if (srcDir.exists("PSX.EXE") && srcDir.exists("E0") && srcDir.exists("E1")
             && srcDir.exists("E2") && srcDir.exists("E3"))
        curGame = VersionedGame::KF;
    else if (srcDir.exists("KFIELD.EXE") && srcDir.exists("MAP.001"))
        curGame = VersionedGame::KFDemo;
    else if (srcDir.exists("SCES_005.10") && srcDir.exists("END.EXE")
             && srcDir.exists("LICENSEE.DAT"))
        curGame = VersionedGame::KF2E;
    else if (srcDir.exists("END.EXE") && srcDir.exists("GAME.EXE") && srcDir.exists("OPEN.EXE")
             && srcDir.exists("PSX.EXE"))
    {
        auto gameExeHandle = QFile(srcDir.absoluteFilePath("GAME.EXE"));
        if (!gameExeHandle.open(QIODevice::ReadOnly)) return;

        auto gameExeHash = QCryptographicHash::hash(gameExeHandle.readAll(),
                                                    QCryptographicHash::Algorithm::Md5);
        if (gameExeHash == QByteArray::fromHex("20e673906133201fded29b3af6d7b6cd"))
            curGame = VersionedGame::KF2Jv1_0;
        else if (gameExeHash == QByteArray::fromHex("5522e7cdeb6c0261befd6eedf95065a3"))
            curGame = VersionedGame::KF2Jv1_7;
    }
    else if (srcDir.exists("SLPS_910.03"))
    {
        auto copyTxtHandle = QFile(srcDir.absoluteFilePath("COPY.TXT"));
        if (!copyTxtHandle.open(QIODevice::ReadOnly)) return;

        const auto copyTxt = copyTxtHandle.readAll();

        if (copyTxt.contains(QByteArrayLiteral("1.8A")))
            curGame = VersionedGame::KF2Jv1_8A;
        else if (copyTxt.contains(QByteArrayLiteral("1.8B")))
            curGame = VersionedGame::KF2Jv1_8B;
        else
            KFMTError::error(
                QStringLiteral("Your copy of KF2 could not be identified. Please open an issue on "
                               "our GitHub repo or contact us on Discord!"));
    }
    else if (srcDir.exists("SLUS_001.58"))
        curGame = VersionedGame::KF2U;
    else if (srcDir.exists("SLPS_003.77"))
    {
        auto copyTxtHandle = QFile(srcDir.absoluteFilePath("COPY.TXT"));
        if (!copyTxtHandle.open(QIODevice::ReadOnly)) return;

        const auto copyTxt = copyTxtHandle.readAll();

        if (copyTxt.contains(QByteArrayLiteral("1.3")))
            curGame = VersionedGame::KF3Jv1_3;
        else if (copyTxt.contains(QByteArrayLiteral("1.4")))
            curGame = VersionedGame::KF3Jv1_4;
        else
            KFMTError::error(
                QStringLiteral("Your copy of KF3 could not be identified. Please open an issue on "
                               "our GitHub repo or contact us on Discord!"));
    }
    else if (srcDir.exists("SLUS_002.55"))
        curGame = VersionedGame::KF3U;
    else if (srcDir.exists("SLPM_800.29"))
        curGame = VersionedGame::KFPS;
    else if (srcDir.exists("END.EXE") && srcDir.exists("ST.EXE") && srcDir.exists("OPTXT.EXE"))
        curGame = VersionedGame::ST;
    else if (srcDir.exists("ST.EXE") && srcDir.exists("TITLE.MIC") && srcDir.exists("TITLE.XA"))
        curGame = VersionedGame::STDemo;
    else
        curGame = VersionedGame::None;
}
