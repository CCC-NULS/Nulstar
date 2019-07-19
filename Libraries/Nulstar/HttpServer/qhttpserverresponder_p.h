/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtHttpServer module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QHTTPSERVERRESPONDER_P_H
#define QHTTPSERVERRESPONDER_P_H

#include <qthttpserverglobal.h>
#include <qhttpserverrequest.h>
#include <qhttpserverresponder.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qpair.h>
#include <QtCore/qpointer.h>
#include <QtCore/qsysinfo.h>

#include <type_traits>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

QT_BEGIN_NAMESPACE

class QHttpServerResponderPrivate
{
    using StatusCode = QHttpServerResponder::StatusCode;

public:
    QHttpServerResponderPrivate(const QHttpServerRequest &request, QTcpSocket *const socket) :
        request(request),
        socket(socket)
    {
        const auto server = QStringLiteral("%1/%2(%3)").arg(
                    QCoreApplication::instance()->applicationName(),
                    QCoreApplication::instance()->applicationVersion(),
                    QSysInfo::prettyProductName());
        addHeader(QByteArrayLiteral("Server"), server.toUtf8());
    }

    inline bool addHeader(const QByteArray &key, const QByteArray &value)
    {
        const auto hash = qHash(key.toLower());
        if (m_headers.contains(hash))
            return false;
        m_headers.insert(hash, qMakePair(key, value));
        return true;
    }

    void writeStatusLine(StatusCode status = StatusCode::Ok,
                         const QPair<quint8, quint8> &version = qMakePair(1u, 1u)) const;
    void writeHeaders() const;
    void writeBody(const QByteArray &body) const;

    const QHttpServerRequest &request;
#if defined(QT_DEBUG)
    const QPointer<QTcpSocket> socket;
#else
    QTcpSocket *const socket;
#endif

    QMap<uint, QPair<QByteArray, QByteArray>> m_headers;

private:
    void writeHeader(const QByteArray &header, const QByteArray &value) const;

public:
    const decltype(m_headers) &headers() const { return m_headers; }
};

QT_END_NAMESPACE

#endif // QHTTPSERVERRESPONDER_P_H
