#include "igstkPivotCalibrationNew.h"

/*
#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#pragma warning( disable : 4786 )
#endif
*/

namespace igstk
{ 

const unsigned int PivotCalibrationNew::MAXIMAL_RETRIES = 300; //10 sec at 30Hz 

PivotCalibrationNew::PivotCalibrationNew() : m_StateMachine( this )
{
              //instantiate the class that performs the pivot calibration 
              //computation
  this->m_PivotCalibrationAlgorithm = PivotCalibrationAlgorithm::New();
              //observer for error's which use specific error messages 
  this->m_ErrorObserver = ErrorObserver::New();
                    //create the observers for all the requests that are 
                    //forwarded to the PivotCalibrationAlgorithm
  this->m_GetCalibrationTransformObserver = CalibrationTransformObserver::New();
  this->m_PivotCalibrationAlgorithm->AddObserver( TransformModifiedEvent() , 
                                                  this->m_GetCalibrationTransformObserver );
  this->m_GetPivotPointObserver = PivotPointObserver::New(); 
  this->m_PivotCalibrationAlgorithm->AddObserver( PointEvent() , 
                                                  this->m_GetPivotPointObserver );
  this->m_GetCalibrationRMSEObserver = CalibrationRMSEObserver::New(); 
  this->m_PivotCalibrationAlgorithm->AddObserver( DoubleTypeEvent() , 
                                                  this->m_GetCalibrationRMSEObserver );

        //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( AttemptingToComputeCalibration );
  igstkAddStateMacro( CalibrationComputed );

                   //define the state machine's inputs
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( ComputeCalibration );
  igstkAddInputMacro( GetTransform );
  igstkAddInputMacro( GetPivotPoint  );
  igstkAddInputMacro( GetRMSE  );

            //define the state machine's transitions
                         //transitions from Idle state
  igstkAddTransitionMacro(Idle,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(Idle,
                          Failed,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          ComputeCalibration,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetTransform,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetPivotPoint,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetRMSE,
                          Idle,
                          ReportInvalidRequest);
  
                         //transitions from AttemptingToInitialize state
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Initialize,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          ComputeCalibration,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTransform,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetPivotPoint,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetRMSE,
                          AttemptingToInitialize,
                          ReportInvalidRequest);
  
                         //transitions from Initialized state
  igstkAddTransitionMacro(Initialized,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(Initialized,
                          Failed,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          Succeeded,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ComputeCalibration);

  igstkAddTransitionMacro(Initialized,
                          GetTransform,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          GetPivotPoint,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          GetRMSE,
                          Initialized,
                          ReportInvalidRequest);
  
                     //transitions from AttemptingToComputeCalibration state
  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Initialize,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Failed,
                          Initialized,
                          ReportCalibrationComputationFailure);
  
  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Succeeded,
                          CalibrationComputed,
                          ReportCalibrationComputationSuccess);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetTransform,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetPivotPoint,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetRMSE,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);
  
                         //transitions from CalibrationComputed state
  igstkAddTransitionMacro(CalibrationComputed,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(CalibrationComputed,
                          Failed,
                          CalibrationComputed,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(CalibrationComputed,
                          Succeeded,
                          CalibrationComputed,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(CalibrationComputed,
                          ComputeCalibration,
                          CalibrationComputed,
                          Empty);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetTransform,
                          CalibrationComputed,
                          GetTransform);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetPivotPoint,
                          CalibrationComputed,
                          GetPivotPoint);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetRMSE,
                          CalibrationComputed,
                          GetRMSE);
  
             //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

         // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
} 


PivotCalibrationNew::~PivotCalibrationNew()  
{

}

void 
PivotCalibrationNew::RequestInitialize( unsigned int n, 
                                        igstk::Tracker::Pointer &tracker, 
                                        unsigned int toolPort, 
                                        unsigned int toolChannel)
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "RequestInitialize called...\n");
  this->m_TmpRequiredNumberOfTransformations = n;
  this->m_TmpTracker = tracker;
  this->m_TmpToolPort = toolPort;
  this->m_TmpToolChannel = toolChannel;  
  igstkPushInputMacro( Initialize );
  this->m_StateMachine.ProcessInputs();
}
  
void 
PivotCalibrationNew::RequestComputeCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "RequestComputeCalibration called...\n");
  igstkPushInputMacro( ComputeCalibration );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationNew::RequestCalibrationTransform()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "RequestCalibrationTransform called...\n");
  igstkPushInputMacro( GetTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationNew::RequestPivotPoint()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "RequestPivotPoint called...\n");
  igstkPushInputMacro( GetPivotPoint );
  this->m_StateMachine.ProcessInputs();  
}

