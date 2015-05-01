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
#include "qstringtokenizer.h"
#include <QChar>

/**
* Constructs a string tokenizer for the specified string. All
* characters in the <code>delim</code> argument are the delimiters
* for separating tokens.
* <p>
* If the <code>returnDelims</code> flag is <code>true</code>, then
* the delimiter characters are also returned as tokens. Each
* delimiter is returned as a string of length one. If the flag is
* <code>false</code>, the delimiter characters are skipped and only
* serve as separators between tokens.
* <p>
*
* @param   str            a string to be parsed.
* @param   delim          the delimiters.
* @param   returnDelims   flag indicating whether to return the delimiters
*                         as tokens.
*/
QStringTokenizer::QStringTokenizer(QString str, QString delim, bool returnDelims)
{
    currentPosition = 0;
    newPosition = -1;
    delimsChanged = false;
    this->str = str;
    maxPosition = str.length();
    delimiters = delim;
    hasSurrogates = false;
    retDelims = returnDelims;
    setMaxDelimCodePoint();

    MIN_HIGH_SURROGATE = 0xDBFF;
    MAX_LOW_SURROGATE  = 0xDFFF;
}

/**
* Constructs a string tokenizer for the specified string. The
* characters in the <code>delim</code> argument are the delimiters
* for separating tokens. Delimiter characters themselves will not
* be treated as tokens.
* <p>
*
* @param   str     a string to be parsed.
* @param   delim   the delimiters.
*/
QStringTokenizer::QStringTokenizer(QString str, QString delim)
{
    QStringTokenizer(str, delim, false);
}

/**
* Constructs a string tokenizer for the specified string. The
* tokenizer uses the default delimiter set, which is
* <code>"&nbsp;&#92;t&#92;n&#92;r&#92;f"</code>: the space character,
* the tab character, the newline character, the carriage-return character,
* and the form-feed character. Delimiter characters themselves will
* not be treated as tokens.
*
* @param   str   a string to be parsed.
*/
QStringTokenizer::QStringTokenizer(QString str)
{
    QStringTokenizer(str, " \t\n\r\f", false);
}

QStringTokenizer::~QStringTokenizer()
{

}

/**
* Set maxDelimCodePoint to the highest char in the delimiter set.
*/
void QStringTokenizer::setMaxDelimCodePoint()
{
    if (delimiters == NULL) {
        maxDelimCodePoint = 0;
        return;
    }
    count = 0;
    int m = 0;
    int c;
    for (int i = 0; i < delimiters.length(); i += QChar::requiresSurrogates(c) ? 2 : 1) {
        c = delimiters.at(i).unicode();
        if (c >= MIN_HIGH_SURROGATE && c <= MAX_LOW_SURROGATE) {
            c = delimiters.at(i).unicode();
            hasSurrogates = true;
        }
        if (m < c)
            m = c;
        count++;
    }
    maxDelimCodePoint = m;

    if (hasSurrogates) {
        *delimiterCodePoints = new int[count];
        for (int i = 0, j = 0; i < count; i++, j += QChar::requiresSurrogates(c) ? 2 : 1) {
            c = delimiters.at(j).unicode();
            *delimiterCodePoints[i] = c;
        }
    }
}

/**
* Skips delimiters starting from the specified position. If retDelims
* is false, returns the index of the first non-delimiter character at or
* after startPos. If retDelims is true, startPos is returned.
*/
int QStringTokenizer::skipDelimiters(int startPos)
{
    if (delimiters == NULL)
        throw "delimiters is NULL!";

    int position = startPos;
    while (!retDelims && position < maxPosition) {
        if (!hasSurrogates) {
            QChar c = str.at(position);
            if ((c > maxDelimCodePoint) || (delimiters.indexOf(c) < 0))
                break;
            position++;
        } else {
            int c = str.at(position).unicode();
            if ((c > maxDelimCodePoint) || !isDelimiter(c)) {
                break;
            }
            position += QChar::requiresSurrogates(c) ? 2 : 1;
        }
    }
    return position;
}

