/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemSetTransformResult.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateSystemSetTransformResult.h"

namespace igstk
{
CoordinateSystemSetTransformResult
::CoordinateSystemSetTransformResult()
{
  m_Source = NULL;
  m_Destination = NULL;
  isAttach = false;
}

CoordinateSystemSetTransformResult
::CoordinateSystemSetTransformResult(
      const CoordinateSystemSetTransformResult& in)
{
  m_Transform = in.m_Transform;
  m_Source = in.m_Source;
  m_Destination = in.m_Destination;
  isAttach  = in.isAttach;
}

const CoordinateSystemSetTransformResult &
CoordinateSystemSetTransformResult
::operator = ( 
      const CoordinateSystemSetTransformResult& in)
{
  if (this != &in)
    {
    m_Transform = in.m_Transform;
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    isAttach  = in.isAttach;
    }
  return *this;
}

void
CoordinateSystemSetTransformResult
::Clear()
{
  m_Transform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  m_Source = NULL;
  m_Destination = NULL;
  isAttach  = false;
}

void
CoordinateSystemSetTransformResult
::Initialize(const Transform& trans, 
             const CoordinateSystem* src,
             const CoordinateSystem* dst, bool isAttaching)
{
  m_Transform = trans;
  m_Source = src;
  m_Destination = dst;
  isAttach = isAttaching;
}

const Transform & 
CoordinateSystemSetTransformResult
::GetTransform() const
{
  return this->m_Transform;
}

const CoordinateSystem * 
CoordinateSystemSetTransformResult
::GetSource() const
{
  return this->m_Source;
}

const CoordinateSystem *
CoordinateSystemSetTransformResult
::GetDestination() const
{
  return this->m_Destination;
}

const bool CoordinateSystemSetTransformResult::IsAttach() const
{
  return this->isAttach;
}

} // end namespace igstk
