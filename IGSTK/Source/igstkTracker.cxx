/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug 
// information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkTracker.h"

namespace igstk
{


/** Constructor */
Tracker::Tracker(void) :  m_StateMachine( this ) 
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // Set the state descriptors
  igstkAddStateMacro( Idle ); 
  igstkAddStateMacro( AttemptingToEstablishCommunication ); 
  igstkAddStateMacro( AttemptingToCloseCommunication); 
  igstkAddStateMacro( AttemptingToAttachTrackerTool );
  igstkAddStateMacro( TrackerToolAttached );
  igstkAddStateMacro( CommunicationEstablished );
  igstkAddStateMacro( AttemptingToTrack ); 
  igstkAddStateMacro( AttemptingToStopTracking); 
  igstkAddStateMacro( Tracking ); 
  igstkAddStateMacro( AttemptingToUpdate ); 
  
  // Set the input descriptors
  igstkAddInputMacro( EstablishCommunication);
  igstkAddInputMacro( AttachTrackerTool);
  igstkAddInputMacro( StartTracking); 
  igstkAddInputMacro( UpdateStatus); 
  igstkAddInputMacro( StopTracking); 
  igstkAddInputMacro( Reset); 
  igstkAddInputMacro( CloseCommunication); 
  igstkAddInputMacro( Success); 
  igstkAddInputMacro( Failure); 
  igstkAddInputMacro( ValidFrequency); 

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           EstablishCommunication,
                           AttemptingToEstablishCommunication,
                           AttemptToOpen );
  igstkAddTransitionMacro( Idle,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           AttachTrackerTool,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           UpdateStatus,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Reset,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           ValidFrequency,
                           Idle,
                           SetFrequency );

  // Transitions from the AttemptingToEstablishCommunication
  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Success,
                           CommunicationEstablished,
                           CommunicationEstablishmentSuccess );

  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Failure,
                           Idle,
                           CommunicationEstablishmentFailure );

  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           ValidFrequency,
                           AttemptingToEstablishCommunication,
                           ReportInvalidRequest );

  // Transitions from CommunicationEstablished
  igstkAddTransitionMacro( CommunicationEstablished,
                           AttachTrackerTool,
                           AttemptingToAttachTrackerTool,
                           AttemptToAttachTrackerTool );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StartTracking,
                           AttemptingToTrack,
                           AttemptToStartTracking );

  igstkAddTransitionMacro( CommunicationEstablished,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           Reset,
                           CommunicationEstablished,
                           ResetFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StopTracking,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           EstablishCommunication,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           UpdateStatus,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           ValidFrequency,
                           CommunicationEstablished,
                           SetFrequency );

  // Transitions from AttemptingToAttachTrackerTool
  igstkAddTransitionMacro( AttemptingToAttachTrackerTool,
                           Success,
                           TrackerToolAttached,
                           AttachingTrackerToolSuccess );

  igstkAddTransitionMacro( AttemptingToAttachTrackerTool,
                           Failure,
                           CommunicationEstablished,
                           AttachingTrackerToolFailure );

  igstkAddTransitionMacro( AttemptingToAttachTrackerTool,
                           ValidFrequency,
                           AttemptingToAttachTrackerTool,
                           ReportInvalidRequest );

  // Transitions from TrackerToolAttached
  igstkAddTransitionMacro( TrackerToolAttached,
                           StartTracking,
                           AttemptingToTrack,
                           AttemptToStartTracking );

  igstkAddTransitionMacro( TrackerToolAttached,
                           AttachTrackerTool,
                           AttemptingToAttachTrackerTool,
                           AttemptToAttachTrackerTool );

  igstkAddTransitionMacro( TrackerToolAttached,
                           ValidFrequency,
                           TrackerToolAttached,
                           SetFrequency );

  igstkAddTransitionMacro( TrackerToolAttached,
                           StopTracking,
                           TrackerToolAttached,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( TrackerToolAttached,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  // Transitions from AttemptingToTrack
  igstkAddTransitionMacro( AttemptingToTrack,
                           Success,
                           Tracking,
                           StartTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToTrack,
                           Failure,
                           CommunicationEstablished,
                           StartTrackingFailure );

  igstkAddTransitionMacro( AttemptingToTrack,
                           ValidFrequency,
                           AttemptingToTrack,
                           ReportInvalidRequest );

  // Transitions from Tracking
  igstkAddTransitionMacro( Tracking,
                           UpdateStatus,
                           AttemptingToUpdate,
                           AttemptToUpdateStatus );

  igstkAddTransitionMacro( Tracking,
                           StopTracking,
                           AttemptingToStopTracking,
                           AttemptToStopTracking );

  igstkAddTransitionMacro( Tracking,
                           Reset,
                           CommunicationEstablished,
                           ResetFromTrackingState );

  igstkAddTransitionMacro( Tracking,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromTrackingState );

  igstkAddTransitionMacro( Tracking,
                           ValidFrequency,
                           Tracking,
                           ReportInvalidRequest );

  // Transitions from AttemptingToUpdate
  igstkAddTransitionMacro( AttemptingToUpdate,
                           Success,
                           Tracking,
                           UpdateStatusSuccess );

  igstkAddTransitionMacro( AttemptingToUpdate,
                           Failure,
                           Tracking,
                           UpdateStatusFailure );

  igstkAddTransitionMacro( AttemptingToUpdate,
                           ValidFrequency,
                           AttemptingToUpdate,
                           ReportInvalidRequest );

  // Transitions from AttemptingToStopTracking
  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Success,
                           CommunicationEstablished,
                           StopTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Failure,
                           Tracking,
                           StopTrackingFailure );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           ValidFrequency,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Success,
                           Idle,
                           CloseCommunicationSuccess );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Failure,
                           CommunicationEstablished,
                           CloseCommunicationFailure );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           ValidFrequency,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create a PulseGenerator object.  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Tracker::RequestUpdateStatus );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  // This is update rate for sending tracking information to the
  // spatial objects, it should be set to at least 30 Hz
  const double DEFAULT_REFRESH_RATE = 30.0;
  m_PulseGenerator->RequestSetFrequency( DEFAULT_REFRESH_RATE );

  // This is the time period for which transformation should be
  // considered valid.  After this time, they expire.  This time
  // is in milliseconds.
  const TimePeriodType DEFAULT_VALIDITY_TIME = 400;
  m_ValidityTime = DEFAULT_VALIDITY_TIME;

  // By default, the reference is not used
  m_ApplyingReferenceTool = false;

  m_ConditionNextTransformReceived = itk::ConditionVariable::New();
  m_Threader = itk::MultiThreader::New();
  m_ThreadingEnabled = false;
  m_TrackingThreadStarted = false;
}