/**
* Skips ahead from startPos and returns the index of the next delimiter
* character encountered, or maxPosition if no such delimiter is found.
*/
int QStringTokenizer::scanToken(int startPos)
{
    int position = startPos;
    while (position < maxPosition) {
        if (!hasSurrogates) {
            QChar c = str.at(position);
            if ((c <= maxDelimCodePoint) && (delimiters.indexOf(c) >= 0))
                break;
            position++;
        } else {
            int c = str.at(position).unicode();
            if ((c <= maxDelimCodePoint) && isDelimiter(c))
                break;
            position += QChar::requiresSurrogates(c) ? 2 : 1;
        }
    }
    if (retDelims && (startPos == position)) {
        if (!hasSurrogates) {
            QChar c = str.at(position);
            if ((c <= maxDelimCodePoint) && (delimiters.indexOf(c) >= 0))
                position++;
        } else {
            int c = str.at(position).unicode();
            if ((c <= maxDelimCodePoint) && isDelimiter(c))
                position += QChar::requiresSurrogates(c) ? 2 : 1;
        }
    }
    return position;
}

bool QStringTokenizer::isDelimiter(int codePoint)
{
    for (int i = 0; i<count; i++) {
        if (*delimiterCodePoints[i] == codePoint) {
            return true;
        }
    }
    return false;
}

/**
* Tests if there are more tokens available from this tokenizer's string.
* If this method returns <tt>true</tt>, then a subsequent call to
* <tt>nextToken</tt> with no argument will successfully return a token.
*
* @return  <code>true</code> if and only if there is at least one token
*          in the string after the current position; <code>false</code>
*          otherwise.
*/
bool QStringTokenizer::hasMoreTokens()
{
    newPosition = skipDelimiters(currentPosition);
    return (newPosition < maxPosition);
}

/**
* Returns the next token from this string tokenizer.
*
* @return     the next token from this string tokenizer.
*/
QString QStringTokenizer::nextToken()
{
    currentPosition = (newPosition >= 0 && !delimsChanged) ?
                newPosition : skipDelimiters(currentPosition);

    /* Reset these anyway */
    delimsChanged = false;
    newPosition = -1;

    if (currentPosition >= maxPosition)
        throw "No Such Element!";
    int start = currentPosition;
    currentPosition = scanToken(currentPosition);
    return str.mid(start, currentPosition-start);
}

/**
* Returns the next token in this string tokenizer's string. First,
* the set of characters considered to be delimiters by this
* <tt>StringTokenizer</tt> object is changed to be the characters in
* the string <tt>delim</tt>. Then the next token in the string
* after the current position is returned. The current position is
* advanced beyond the recognized token.  The new delimiter set
* remains the default after this call.
*
* @param      delim   the new delimiters.
* @return     the next token, after switching to the new delimiter set.
*/
QString QStringTokenizer::nextToken(QString delim)
{
    delimiters = delim;

    /* delimiter string specified, so set the appropriate flag. */
    delimsChanged = true;

    setMaxDelimCodePoint();
    return nextToken();
}

/**
* Returns the same value as the <code>hasMoreTokens</code>
* method. It exists so that this class can implement the
* <code>Enumeration</code> interface.
*
* @return  <code>true</code> if there are more tokens;
*          <code>false</code> otherwise.
*/
bool QStringTokenizer::hasMoreElements()
{
    return hasMoreTokens();
}

/**
* Returns the same value as the <code>nextToken</code> method,
* except that its declared return value is <code>Object</code> rather than
* <code>String</code>. It exists so that this class can implement the
* <code>Enumeration</code> interface.
*
* @return     the next token in the string.
*/
QString QStringTokenizer::nextElement()
{
    return nextToken();
}

/**
* Calculates the number of times that this tokenizer's
* <code>nextToken</code> method can be called before it generates an
* exception. The current position is not advanced.
*
* @return  the number of tokens remaining in the string using the current
*          delimiter set.
*/
int QStringTokenizer::countTokens()
{
    int count = 0;
    int currpos = currentPosition;
    while (currpos < maxPosition) {
        currpos = skipDelimiters(currpos);
        if (currpos >= maxPosition)
            break;
        currpos = scanToken(currpos);
        count++;
    }
    return count;
}
