/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkUSImageObjectRepresentation.h"

namespace igstk
{ 

/** Constructor */
USImageObjectRepresentation
::USImageObjectRepresentation():m_StateMachine(this)
{

} 

/** Print Self function */
void USImageObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