/** Destructor */
Tracker::~Tracker(void)
{
}

/** This method sets the reference tool. */
void Tracker::RequestSetReferenceTool( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestSetReferenceTool called ...\n");
  // connect the reference tracker tool the tracker 
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                    igstk::TimeStamp::GetLongestPossibleTime() );
  
  trackerTool->RequestSetTransformAndParent( identityTransform, this );

  if( trackerTool != NULL )
    {
    // check if it is already attached to the tracker
    typedef TrackerToolsContainerType::iterator InputIterator;
    InputIterator toolItr = 
                m_TrackerTools.find( trackerTool->GetTrackerToolIdentifier() );

    if( toolItr != m_TrackerTools.end() )
      {
      m_ApplyingReferenceTool = true;
      m_ReferenceTool = trackerTool;

      //  Connect the coordinate system of all the other tracker tools to
      //  the reference tracker tool. In other words, make reference tracker tool
      //  the parent of all the other tracker tools.
      typedef TrackerToolsContainerType::iterator  InputConstIterator;

      InputConstIterator inputItr = m_TrackerTools.begin();
      InputConstIterator inputEnd = m_TrackerTools.end();

      while( inputItr != inputEnd )
        {
        // avoid making the reference tracker tool become parent of itself.
        if ( (inputItr->first) != m_ReferenceTool->GetTrackerToolIdentifier())
          {
          (inputItr->second)->RequestSetTransformAndParent( 
                                      identityTransform, m_ReferenceTool );
          }
        ++inputItr;
        }
      }
    else
      {
      igstkLogMacro( CRITICAL, "Request to use a tracker tool as a reference"
      << "has failed. The tracker tool is not attached to the tracker ");
      }
   }
}

/** The "RequestOpen" method attempts to open communication with the
 *  tracking device. */
void Tracker::RequestOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestOpen called...\n");
  igstkPushInputMacro( EstablishCommunication );
  this->m_StateMachine.ProcessInputs();
}


/** The "RequestClose" method closes communication with the device. */
void Tracker::RequestClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestClose called ...\n");
  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();
}

