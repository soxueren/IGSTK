/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTransform.h"


namespace igstk
{

Transform
::Transform()
{
}


Transform
::~Transform()
{
}


const Transform &
Transform
::operator=( const Transform & inputTransform )
{
  m_Translation = inputTransform.m_Translation; 
  m_Rotation    = inputTransform.m_Rotation;
  m_TimeStamp   = inputTransform.m_TimeStamp;
  return *this;
}


void 
Transform
::SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Rotation    = rotation;
}


void 
Transform
::SetTranslation(
          const  VectorType & translation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
}


void 
Transform
::SetRotation(
          const  VersorType & rotation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Rotation = rotation;
}


const Transform::VectorType &
Transform
::GetTranslation() const
{
  return m_Translation;
}



const Transform::VersorType &
Transform
::GetRotation() const
{
  return m_Rotation;
}



double 
Transform
::GetStartTime() const
{
  return m_TimeStamp.GetStartTime();
}



double 
Transform
::GetExpirationTime() const
{
  return m_TimeStamp.GetExpirationTime();
}


bool
Transform
::IsValidAtTime( double timeToCheckInMilliseconds ) const
{
  return m_TimeStamp.IsValidAtTime( timeToCheckInMilliseconds );
}


void 
Transform
::ExportTransform( ::vtkMatrix4x4 & outmatrix )
{
  VersorType::MatrixType inmatrix = m_Rotation.GetMatrix();
 
  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      outmatrix.SetElement(i,j,inmatrix.GetVnlMatrix().get(i,j));   
      }

    outmatrix.SetElement(i,3,m_Translation[i]);
    }
}


} // end namespace itk


