#include "./qrpc_last_error.h"
#include <QVariant>

namespace QRpc {

static const char * __nativeErrorCode="nativeErrorCode";
static const char * __nativeErrorText="nativeErrorText";

LastError::LastError(QObject *parent) : QObject(parent)
{

}

LastError::~LastError()
{

}

QString LastError::nativeErrorCode() const
{
    return this->property(__nativeErrorCode).toString().trimmed();
}

QString LastError::text() const
{
    return this->property(__nativeErrorText).toString().trimmed();
}

bool LastError::isValid() const
{
    return !nativeErrorCode().trimmed().isEmpty() || !text().isEmpty();
}

void LastError::clear()
{
    this->setProperty(__nativeErrorCode, QVariant());
    this->setProperty(__nativeErrorText, QVariant());
}

} // namespace QRpc