/** The "RequestReset" tracker method should be used to bring the tracker
 * to some defined default state. */
void Tracker::RequestReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestReset called ...\n");
  igstkPushInputMacro( Reset );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStartTracking" method readies the tracker for tracking the
 *  tools connected to the tracker. */
void Tracker::RequestStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestStartTracking called ...\n");
  igstkPushInputMacro( StartTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStopTracking" stops tracker from tracking the tools. */
void Tracker::RequestStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestStopTracking called ...\n");
  igstkPushInputMacro( StopTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestUpdateStatus" method is used for updating the status of 
 *  ports and tools when the tracker is in tracking state. */
void Tracker::RequestUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestUpdateStatus called ...\n");
  igstkPushInputMacro( UpdateStatus );
  m_StateMachine.ProcessInputs();
}


/** The "RequestSetFrequency" method is used for defining the rate at which 
 * Transforms are queried from the Tracker device */
void Tracker::RequestSetFrequency( double frequencyInHz )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestSetFrequency called ...\n");
  if( frequencyInHz > 0.0 )
    {
    this->m_FrequencyToBeSet = frequencyInHz;
    igstkPushInputMacro( ValidFrequency );
    m_StateMachine.ProcessInputs();
    }
}

/** The "AttemptToOpen" method attempts to open communication with a
 *  tracking device. */
void Tracker::AttemptToOpenProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::Tracker::AttemptToOpenProcessing called ...\n");

  ResultType result = this->InternalOpen();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after communication setup has been successful. */ 
void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerOpenEvent() );
}


/** Post-processing after communication setup has failed. */ 
void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");

  this->InvokeEvent( TrackerOpenErrorEvent() );
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void Tracker::ResetFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "ResetFromTrackingStateProcessing() called ...\n");
  // leaving TrackingState, going to CommunicationEstablishedState
  this->ExitTrackingStateProcessing();
  this->ResetFromToolsActiveStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished  state */
void Tracker::ResetFromToolsActiveStateProcessing( void )
{
  igstkLogMacro( DEBUG, 
         "igstk::Tracker::ResetFromToolsActiveStateProcessing() called ...\n");
  this->ResetFromCommunicatingStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void Tracker::ResetFromCommunicatingStateProcessing( void )
{
  ResultType result = this->InternalReset();

  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset failed ...\n");
    }
}

/** Post-processing after ports and tools setup has been successful. */ 
void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerInitializeEvent() );
}

/** Post-processing after ports and tools setup has failed. */ 
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerInitializeErrorEvent() );
}

/** The "AttemptToStartTracking" method attempts to start tracking. */
void Tracker::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStartTrackingProcessing  "
                 "called ...\n");

  ResultType result = this->InternalStartTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after start tracking has been successful. */ 
void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingSuccessProcessing "
                 "called ...\n");

  // Report to all the tracker tools that tracking has been started
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportTrackingStarted();
    ++inputItr;
    }


  // going from AttemptingToTrackState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerStartTrackingEvent() );
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerStartTrackingErrorEvent() );
}

/** Post-processing after attaching a tracker tool to the tracker
 *  has been successful. */ 
void Tracker::AttachingTrackerToolSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttachingTrackerToolSuccessProcessing "
                 "called ...\n");

  m_TrackerTools[ m_TrackerToolToBeAttached->GetTrackerToolIdentifier() ] 
                                   = m_TrackerToolToBeAttached; 

  // report to the tracker tool that the attachment has been 
  // successful
  m_TrackerToolToBeAttached->RequestReportSuccessfulTrackerToolAttachment();

  //connect the tracker tool coordinate system to the tracker
  //system. By default, make the tracker coordinate system to 
  //be a parent of the tracker tool coordinate system
  //If a reference tracker tool is specified, the reference
  //tracker tool will become the parent of all the tracker tools.
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                  igstk::TimeStamp::GetLongestPossibleTime() );

  m_TrackerToolToBeAttached->RequestSetTransformAndParent( 
    identityTransform, this );

  this->InvokeEvent( AttachingTrackerToolToTrackerEvent() );
}

/** Post-processing after attaching a tracker tool to the tracker
 *  has failed. */ 
