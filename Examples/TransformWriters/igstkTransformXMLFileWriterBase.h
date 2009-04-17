#ifndef __igstkTransformXMLFileWriterBase_h
#define __igstkTransformXMLFileWriterBase_h

#include <itkXMLFile.h>
#include "igstkPrecomputedTransformData.h"


namespace igstk
{
/** \class TransformXMLFileWriterBase
 * 
 *  \brief This class is an abstract writer for xml files containing a 
 *         precomputed transformation. The specific type of transformation is
 *         defined by the subclass.
 *
 *  This class is the base class for all the writers that output a precomputed
 *  transformation. 
 *
 *  The xml file format is as follows:
    @verbatim
    <precomputed_transform>
   
    <description>
       Claron bayonet probe calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       your parameters go here
    </transformation>
     
   </precomputed_transform>
   @endverbatim
 *
 * 
 */
class TransformXMLFileWriterBase : public itk::XMLWriterBase<PrecomputedTransformData> 
{
public:
  
  //standard typedefs
  typedef TransformXMLFileWriterBase                              Self;
  typedef itk::XMLWriterBase<PrecomputedTransformData>            Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( TransformXMLFileWriterBase<PrecomputedTransformData>, 
                itk::XMLWriterBase );

  virtual int CanWriteFile( const char* name );
  virtual int WriteFile();
  /**Check that the transformation matches the writer type*/ 
  virtual bool IsCompatible( PrecomputedTransformData::Pointer transformation )=0;

protected:

  TransformXMLFileWriterBase() {}
  virtual ~TransformXMLFileWriterBase() {}

  void WriteDescription( std::ofstream &out );
  void WriteDate( std::ofstream &out ); 
  virtual void WriteTransformation( std::ofstream &out ) = 0;
 
       //observer to get the transformation description using IGSTK's event
       //based approach
  igstkObserverMacro( TransformationDescription, 
                      igstk::StringEvent, 
                      std::string )

       //observer to get the transformation date using IGSTK's event
       //based approach
  igstkObserverMacro( TransformationDate, 
                      igstk::PrecomputedTransformData::TransformDateTypeEvent, 
                      std::string )

  typedef igstk::PrecomputedTransformData::TransformType * 
    TransformTypePointer;

         //observer to get the transformation using IGSTK's event
       //based approach
  igstkObserverMacro( TransformRequest, 
                      igstk::PrecomputedTransformData::TransformTypeEvent, 
                      TransformTypePointer )

         //observer to get the transformation's estimation error using IGSTK's 
         //event based approach
  igstkObserverMacro( TransformError, 
                      igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                      igstk::PrecomputedTransformData::ErrorType )

private:

  TransformXMLFileWriterBase( 
    const TransformXMLFileWriterBase & other );


  const TransformXMLFileWriterBase & operator=( 
    const TransformXMLFileWriterBase & right );
};


}
#endif //__igstkTransformXMLFileWriterBase_h