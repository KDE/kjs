// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003 Apple Computer, Inc
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _KJS_COMPLETION_H_
#define _KJS_COMPLETION_H_

#include "CommonIdentifiers.h"

namespace KJS {

  typedef unsigned Addr; // ### should there be some separare types h?
  
  class Node;
  class JSValue;

  /**
   * Completion types.
   */
  enum ComplType { Normal, Break, Continue, ReturnValue, Throw, Interrupted };

  /**
   * Completion objects are used to convey the return status and value
   * from functions.
   *
   * See FunctionImp::execute()
   *
   * @see FunctionImp
   *
   * @short Handle for a Completion type.
   */
  class KJS_EXPORT Completion {
  public:
    explicit Completion(ComplType c = Normal, JSValue *v = NULL, Addr t = 0 )
        : comp(c), val(v), tar(t) { }

    /**
     * Returns the type of this completion.
     */
    ComplType complType() const { return comp; }

    /**
     * Returns the value of this completion if it is of type
     * value-completion, 0 otherwise.
     */
    JSValue *value() const { return val; }

     /**
     * Returns the address a break or a continue statement targets
     */
    Addr target() const { return tar; }

    /**
     * Returns true if this is a value completion, false otherwise.
     */
    bool isValueCompletion() const { return !!val; }
  private:
    ComplType comp;
    JSValue *val;
    Addr tar;
  };

}

#endif