void Tracker::AttachingTrackerToolFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttachingTrackerToolFailureProcessing "
                 "called ...\n");

  // report to the tracker tool that the attachment has failed
  m_TrackerToolToBeAttached->RequestReportFailedTrackerToolAttachment();

  this->InvokeEvent( AttachingTrackerToolToTrackerErrorEvent() );
}
 
/** The "AttemptToStopTracking" method attempts to stop tracking. */
void Tracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStopTrackingProcessing "
                        "called ...\n");
  // leaving TrackingState, going to AttemptingToStopTrackingState
  this->ExitTrackingStateProcessing();

  ResultType result = this->InternalStopTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after stop tracking has been successful. */ 
void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingSuccessProcessing "
                 "called ...\n");

  // Report to all the tracker tools that tracking has been stopped
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportTrackingStopped();
    ++inputItr;
    }

  this->InvokeEvent( TrackerStopTrackingEvent() );  
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopTrackingState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerStopTrackingErrorEvent() );  
}

/** Needs to be called every time when entering tracking state. */ 
void Tracker::EnterTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::EnterTrackingStateProcessing "
                 "called ...\n");

  if ( ! m_TrackingThreadStarted && this->GetThreadingEnabled() )
    {
    m_ThreadID = m_Threader->SpawnThread( TrackingThreadFunction, this );
    m_TrackingThreadStarted= true;
    }

  m_PulseGenerator->RequestStart();
}

/** Needs to be called every time when exiting tracking state. */ 
void Tracker::ExitTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ExitTrackingStateProcessing "
                 "called ...\n");
  m_PulseGenerator->RequestStop();
}

/** The "AttemptToUpdateStatus" method attempts to update status
    during tracking. */
void Tracker::AttemptToUpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToUpdateStatusProcessing "
                        "called ...\n");

  // Set all tools to "not updated"
  //
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {
    (inputItr->second)->SetUpdated(false);
    ++inputItr;
    }
 
  // wait for a new transform to be available, it would be nice if
  // "Wait" had a time limit like pthread_cond_timedwait() on Unix or
  // WaitForSingleObject() on Windows
  if ( this->GetThreadingEnabled() )
    {
    m_ConditionNextTransformReceived->Wait( 
      & m_LockForConditionNextTransformReceived );
    }
  else
    {
    this->InternalThreadedUpdateStatus();
    }

  ResultType result = this->InternalUpdateStatus();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** This method is called when a call to UpdateStatus succeeded */
