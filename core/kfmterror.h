#ifndef KFMTERROR_H
#define KFMTERROR_H

#include <queue>
#include <QWidget>

#define fsmt_assert(expr, reason) \
    if (!(expr)) KFMTError::failAssert(QStringLiteral(#expr), QStringLiteral(__FILE__), __LINE__, QStringLiteral(reason))

namespace KFMTError
{

/*!
 * \brief Displays a non-fatal error message.
 * \param errorMessage Error message to display.
 */
void error(const QString& errorMessage);

[[noreturn]] void failAssert(const QString& assertExpr, const QString& fileName, uint16_t line, const QString& reason);

/*!
 * \brief Displays a fatal error message.
 * \param errorMessage Error message to display.
 */
[[noreturn]] void fatalError(const QString& fatalErrorMessage);

/*!
 * \brief Writes a quiet log message to stderr.
 * \param logMessage Log message to write.
 */
void log(const QString& logMessage);

/*!
 * \brief Sets the parent for the error messages. Should only be called once in the main window.
 * \param parentPtr Pointer to the parent. (QWidget)
 */
void setParent(QWidget* parentPtr);

/*!
 * \brief Displays a non-fatal warning message.
 * \param warningMessage Warning message to display.
 */
void warning(const QString& warningMessage);

/*!
 * \brief Displays a fatal out of range message.
 * \param index Index that caused the out of range exception.
 * \param arrayName Name of the array (e.g. "object class declaration").
 */
[[noreturn]] inline void outOfRange(const size_t& index, const QString& arrayName)
{
    fatalError(
        QStringLiteral("Tried to access index %1 of the %2 array.\n").arg(index).arg(arrayName));
}

}; // namespace KFMTError

#endif // KFMTERROR_H
