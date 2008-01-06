/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebCameraTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebCameraTrackerTool_h
#define __igstkWebCameraTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class WebCameraTrackerTool
  * \brief A Tracker-specific TrackerTool class.
  *
  * \ingroup Tracker
  *
  */

class WebCameraTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( WebCameraTrackerTool, TrackerTool )

  typedef std::string       NameType;

  /** Get Tracker tool mouse name */
  igstkGetStringMacro( Name );

  /** Set tracker tool mouse name */
  virtual void RequestSetName( const NameType & Name ); 

protected:

  WebCameraTrackerTool();
  ~WebCameraTrackerTool();

 /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured(); 

  WebCameraTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  NameType   m_Name;

  bool            m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_WebCameraTrackerTool_h_
