#include "kfmtcore.h"
#include "kfmterror.h"
#include "kfmtfile.h"
#include <QCryptographicHash>
#include <QTextStream>

// Declaring the global
KFMTCore core;
// Declaring the unknown string
const QString unknownString = QStringLiteral(u"???");

KFMTCore::SimpleGame KFMTCore::currentGame() const
{
    switch (m_curGame)
    {
        case VersionedGame::None: return SimpleGame::None;
        case VersionedGame::ACU: [[fallthrough]];
        case VersionedGame::ACJDemoD2: [[fallthrough]];
        case VersionedGame::ACJDemoHR7: [[fallthrough]];
        case VersionedGame::ACUDemoISV4: [[fallthrough]];
        case VersionedGame::ACUDemoPUG3: [[fallthrough]];
        case VersionedGame::ACJDemoT977: [[fallthrough]];
        case VersionedGame::ACUProto: return SimpleGame::AC;
        case VersionedGame::KF: [[fallthrough]];
        case VersionedGame::KFJDemo: return SimpleGame::KF1;
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
        case VersionedGame::STJv1_4: [[fallthrough]];
        case VersionedGame::STJv2_2: [[fallthrough]];
        case VersionedGame::STJDemo: [[fallthrough]];
        case VersionedGame::STU: [[fallthrough]];
        case VersionedGame::STUDemo: return SimpleGame::ST;
        default: KFMTError::fatalError(QStringLiteral("KFMTCore::currentGame: Unhandled game!"));
    }
}

void KFMTCore::loadFrom(const QDir& srcDir)
{
    detectGame(srcDir);
    files.resetRoot();
    if (m_curGame == VersionedGame::None) return;

    std::vector<QString> fileLists;

    m_curSourceDirectory = srcDir;
    switch (m_curGame)
    {
        case VersionedGame::KF: fileLists.emplace_back(QStringLiteral("kf1.csv")); break;
        case VersionedGame::KF2Jv1_0: [[fallthrough]];
        case VersionedGame::KF2Jv1_7: [[fallthrough]];
        case VersionedGame::KF2Jv1_8A: [[fallthrough]];
        case VersionedGame::KF2Jv1_8B:
            fileLists.emplace_back(QStringLiteral("kf2_cd.csv"));
            fileLists.emplace_back(QStringLiteral("kf2_op_j.csv"));
            fileLists.emplace_back(QStringLiteral("kf2_gameexe.csv"));
            break;
        case VersionedGame::KF2U:
            fileLists.emplace_back(QStringLiteral("kf2_cd.csv"));
            fileLists.emplace_back(QStringLiteral("kf2_op_u.csv"));
            fileLists.emplace_back(QStringLiteral("kf2_gameexe.csv"));
            break;
        default:
            KFMTError::error(QStringLiteral("KFMTCore::loadFrom: Unhandled Game value %1")
                                 .arg(static_cast<int>(m_curGame)));
            break;
    }

    if (m_curGame != VersionedGame::KF2E && m_curGame != VersionedGame::KF2Jv1_0
        && m_curGame != VersionedGame::KF2Jv1_8A && m_curGame != VersionedGame::KF2Jv1_8B
        && m_curGame != VersionedGame::KF2U)
        KFMTError::warning(QStringLiteral("Support for the game you loaded is INCOMPLETE. Things "
                                          "will be broken, and names will be incorrect."));
    
    for (const auto& fileList : fileLists)
        loadFileList(fileList);
}

