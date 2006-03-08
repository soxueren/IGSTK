/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUSImageObject_h
#define __igstkUSImageObject_h

#include "igstkImageSpatialObject.h"

namespace igstk
{

/** \class USImageObject
 * \brief  Represents a MR scan placed correctly in physical coordinates.
 *
 *  This class is the data structure used for containing a volumetric MRI scan.
 *  The scan is usually provided by a MRImageReader class, but can also be
 *  provided by other sources. Due to the protection of the ITK layer, any
 *  other potential source of ITK images must be included in the Friends
 *  mechanism.
 *
 *  \sa CTImageSpatialObject
 *  \sa MRImageReader
 *  \sa USImageObjectRepresentation

 * \ingroup Object
 */

class USImageObject : public ImageSpatialObject< unsigned char, 3 >
{

public:

  /** Typedef for the superclass. This is needed because the 
   *  igstkStandardClassTraitsMacro gets confused with the commas
   *  in the template declaration. */
  typedef ImageSpatialObject< unsigned char, 3 >  SuperclassType;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( USImageObject, SuperclassType )

protected:

  USImageObject( void );
  virtual ~USImageObject( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  USImageObject(const Self&);     //purposely not implemented
  void operator=(const Self&);           //purposely not implemented


};

} // end namespace igstk

#endif // __igstkUSImageObject_h
