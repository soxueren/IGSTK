#include "igstkRigidTransformXMLFileWriter.h"

#include "igstkTransform.h"

namespace igstk
{

void 
RigidTransformXMLFileWriter::WriteTransformation( std::ofstream &out )
{
  TransformRequestObserver::Pointer transformObserver = 
    TransformRequestObserver::New();
  unsigned long observerID = this->m_InputObject->AddObserver( 
    igstk::PrecomputedTransformData::TransformTypeEvent(), 
    transformObserver );
  this->m_InputObject->RequestTransform();
  this->m_InputObject->RemoveObserver( observerID );

   TransformErrorObserver::Pointer transformErrorObserver = 
      TransformErrorObserver::New();
   observerID = this->m_InputObject->AddObserver( 
      igstk::PrecomputedTransformData::TransformErrorTypeEvent(), 
      transformErrorObserver );
  this->m_InputObject->RequestEstimationError();
  this->m_InputObject->RemoveObserver( observerID );

  if( transformObserver->GotTransformRequest() && 
      transformErrorObserver->GotTransformError() )
  {
    igstk::TransformBase *transformData = 
      transformObserver->GetTransformRequest();
    const igstk::Transform *rigidTransformation = 
      dynamic_cast<const igstk::Transform *>( transformData );
    out<<"\t <transformation estimation_error=\"";
    out<<transformErrorObserver->GetTransformError()<<"\">\n";    
    igstk::Transform::VectorType t = rigidTransformation->GetTranslation();
    igstk::Transform::VersorType r = rigidTransformation->GetRotation();
    out<<"\t"<<r.GetX()<<"\t"<<r.GetY()<<"\t"<<r.GetZ()<<"\t"<< r.GetW()<<"\t";
    out<<t[0]<<"\t"<<t[1]<<"\t"<<t[2]<<"\n";
    out<<"\t</transformation>\n\n";
  }
}


bool 
RigidTransformXMLFileWriter::IsCompatible( 
  PrecomputedTransformData::Pointer transformation )
{
  TransformRequestObserver::Pointer transformObserver = 
    TransformRequestObserver::New();
  unsigned long observerID = transformation->AddObserver( 
    igstk::PrecomputedTransformData::TransformTypeEvent(), 
    transformObserver );
  transformation->RequestTransform();

  if( transformObserver->GotTransformRequest() )
    {
    igstk::TransformBase *transformData = transformObserver->GetTransformRequest();
    return dynamic_cast<const igstk::Transform *>( transformData ) != NULL;
    }
  return false;
}

} //namespace