/*=========================================================================

  Program:   ITK-SNAP
  Module:    $RCSfile: AnnotationInteractionMode.h,v $
  Language:  C++
  Date:      $Date: 2009/01/23 21:22:02 $
  Version:   $Revision: 1.1 $
  Copyright (c) 2007 Paul A. Yushkevich
  
  This file is part of ITK-SNAP 

  ITK-SNAP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details. 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  -----

  Copyright (c) 2003 Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  -----

  Copyright (c) 2003 Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information. 

=========================================================================*/
#ifndef __AnnotationInteractionMode_h_
#define __AnnotationInteractionMode_h_

#include "GenericSliceWindow.h"
#include <list>
#include <utility>

/**
 * \class AnnotationInteractionMode
 * \brief UI interaction mode that takes care of polygon drawing and editing.
 *
 * \see GenericSliceWindow
 */
class AnnotationInteractionMode : public GenericSliceWindow::EventHandler 
{
public:
  AnnotationInteractionMode(GenericSliceWindow *parent);
  virtual ~AnnotationInteractionMode();

  int OnMousePress(const FLTKEvent &event);
  int OnKeyDown(const FLTKEvent &event) {return 0;};
  int OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent);
  int OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent) {return 0;};
  int OnShortcut(const FLTKEvent &event) {return 0;};
  void OnDraw();  

private:

  // Annotations
  typedef std::pair<Vector3d, Vector3d> LineIntervalType;
  typedef std::list<LineIntervalType> LineIntervalList;
  LineIntervalList m_Lines;

  // What happened on last click
  bool m_FlagDrawingLine;
  Vector3d m_LineStart;

  // The window handler needs to access our privates
  friend class IRISSliceWindow;
};



#endif // __AnnotationInteractionMode_h_