KFMTFile::DataType KFMTCore::parseDataType(const QString& dataTypeStr)
{
    // Generic
    if (dataTypeStr == QStringLiteral(u"Container")) return KFMTFile::DataType::Container;
    if (dataTypeStr == QStringLiteral(u"GameDB")) return KFMTFile::DataType::GameDB;
    if (dataTypeStr == QStringLiteral(u"MapTile")) return KFMTFile::DataType::MapTile;
    if (dataTypeStr == QStringLiteral(u"MapTilemap")) return KFMTFile::DataType::MapTilemap;
    if (dataTypeStr == QStringLiteral(u"MapDB")) return KFMTFile::DataType::MapDB;
    if (dataTypeStr == QStringLiteral(u"MapScript")) return KFMTFile::DataType::MapScript;
    if (dataTypeStr == QStringLiteral(u"Model")) return KFMTFile::DataType::Model;
    if (dataTypeStr == QStringLiteral(u"MusicSequence")) return KFMTFile::DataType::MusicSequence;
    if (dataTypeStr == QStringLiteral(u"SoundBankBody")) return KFMTFile::DataType::SoundBankBody;
    if (dataTypeStr == QStringLiteral(u"SoundBankHeader"))
        return KFMTFile::DataType::SoundBankHeader;
    if (dataTypeStr == QStringLiteral(u"TextureDB")) return KFMTFile::DataType::TextureDB;
    if (dataTypeStr == QStringLiteral(u"Unknown")) return KFMTFile::DataType::Unknown;

    // KF1 specific
    if (dataTypeStr == QStringLiteral(u"KF1_ArmourParams"))
        return KFMTFile::DataType::KF1_ArmourParams;
    if (dataTypeStr == QStringLiteral("KF1_LevelCurve")) return KFMTFile::DataType::KF1_LevelCurve;
    if (dataTypeStr == QStringLiteral(u"KF1_MagicParams"))
        return KFMTFile::DataType::KF1_MagicParams;
    if (dataTypeStr == QStringLiteral(u"KF1_WeaponParams"))
        return KFMTFile::DataType::KF1_WeaponParams;

    // KF2 specific
    if (dataTypeStr == QStringLiteral(u"KF2_ArmourParams"))
        return KFMTFile::DataType::KF2_ArmourParams;
    if (dataTypeStr == QStringLiteral(u"KF2_GameExec")) return KFMTFile::DataType::KF2_GameExec;
    if (dataTypeStr == QStringLiteral(u"KF2_LevelCurve")) return KFMTFile::DataType::KF2_LevelCurve;
    if (dataTypeStr == QStringLiteral(u"KF2_MagicParams"))
        return KFMTFile::DataType::KF2_MagicParams;
    if (dataTypeStr == QStringLiteral(u"KF2_ModelPack_MO")) return KFMTFile::DataType::KF2_ModelPack_MO;
    if (dataTypeStr == QStringLiteral(u"KF2_ModelPack_TMD")) return KFMTFile::DataType::KF2_ModelPack_TMD;
    if (dataTypeStr == QStringLiteral(u"KF2_ObjectClasses"))
        return KFMTFile::DataType::KF2_ObjectClasses;
    if (dataTypeStr == QStringLiteral(u"KF2_SoundEffectParams"))
        return KFMTFile::DataType::KF2_SoundEffectParams;
    if (dataTypeStr == QStringLiteral(u"KF2_TileRenderParams"))
        return KFMTFile::DataType::KF2_TileRenderParams;
    if (dataTypeStr == QStringLiteral(u"KF2_WeaponParams"))
        return KFMTFile::DataType::KF2_WeaponParams;

    fsmt_assert(false, "KFMTCore::parseDataType: File had unknown data type.");
}

KFMTFile::FileFormat KFMTCore::parseFileFormat(const QString& fileTypeStr)
{
    if (fileTypeStr == QStringLiteral("Folder")) return KFMTFile::FileFormat::Folder;
    else if (fileTypeStr == QStringLiteral("MIX")) return KFMTFile::FileFormat::MIX;
    else if (fileTypeStr == QStringLiteral("Raw")) return KFMTFile::FileFormat::Raw;
    else if (fileTypeStr == QStringLiteral("T")) return KFMTFile::FileFormat::T;

    fsmt_assert(false, "KFMTCore::parseFileFormat: File had unknown file format.");
}