void 
PivotCalibrationNew::RequestCalibrationRMSE()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "RequestCalibrationTransformRMSE called...\n");
  igstkPushInputMacro( GetRMSE );
  this->m_StateMachine.ProcessInputs();  
}


void 
PivotCalibrationNew::EmptyProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "EmptyProcessing called...\n");
}

void 
PivotCalibrationNew::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationNew::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

void 
PivotCalibrationNew::InitializeProcessing()
{
              //check that the tracker is in tracking state, update status 
              //should not generate an error
  unsigned long observerID = this->m_TmpTracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), 
                                                              this->m_ErrorObserver );
  this->m_TmpTracker->RequestUpdateStatus();                  
  this->m_TmpTracker->RemoveObserver( observerID );
                      //if the tracker isn't in tracking state the observer 
                      //has been notified
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
  }
  else 
  {
    this->m_Tracker = this->m_TmpTracker;
    this->m_ToolPort = this->m_TmpToolPort;
    this->m_ToolChannel = this->m_TmpToolChannel;
    this->m_RequiredNumberOfTransformations = this->m_TmpRequiredNumberOfTransformations;
    this->m_Transforms.clear();
    this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    igstkPushInputMacro( Succeeded );
  }
}

void 
PivotCalibrationNew::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "ReportInitializationFailureProcessing called...\n");
  this->InvokeEvent( InitializationFailureEvent() );
}
  
void 
PivotCalibrationNew::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "ReportInitializationSuccessProcessing called...\n");
  this->InvokeEvent( InitializationSuccessEvent() );
}

void 
PivotCalibrationNew::ComputeCalibrationProcessing()
{
             //get the required number of transformations and then perform the 
             //calibration
  unsigned int numberOfAcquisitionAttempts, currentSampleSize = 0;
  igstk::Transform currentTransform;

  this->m_ReasonForCalibrationFailure.clear();
  unsigned long observerID = this->m_Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), 
                                                           this->m_ErrorObserver);

               //the following lines are part of the hack that allows us to 
               //check if a transformation is valid based on its time stamps
  igstk::TimeStamp::TimePeriodType currentTime, prevTime; 

  prevTime = -1; //invalid time
  this->InvokeEvent( DataAcquisitionStartEvent() );
                       //get the data for the calibration
  while( currentSampleSize < this->m_RequiredNumberOfTransformations ) 
  {
    numberOfAcquisitionAttempts = 0;
    bool acquired = false;
    while( numberOfAcquisitionAttempts < PivotCalibrationNew::MAXIMAL_RETRIES && 
           !acquired) 
    {
      this->m_Tracker->RequestUpdateStatus();
      currentTime = igstk::RealTimeClock::GetTimeStamp();
                           //an internal tracker error occured, we can't recover      
      if( this->m_ErrorObserver->ErrorOccured() ) 
      {        
        this->m_ErrorObserver->GetErrorMessage( this->m_ReasonForCalibrationFailure );
        igstkPushInputMacro( Failed );
        this->m_ErrorObserver->ClearError();
        this->m_Tracker->RemoveObserver(observerID);
        return;
      }
      else 
      {
        numberOfAcquisitionAttempts++;
        this->m_Tracker->GetToolTransform( this->m_ToolPort, this->m_ToolChannel, currentTransform);        
                     //For the transformation to be valid for our purposes it 
                     //must be valid as defined by its internal time span, in 
                     //addition IGSTK will not update the returned 
                     //transformation if the tool is not detected and it will 
                     //not generate an error event. This can result in the same 
                     //transformation reported multiple times while its time 
                     //span is still valid. As we don't want these repeated 
                     //transforms we check that the time span is both valid with 
                     //regard to the current time and that it is not equivalent 
                     //to the previously reported transform's start time 
                     //(expiration time is just start time plus a fixed constant 
                     //for all transforms).
        if( currentTransform.IsValidAtTime(currentTime) && 
            currentTransform.GetStartTime()!= prevTime )
        {
          currentSampleSize++;
          DataAcquisitionEvent evt;
          evt.Set( (double)currentSampleSize/(double)(this->m_RequiredNumberOfTransformations) );
          this->InvokeEvent( evt );
          acquired = true;
          prevTime = currentTransform.GetStartTime();
          this->m_Transforms.push_back( currentTransform );
        }           
      }
    }               //tried to get a valid transformation MAXIMAL_RETRIES times 
                    //and failed
    if( numberOfAcquisitionAttempts == PivotCalibrationNew::MAXIMAL_RETRIES )  
    {
      std::stringstream msg;
      msg<< "Failed to acquire a valid transformation after ";
      msg<< PivotCalibrationNew::MAXIMAL_RETRIES<< " retries.";      
      this->m_ReasonForCalibrationFailure = msg.str();
      igstkPushInputMacro( Failed );
      this->m_Tracker->RemoveObserver( observerID );
      return;
    }
  }
  this->m_Tracker->RemoveObserver( observerID );
  this->InvokeEvent( DataAcquisitionEndEvent() );

               //actually perform the calibration
  observerID = this->m_PivotCalibrationAlgorithm->AddObserver( PivotCalibrationAlgorithm::CalibrationFailureEvent(), 
                                                               this->m_ErrorObserver );
  this->m_PivotCalibrationAlgorithm->RequestAddTransforms( this->m_Transforms );
  this->m_PivotCalibrationAlgorithm->RequestComputeCalibration();
                //check if the calibration computation failed
  if( this->m_ErrorObserver->ErrorOccured() ) 
  {        
    this->m_ErrorObserver->GetErrorMessage( this->m_ReasonForCalibrationFailure );
    igstkPushInputMacro( Failed );
    this->m_ErrorObserver->ClearError();
    this->m_PivotCalibrationAlgorithm->RemoveObserver(observerID);
    return;
  }
  igstkPushInputMacro( Succeeded );
}
  
