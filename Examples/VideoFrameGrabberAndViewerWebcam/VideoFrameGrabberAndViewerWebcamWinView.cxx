/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerWebcamWinView.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include "VideoFrameGrabberAndViewerWebcamWinView.h"

namespace igstk
{

VideoFrameGrabberAndViewerWebcamWinView::VideoFrameGrabberAndViewerWebcamWinView(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L)
{
  Fl_Group * parentGroup = this->parent();

  m_X = 150; // width of the control panel
  m_Y = 0;

  m_Width = parentGroup->w() - m_X;
  m_Height = parentGroup->h();

  // Create widgets
  typedef igstk::FLTKWidget   WidgetType;

  m_VideoWidget    = new WidgetType(X, Y, m_Width, m_Height, "Display 0");

  // Create views
  m_VideoView    = ViewType3D::New();
  //m_VideoView->RequestSetOrientation( View2D::Axial );

  m_VideoWidget->RequestSetView( m_VideoView );

  end();
}

VideoFrameGrabberAndViewerWebcamWinView::~VideoFrameGrabberAndViewerWebcamWinView()
{
  delete m_VideoWidget;
}
} // end namespace igstk