void KFMTCore::loadFileList(const QString& fileList)
{
    QFile listFile(":/filelists/" + fileList);
    listFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream listStream(&listFile);
    QStringList line;

    line = listStream.readLine().split(',');
    fsmt_assert(line[0] == "FSMTFileList", "KFMTCore::loadFileList: Invalid version header!");
    fsmt_assert(line[1] == "v", "KFMTCore::loadFileList: Invalid version header!");
    fsmt_assert(line[2] == "0", "KFMTCore::loadFileList: Invalid version header!");
    fsmt_assert(line[3] == "1", "KFMTCore::loadFileList: Invalid version header!");

    line = listStream.readLine().split(',');
    fsmt_assert(line[0] == "FILES", "KFMTCore::loadFileList: Files section not found in file list!");

    // Skip FILES section header
    line = listStream.readLine().split(',');

    while (!listStream.atEnd())
    {
        line = listStream.readLine().split(',');
        auto path = line[0].split('/');
        fsmt_assert(path.size() > 0, "KFMTCore::loadFileList: Empty file path found!");

        KFMTFile* parent = &files;
        // If we're not in the root, dive to find the parent KFMTFile's child vector.
        if (path.size() > 1)
        {
            for (auto i = 0; i < path.size() - 1; i++)
            {
                parent = (*parent)[path[i]];
                fsmt_assert(parent != nullptr, "KFMTCore::loadFileList: Couldn't find file parent!");
            }
        }

        // Special case for folders
        if (parseFileFormat(line[1]) == KFMTFile::FileFormat::Folder)
        {
            parent->createChild(path.back(),
                                QByteArray(),
                                KFMTFile::FileFormat::Folder,
                                KFMTFile::DataType::Container);
            continue;
        }

        // Check if there is a "sibling" with this name...
        KFMTFile* sameName = nullptr;
        // As long as we're not in the root, that is!
        if (path.size() > 1) sameName = (*parent)[path.last()];
        // If there is one, just apply the properties in this line to it.
        if (sameName != nullptr)
        {
            sameName->setFileFormat(parseFileFormat(line[1]));
            sameName->setDataType(parseDataType(line[2]));
            sameName->setPrettyName(line[3]);
            //KFMTError::log(QStringLiteral("Loaded %1.").arg(line[0]));
            continue;
        }

        // If there is no child with this name, create one.
        QFile file(m_curSourceDirectory.filePath(line[0]));
        file.open(QIODevice::ReadOnly);
        parent->createChild(path.back(),
                            file.readAll(),
                            parseFileFormat(line[1]),
                            parseDataType(line[2]),
                            line[3]);
        file.close();
        
        //KFMTError::log(QStringLiteral("Loaded %1.").arg(line[0]));
    }
}

void KFMTCore::saveTo(const QDir& outDir)
{
    // FIXME: REIMPLEMENT THIS URGENTLYYY!!!
    KFMTError::error(QStringLiteral(u"Saving has not been reimplemented yet."));
}