void Tracker::UpdateStatusSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatusSuccessProcessing "
                 "called ...\n");

  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    if ( (inputItr->second)->GetUpdated() &&
           ( !m_ApplyingReferenceTool || m_ReferenceTool->GetUpdated() ) ) 
      {
      const TransformType transform = (inputItr->second)->GetRawTransform();

      const double timeToExpiration = transform.GetExpirationTime() - 
                                      transform.GetStartTime();

      TransformType::VersorType rotationRaw;
      TransformType::VectorType translationRaw;

      translationRaw = transform.GetTranslation();
      rotationRaw    = transform.GetRotation();

      TransformType toolRawTransform;
      toolRawTransform.SetTranslationAndRotation( translationRaw, rotationRaw,
                        transform.GetError(),
                        timeToExpiration );

      (inputItr->second)->SetRawTransform( toolRawTransform );
 
      TransformType toolCalibrationTransform
        = (inputItr->second)->GetCalibrationTransform();
      
      TransformType::VersorType rotationCalibrated;
      TransformType::VectorType translationCalibrated;

      rotationCalibrated = toolCalibrationTransform.GetRotation();
      translationCalibrated = toolCalibrationTransform.GetTranslation();

      // transform by the tracker's tool transform
      rotationCalibrated = rotationRaw * rotationCalibrated;
      translationCalibrated = rotationRaw.Transform(translationCalibrated);
      translationCalibrated += translationRaw;

      TransformType toolCalibratedTransform;
      toolCalibratedTransform.SetTranslationAndRotation( 
        translationCalibrated, 
        rotationCalibrated,
        transform.GetError(),
        timeToExpiration );

      (inputItr->second)->SetCalibratedTransform( toolCalibratedTransform );
        
      // T ' = R^-1 * T * C
      //
      // where:
      // " T " is the raw transform reported by the device,
      // " C " is the tool calibration transform.
      // " R^-1 " is the inverse of the transform for the reference tool,
      // applying ReferenceTool

      TransformType::VersorType 
           rotationCalibratedWRTReferenceTrackerTool = rotationCalibrated;

      TransformType::VectorType 
           translationCalibratedWRTReferenceTrackerTool = translationCalibrated;

      if ( m_ApplyingReferenceTool )
        {
        // since this is an inverse transform, apply translation first
        TransformType::VersorType inverseRotation =
          m_ReferenceTool->GetRawTransform().GetRotation().GetReciprocal();

        translationCalibratedWRTReferenceTrackerTool -= 
               m_ReferenceTool->GetRawTransform().GetTranslation();

        translationCalibratedWRTReferenceTrackerTool =
          inverseRotation.Transform(
            translationCalibratedWRTReferenceTrackerTool);

        rotationCalibratedWRTReferenceTrackerTool = 
                       inverseRotation*rotationCalibratedWRTReferenceTrackerTool;

        // also include the reference tool's ToolCalibrationTransform
        inverseRotation = m_ReferenceTool->GetCalibrationTransform().
                                           GetRotation().GetReciprocal();

        translationCalibratedWRTReferenceTrackerTool -= 
          m_ReferenceTool->GetCalibrationTransform().GetTranslation();

        translationCalibratedWRTReferenceTrackerTool = 
          inverseRotation.Transform(
            translationCalibratedWRTReferenceTrackerTool);

        rotationCalibratedWRTReferenceTrackerTool = 
                 inverseRotation*rotationCalibratedWRTReferenceTrackerTool;
        }

      TransformType toolCalibratedTransformWRTReferenceTrackerTool;

      toolCalibratedTransformWRTReferenceTrackerTool.SetTranslationAndRotation( 
                        translationCalibratedWRTReferenceTrackerTool,
                        rotationCalibratedWRTReferenceTrackerTool,
                        transform.GetError(),
                        timeToExpiration );

      (inputItr->second)->
        SetCalibratedTransformWithRespectToReferenceTrackerTool( 
          toolCalibratedTransformWRTReferenceTrackerTool );

      // set transfrom with respect to the reference tool
      if ( m_ApplyingReferenceTool )
        {
        // avoid making the reference tracker tool become parent of itself.
        if ( (inputItr->first) != m_ReferenceTool->GetTrackerToolIdentifier())
          {
          (inputItr->second)->RequestSetTransformAndParent( 
            toolCalibratedTransformWRTReferenceTrackerTool, m_ReferenceTool );
          }
        }
      else
        {
        (inputItr->second)->RequestSetTransformAndParent( 
          toolCalibratedTransform, this );
        }
      }
    ++inputItr;
    }

  this->InvokeEvent( TrackerUpdateStatusEvent() );  
}

/** This method is called when a call to RequestUpdateStatus failed */
void Tracker::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatusFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerUpdateStatusErrorEvent() );  
}


/** The "CloseFromTrackingStateProcessing" method closes tracker in
 *  use, when the tracker is in tracking state. */
void Tracker::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromTrackingStateProcessing "
                 "called ...\n");

  // leaving TrackingState, going to AttemptingToCloseState
  this->ExitTrackingStateProcessing();

  ResultType result = this->InternalStopTracking();

  // detach all the tracker tools from the tracker
  this->DetachAllTrackerToolsFromTracker();

  // Terminating the TrackingThread and if it is started
  if ( m_TrackingThreadStarted && this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    m_TrackingThreadStarted = false;
    }


  if( result == SUCCESS )
    {
    result = this->InternalClose();
    }

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Detach all tracker tools from the tracker */
void Tracker::DetachAllTrackerToolsFromTracker()
{

  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();  
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    this->RemoveTrackerToolFromInternalDataContainers( inputItr->second ); 
    ++inputItr;
    }

  m_TrackerTools.clear();
}
 
/** The "CloseFromCommunicatingStateProcessing" method closes
 *  tracker in use, when the tracker is in communicating state. */
void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

  // Detach all the tracker tools from the tracker
  this->DetachAllTrackerToolsFromTracker();

  // Terminating the TrackingThread and if it is started
  if ( m_TrackingThreadStarted && this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    m_TrackingThreadStarted = false;
    }

  
  ResultType result = this->InternalClose();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after close tracking has been successful. */ 
void Tracker::CloseCommunicationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseCommunicationSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerCloseEvent() );
}

/** Post-processing after close tracking has failed. */ 
void Tracker::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseCommunicationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerCloseErrorEvent() );
}

