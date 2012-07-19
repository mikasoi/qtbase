/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qguiapplication_p.h>
#include "qkmsnativeinterface.h"
#include "qkmsdevice.h"

#include "qscreen.h"
#include "qkmscontext.h"
#include <QOpenGLContext>

class QKmsResourceMap : public QMap<QByteArray, QKmsNativeInterface::ResourceType>
{
public:
    QKmsResourceMap()
        :QMap<QByteArray, QKmsNativeInterface::ResourceType>()
    {
        insert("egldisplay", QKmsNativeInterface::EglDisplay);
        insert("eglcontext", QKmsNativeInterface::EglContext);
    }
};

Q_GLOBAL_STATIC(QKmsResourceMap, qKmsResourceMap)

void *QKmsNativeInterface::nativeResourceForWindow(const QByteArray &resourceString, QWindow *window)
{
    QByteArray lowerCaseResource = resourceString.toLower();
    ResourceType resource = qKmsResourceMap()->value(lowerCaseResource);
    void *result = 0;
    switch (resource) {
    case EglDisplay:
        result = eglDisplayForWindow(window);
        break;
    case EglContext:
        result = eglContextForWindow(window);
        break;
    default:
        result = 0;
    }
    return result;
}

QPlatformNativeInterface::NativeResourceForContextFunction QKmsNativeInterface::nativeResourceFunctionForContext(const QByteArray &resource)
{
    QByteArray lowerCaseResource = resource.toLower();
    if (lowerCaseResource == "get_egl_context") {
        return eglContextForContext;
    }
    return 0;
}



void *QKmsNativeInterface::eglDisplayForWindow(QWindow *window)
{
    QKmsScreen *screen = qPlatformScreenForWindow(window);
    if (!screen)
        return 0;
    QKmsDevice *device = screen->device();
    if (!device)
        return 0;
    return device->eglDisplay();
}

void *QKmsNativeInterface::eglContextForWindow(QWindow *)
{
    return 0;
}

QKmsScreen *QKmsNativeInterface::qPlatformScreenForWindow(QWindow *window)
{
    QScreen *screen = window ? window->screen() : QGuiApplication::primaryScreen();
    return static_cast<QKmsScreen *>(screen->handle());
}

void *QKmsNativeInterface::eglContextForContext(QOpenGLContext *context)
{
    Q_ASSERT(context);

    QKmsContext *eglPlatformContext = static_cast<QKmsContext *>(context->handle());

    return eglPlatformContext->eglContext();
}