void KFMTCore::detectGame(const QDir& srcDir)
{
    if (srcDir.exists("SCUS_941.82") || srcDir.exists("SLUS_013.23"))
        m_curGame = VersionedGame::ACU;
    else if (srcDir.exists("STR") && srcDir.exists("AC.EXE")) m_curGame = VersionedGame::ACJDemoD2;
    else if (srcDir.exists("STR") && srcDir.exists("DEMO1")) m_curGame = VersionedGame::ACJDemoHR7;
    else if (srcDir.exists("USA10.EXE"))
    {
        if (srcDir.exists(QStringLiteral("COM%1DEMO00.XA").arg(QDir::separator())))
            m_curGame = VersionedGame::ACUDemoISV4;
        else if (srcDir.exists(QStringLiteral("COM%1DEMO01.XA").arg(QDir::separator())))
            m_curGame = VersionedGame::ACUDemoPUG3;
    }
    else if (!srcDir.exists("STR") && srcDir.exists("AC.EXE"))
        m_curGame = VersionedGame::ACJDemoT977;
    else if (srcDir.exists("SLPS_009.00")) m_curGame = VersionedGame::ACUProto;
    else if (srcDir.exists("PSX.EXE") && srcDir.exists("E0") && srcDir.exists("E1")
             && srcDir.exists("E2") && srcDir.exists("E3"))
        m_curGame = VersionedGame::KF;
    else if (srcDir.exists("KFIELD.EXE") && srcDir.exists("MAP.001"))
        m_curGame = VersionedGame::KFJDemo;
    else if (srcDir.exists("SCES_005.10") && srcDir.exists("END.EXE")
             && srcDir.exists("LICENSEE.DAT"))
        m_curGame = VersionedGame::KF2E;
    else if (srcDir.exists("END.EXE") && srcDir.exists("GAME.EXE") && srcDir.exists("OPEN.EXE")
             && srcDir.exists("PSX.EXE"))
    {
        auto gameExeHandle = QFile(srcDir.absoluteFilePath("GAME.EXE"));
        if (!gameExeHandle.open(QIODevice::ReadOnly)) return;

        auto gameExeHash = QCryptographicHash::hash(gameExeHandle.readAll(),
                                                    QCryptographicHash::Algorithm::Md5);
        if (gameExeHash == QByteArray::fromHex("20e673906133201fded29b3af6d7b6cd"))
            m_curGame = VersionedGame::KF2Jv1_0;
        else if (gameExeHash == QByteArray::fromHex("5522e7cdeb6c0261befd6eedf95065a3"))
            m_curGame = VersionedGame::KF2Jv1_7;
    }
    else if (srcDir.exists("SLPS_910.03"))
    {
        auto copyTxtHandle = QFile(srcDir.absoluteFilePath("COPY.TXT"));
        if (!copyTxtHandle.open(QIODevice::ReadOnly)) return;

        const auto copyTxt = copyTxtHandle.readAll();

        if (copyTxt.contains(QByteArrayLiteral("1.8A"))) m_curGame = VersionedGame::KF2Jv1_8A;
        else if (copyTxt.contains(QByteArrayLiteral("1.8B"))) m_curGame = VersionedGame::KF2Jv1_8B;
        else
            KFMTError::error(
                QStringLiteral("Your copy of KF2 could not be identified. Please open an issue on "
                               "our GitHub repo or contact us on Discord!"));
    }
    else if (srcDir.exists("SLUS_001.58")) m_curGame = VersionedGame::KF2U;
    else if (srcDir.exists("SLPS_003.77"))
    {
        auto copyTxtHandle = QFile(srcDir.absoluteFilePath("COPY.TXT"));
        if (!copyTxtHandle.open(QIODevice::ReadOnly)) return;

        const auto copyTxt = copyTxtHandle.readAll();

        if (copyTxt.contains(QByteArrayLiteral("1.3"))) m_curGame = VersionedGame::KF3Jv1_3;
        else if (copyTxt.contains(QByteArrayLiteral("1.4"))) m_curGame = VersionedGame::KF3Jv1_4;
        else
            KFMTError::error(
                QStringLiteral("Your copy of KF3 could not be identified. Please open an issue on "
                               "our GitHub repo or contact us on Discord!"));
    }
    else if (srcDir.exists("SLUS_002.55")) m_curGame = VersionedGame::KF3U;
    else if (srcDir.exists("SLPM_800.29")) m_curGame = VersionedGame::KFPS;
    else if (srcDir.exists("SLPS_014.20"))
    {
        auto copyTxtHandle = QFile(srcDir.absoluteFilePath("COPY.TXT"));
        if (!copyTxtHandle.open(QIODevice::ReadOnly)) return;

        const auto copyTxt = copyTxtHandle.readAll();

        if (copyTxt.contains(QByteArrayLiteral("1.4"))) m_curGame = VersionedGame::STJv1_4;
        else if (copyTxt.contains(QByteArrayLiteral("2.2"))) m_curGame = VersionedGame::STJv2_2;
        else
            KFMTError::error(
                QStringLiteral("Your copy of Shadow Tower could not be identified. Please open an "
                               "issue on our GitHub repo or contact us on Discord!"));
    }
    else if (srcDir.exists("SLUS_008.63")) m_curGame = VersionedGame::STU;
    else if (srcDir.exists("ST.EXE") && srcDir.exists("TITLE.XA"))
    {
        if (srcDir.exists("PSX.EXE")) m_curGame = VersionedGame::STUDemo;
        else m_curGame = VersionedGame::STJDemo;
    }
    else m_curGame = VersionedGame::None;
}
