#include "core/kfmterror.h"
#include <QMessageBox>
#include <iostream>

static QWidget* KFMTErrorParent;
static std::queue<QString> lastErrors{};

static const auto errorStr = QStringLiteral("Error: ");
static const auto failAssertStr = QStringLiteral(
    "ASSERTION FAILED!\nThe assertion '%1' from file '%2' at line %3 has failed.\n\nReason: %4\n\nPlease report "
    "this issue on the FSModTool GitHub issues page with some context on what you were doing "
    "before this happened.\nFSModTool will now shut down.");
static const auto fatalErrorStr = QStringLiteral("Fatal Error: ");
static const auto warningStr = QStringLiteral("Warning: ");

void KFMTError::error(const QString & errorMessage)
{
    log(errorStr + errorMessage);

    if (!lastErrors.empty()
        && (errorMessage == lastErrors.front() || errorMessage == lastErrors.back()))
        return;

    lastErrors.push(errorMessage);
    lastErrors.pop();

    QMessageBox::critical(KFMTErrorParent, QStringLiteral("Error"), errorMessage);
}

void KFMTError::failAssert(const QString& assertExpr, const QString& fileName, uint16_t line, const QString& reason)
{
    KFMTError::fatalError(failAssertStr.arg(assertExpr).arg(fileName).arg(line).arg(reason));
}

void KFMTError::fatalError(const QString & fatalErrorMessage)
{
    log(fatalErrorStr + fatalErrorMessage);
    QMessageBox::critical(KFMTErrorParent, QStringLiteral("Fatal Error"), fatalErrorMessage);
    throw;
}

void KFMTError::log(const QString & logMessage)
{
    std::cerr << logMessage.toStdString() << '\n';
}

void KFMTError::setParent(QWidget* parentPtr)
{
    KFMTErrorParent = parentPtr;
}

void KFMTError::warning(const QString & warningMessage)
{
    log(warningStr + warningMessage);
    QMessageBox::warning(KFMTErrorParent, QStringLiteral("Warning"), warningMessage);
}
