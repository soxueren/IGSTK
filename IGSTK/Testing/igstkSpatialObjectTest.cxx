/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkSpatialObject.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"

#include "igstkTransformObserverTestHelper.h"

namespace igstk
{
  
namespace SpatialObjectTest
{

class DummySpatialObject : public SpatialObject
{
public:
  igstkStandardClassTraitsMacro( DummySpatialObject, SpatialObject )

  bool TestMethods()
    {
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    this->RequestAddObject( NULL );        // Test with null pointer
#endif
    this->RequestSetInternalSpatialObject( NULL ); // Test with null pointer

    Self::Pointer sibling = Self::New();   // Test with valid pointer
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    this->RequestAddObject( sibling );
#endif

    typedef SpatialObject::SpatialObjectType SpatialObjectType;
    SpatialObjectType::Pointer so = SpatialObjectType::New();
    this->RequestSetInternalSpatialObject( so );   // Test with valid pointer

    sibling->RequestSetInternalSpatialObject( so );
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    this->RequestAddObject( sibling );
#endif

#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    const Self * me1 = dynamic_cast< const Self *>( this->GetObject(  0  ) );
    if( !me1 )
      {
      std::cerr << "Error in GetObject() with valid Id" << std::endl;
      return false;
      }
    
    const Self * me2 = dynamic_cast< const Self *>( this->GetObject( 100 ) );
    if( me2 )
      {
      std::cerr << "Error in GetObject() with invalid Id" << std::endl;
      return false;
      }
#endif

    return true;
  }

protected:
  DummySpatialObject( void ):m_StateMachine(this) {};
  ~DummySpatialObject( void ) {};
  
};

class MyTracker : public Tracker
{
public:

    /** Macro with standard traits declarations. */
    igstkStandardClassTraitsMacro( MyTracker, Tracker )

    typedef Superclass::TransformType           TransformType;

protected:
    MyTracker():m_StateMachine(this) 
      {
      m_Position[0] = 0.0;
      m_Position[1] = 0.0;
      m_Position[2] = 0.0;

      m_ValidityTime = 100.0; // 100.0 milliseconds
      }
    virtual ~MyTracker() {};

    typedef Tracker::ResultType                 ResultType;
    typedef TransformType::VectorType           PositionType;
    typedef TransformType::ErrorType            ErrorType;

    virtual ResultType InternalOpen( void ) { return SUCCESS; }
    virtual ResultType InternalStartTracking( void ) { return SUCCESS; }
    virtual ResultType InternalUpdateStatus( void ) 
      { 

      typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;
      TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();
     
      ConstIteratorType inputItr = trackerToolContainer.begin();
      ConstIteratorType inputEnd = trackerToolContainer.end();
     
      typedef igstk::Transform   TransformType;
      TransformType transform;

      while( inputItr != inputEnd )
        {
        transform.SetToIdentity( this->GetValidityTime() );
        m_Position[0] += 1.0;  // drift along a vector (1.0, 2.0, 3.0)
        m_Position[1] += 2.0;  // just to simulate a linear movement
        m_Position[2] += 3.0;  // being tracked in space.

        ErrorType errorValue = 0.5; 
        transform.SetTranslation( m_Position, errorValue, m_ValidityTime );
        // set the raw transform
        this->SetTrackerToolRawTransform( trackerToolContainer[inputItr->first], transform );
        this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], true );
        ++inputItr;
        }

      return SUCCESS; 
      }
    virtual ResultType InternalReset( void ) { return SUCCESS; }
    virtual ResultType InternalStopTracking( void ) { return SUCCESS; }
    virtual ResultType InternalClose( void ) { return SUCCESS; }

private:

    MyTracker(const Self&);  //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    typedef TrackerTool                 TrackerToolType;
    typedef Transform::TimePeriodType   TimePeriodType;
    TimePeriodType                      m_ValidityTime;
    PositionType                        m_Position;
};

} // end SpatialObjectTest namespace

} // end igstk namespace


int igstkSpatialObjectTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject  
    WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference =
    WorldReferenceSystemType::New();
  */

  typedef igstk::SpatialObjectTest::DummySpatialObject ObjectType;
  ObjectType::Pointer dummyObject = ObjectType::New();

  dummyObject->TestMethods();

  typedef igstk::SpatialObjectTest::MyTracker   TrackerType;
  TrackerType::Pointer  tracker = TrackerType::New();

  // FIXCS tracker->RequestAttachToSpatialObjectParent( worldReference );

  tracker->RequestOpen();

  typedef igstk::TrackerTool                TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->RequestAttachToTracker( tracker );

  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                      igstk::TimeStamp::GetLongestPossibleTime() );
   
  dummyObject->RequestSetTransformAndParent( identityTransform, trackerTool.GetPointer() );

  dummyObject->Print( std::cout );
  
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<50; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();
#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
    dummyObject->RequestGetTransform();
#endif
    }

  tracker->RequestStopTracking();
  tracker->RequestClose();

  std::cout << "Tracked object [PASSED]" << std::endl;


  ObjectType::Pointer dummyObject2 = ObjectType::New();

  // FIXCS dummyObject2->RequestAttachToSpatialObjectParent( worldReference );

  igstk::Transform trackerTransform;
  trackerTransform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  // FIXCS tracker->RequestSetTransformToSpatialObjectParent( trackerTransform );


  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