void 
PivotCalibrationNew::ReportCalibrationComputationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "ReportCalibrationComputationSuccessProcessing called...\n");
  this->InvokeEvent( CalibrationSuccessEvent() );
}

void 
PivotCalibrationNew::ReportCalibrationComputationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "ReportCalibrationComputationFailureProcessing called...\n");
  CalibrationFailureEvent evt; 
  evt.Set( this->m_ReasonForCalibrationFailure );
  this->InvokeEvent( evt );
}

void 
PivotCalibrationNew::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "GetTransformProcessing called...\n");
  this->m_PivotCalibrationAlgorithm->RequestCalibrationTransform();
  if( this->m_GetCalibrationTransformObserver->GotCalibrationTransform() ) 
  {
    TransformModifiedEvent evt;
    evt.Set( this->m_GetCalibrationTransformObserver->GetCalibrationTransform() );
    this->InvokeEvent( evt );
  }  
}

void 
PivotCalibrationNew::GetPivotPointProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "GetPivotPointProcessing called...\n");
               //the events generated by the 
  this->m_PivotCalibrationAlgorithm->RequestPivotPoint(); 
  if( this->m_GetPivotPointObserver->GotPivotPoint() ) 
  {
    PointEvent evt;
    evt.Set( this->m_GetPivotPointObserver->GetPivotPoint() );
    this->InvokeEvent( evt );
  }
}

void 
PivotCalibrationNew::GetRMSEProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationNew::"
                  "GetTransformRMSEProcessing called...\n");
               //the events generated by the 
  this->m_PivotCalibrationAlgorithm->RequestCalibrationRMSE();
  if( this->m_GetCalibrationRMSEObserver->GotCalibrationRMSE() ) 
  {
    DoubleTypeEvent evt;
    evt.Set( this->m_GetCalibrationRMSEObserver->GetCalibrationRMSE() );
    this->InvokeEvent( evt );
  }
}

void 
PivotCalibrationNew::PrintSelf( std::ostream& os,
                                itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tracker: " << std::endl;
  os << indent << this->m_Tracker << std::endl;
  os << indent << "Tool information: " << std::endl;
  os << indent << "port " << this->m_ToolPort << ", channel ";
  os << this->m_ToolChannel << std::endl;
  os << indent << "Required number of transformations: " << std::endl;
  os << indent << m_RequiredNumberOfTransformations << std::endl;
}

PivotCalibrationNew::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{                             //tracker errors
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>( (igstk::TrackerUpdateStatusErrorEvent()).GetEventName(),
                                                                               "Tracker Error: failed updating transformations from hardware." ) );
                           //calibration errors
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>( (igstk::PivotCalibrationAlgorithm::CalibrationFailureEvent()).GetEventName(),
                                                                               "Pivot Calibration Algorithm: computation failed." ) );
}

void 
PivotCalibrationNew::ErrorObserver::Execute(itk::Object *caller, 
                                            const itk::EventObject & event) throw (std::exception)
{
  std::map<std::string,std::string>::iterator it;
  std::string className = event.GetEventName();
  it = this->m_ErrorEvent2ErrorMessage.find(className);

  this->m_ErrorOccured = true;
  this->m_ErrorMessage = (*it).second;
}

void 
PivotCalibrationNew::ErrorObserver::Execute(const itk::Object *caller, 
                                            const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk