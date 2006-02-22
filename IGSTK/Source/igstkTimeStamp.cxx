/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTimeStamp.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTimeStamp.h"
#include "igstkRealTimeClock.h"

namespace igstk
{

  
TimeStamp
::TimeStamp()
{
  this->m_StartTime       = 0;
  this->m_ExpirationTime  = 0;
}


TimeStamp
::~TimeStamp()
{
  this->m_StartTime       = 0;
  this->m_ExpirationTime  = 0;
}


const TimeStamp &
TimeStamp
::operator=( const TimeStamp & inputTimeStamp )
{
  this->m_StartTime      = inputTimeStamp.m_StartTime;
  this->m_ExpirationTime = inputTimeStamp.m_ExpirationTime;
  return *this;
}
   

void
TimeStamp
::SetStartTimeNowAndExpireAfter(double millisecondsToExpire) 
{
  this->m_StartTime      = RealTimeClock::GetTimeStamp();
  this->m_ExpirationTime = this->m_StartTime + millisecondsToExpire/1000;
}


double 
TimeStamp
::GetStartTime() const 
{
  return this->m_StartTime;
}


double 
TimeStamp
::GetExpirationTime() const 
{
  return this->m_ExpirationTime;
}


bool 
TimeStamp
::IsValidAtTime( double milliseconds ) const
{
  if( this->m_StartTime > milliseconds )
    {
    return false;
    }

  if( this->m_ExpirationTime < milliseconds )
    {
    return false;
    }

  return true;
}


void 
TimeStamp
::Print(std::ostream& os, itk::Indent indent) const
{
  this->PrintHeader(os, indent); 
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os, indent);
}


/**
 * Define a default print header for all objects.
 */
void 
TimeStamp
::PrintHeader(std::ostream& os, itk::Indent indent) const
{
  os << indent << "TimeStamp" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
TimeStamp
::PrintTrailer(std::ostream& itkNotUsed(os), 
               itk::Indent itkNotUsed(indent)) const
{

}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const TimeStamp& o)
{
  o.Print(os, 0);
  return os;
}


/** Print Self function */
void TimeStamp::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:    " << typeid( *this ).name() << std::endl;
  os << indent << "Start Time      = " << this->m_StartTime << std::endl;
  os << indent << "Expiration Time = " << this->m_ExpirationTime << std::endl;
}


} // end namespace igstk
