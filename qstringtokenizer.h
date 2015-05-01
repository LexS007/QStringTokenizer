/*
 * Copyright (c) 1994, 2004, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#ifndef QSTRINGTOKENIZER_H
#define QSTRINGTOKENIZER_H
#include <QString>
#include <QObject>

/**
 * @brief The QStringTokenizer class
 */
class QStringTokenizer
{
public:
    QStringTokenizer(QString str, QString delim, bool returnDelims);
    QStringTokenizer(QString str, QString delim);
    QStringTokenizer(QString str);
    ~QStringTokenizer();

    bool hasMoreTokens();
    QString nextToken();
    QString nextToken(QString delim);
    bool hasMoreElements();
    QString nextElement();
    int countTokens();


private:
    int currentPosition;
    int newPosition;
    int maxPosition;
    QString str;
    QString delimiters;
    bool retDelims;
    bool delimsChanged;

    /**
    * maxDelimCodePoint stores the value of the delimiter character with the
    * highest value. It is used to optimize the detection of delimiter
    * characters.
    *
    * It is unlikely to provide any optimization benefit in the
    * hasSurrogates case because most string characters will be
    * smaller than the limit, but we keep it so that the two code
    * paths remain similar.
    */
    int maxDelimCodePoint;
    /**
    * If delimiters include any surrogates (including surrogate
    * pairs), hasSurrogates is true and the tokenizer uses the
    * different code path. This is because String.indexOf(int)
    * doesn't handle unpaired surrogates as a single character.
    */
    bool hasSurrogates;
    /**
    * When hasSurrogates is true, delimiters are converted to code
    * points and isDelimiter(int) is used to determine if the given
    * codepoint is a delimiter.
    */
    int * delimiterCodePoints[];
    int count;

    void setMaxDelimCodePoint();
    int skipDelimiters(int startPos);
    int scanToken(int startPos);
    bool isDelimiter(int codePoint);

    ushort MIN_HIGH_SURROGATE;
    ushort MAX_LOW_SURROGATE;
};

#endif // QSTRINGTOKENIZER_H