/** Print object information */
void Tracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_PulseGenerator )
    {
    os << indent << this->m_PulseGenerator << std::endl;
    }

  if( this->m_PulseObserver )
    {
    os << indent << this->m_PulseObserver << std::endl;
    }

  os << indent << "ValidityTime: " << this->m_ValidityTime << std::endl;
}

/** The "SetFrequencyProcessing" passes the frequency value to the Pulse
 * Generator. Note that it is still possible for the PulseGenerator to reject
 * the value and stay at its current frequency. */
void Tracker::SetFrequencyProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::Tracker::SetFrequencyProcessing called ...\n");

  this->m_PulseGenerator->RequestSetFrequency( this->m_FrequencyToBeSet );
}


/** Request adding a tool to the tracker  */
void
Tracker::
RequestAttachTool( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "RequestAttachTool called ...\n");
 
  m_TrackerToolToBeAttached = trackerTool;
  
  igstkPushInputMacro( AttachTrackerTool );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToAttachTrackerToolProcessing" method attempts to
 * add a tracker tool to the tracker */
void Tracker::AttemptToAttachTrackerToolProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::AttemptToAttachTrackerToolProcessing called ...\n");

  // Verify the tracker tool information before adding it to the
  // tracker. The conditions that need be verified depend on 
  // the tracker type. For example, for MicronTracker, the 
  // the tracker should have access to the template file of the
  // Marker that is attached to the tracker tool. 
  ResultType result = 
    this->VerifyTrackerToolInformation( m_TrackerToolToBeAttached );
 
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Request remove a tool from the tracker  */
Tracker::ResultType 
Tracker::
RequestRemoveTool( TrackerToolType * trackerTool )
{
  this->m_TrackerTools.erase( trackerTool->GetTrackerToolIdentifier() );
  this->RemoveTrackerToolFromInternalDataContainers( trackerTool ); 
  return SUCCESS;
}

const Tracker::TrackerToolsContainerType &
Tracker::GetTrackerToolContainer() const
{
  return m_TrackerTools;
}

/** Thread function for tracking */
ITK_THREAD_RETURN_TYPE Tracker::TrackingThreadFunction(void* pInfoStruct)
{
  struct itk::MultiThreader::ThreadInfoStruct * pInfo = 
    (struct itk::MultiThreader::ThreadInfoStruct*)pInfoStruct;

  if( pInfo == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  if( pInfo->UserData == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  Tracker *pTracker = (Tracker*)pInfo->UserData;

  // counters for error rates
  unsigned long errorCount = 0;
  unsigned long totalCount = 0;

  int activeFlag = 1;
  while ( activeFlag )
    {
    ResultType result = pTracker->InternalThreadedUpdateStatus();
    pTracker->m_ConditionNextTransformReceived->Signal();
    
    totalCount++;
    if (result != SUCCESS)
      {
      errorCount++;
      }
      
    // check to see if we are being told to quit 
    pInfo->ActiveFlagLock->Lock();
    activeFlag = *pInfo->ActiveFlag;
    pInfo->ActiveFlagLock->Unlock();
    }
  
  igstkLogMacroStatic(pTracker, DEBUG, "TrackingThreadFunction was "
                      "terminated, " << errorCount << " errors "
                      "out of " << totalCount << "updates." << std::endl );

  return ITK_THREAD_RETURN_VALUE;
}

/** Report to the tracker tool that the tool is not available */
void 
Tracker::ReportTrackingToolNotAvailable( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::ReportTrackingToolNotAvailable called...\n");
  trackerTool->RequestReportTrackingToolNotAvailable();
}

/** Report to the tracker tool that the tool is Visible */
void 
Tracker::ReportTrackingToolVisible( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ReportTrackingToolVisible called...\n");
  trackerTool->RequestReportTrackingToolVisible();
}

/** Set raw transform */
void 
Tracker::SetTrackerToolRawTransform( 
  TrackerToolType * trackerTool, TransformType transform )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::SetTrackerToolRawTransform called...\n");
  trackerTool->SetRawTransform( transform );
}

/** Turn on/off update flag of the tracker tool */
void 
Tracker::SetTrackerToolTransformUpdate( 
  TrackerToolType * trackerTool, bool flag )
{
  igstkLogMacro( DEBUG, 
     "igstk::Tracker::SetTrackerToolTransformUpdate called...\n");
  trackerTool->SetUpdated( flag ); 
}

/** Report invalid request */
void Tracker::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

}
