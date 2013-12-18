/*
 *  This file is part of the KDE libraries
 *
 *  Copyright (C) 2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */

#ifndef ECMATEST_H
#define ECMATEST_H

#include <QtCore/QObject>

class ECMAscriptTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void runAllTests();
    void runAllTests_data();
    void cleanup();
    void cleanupTestCase();
private:
    unsigned int m_passed;
    unsigned int m_failed;
};

#endif /* ECMATEST_H */
