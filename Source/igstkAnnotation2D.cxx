/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAnnotation2D.h" 
#include "vtkActor2D.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"


namespace igstk 
{ 

/** Constructor */
Annotation2D::Annotation2D():m_StateMachine(this),m_Logger(NULL)
{
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  for (int i = 0; i < 4; i++)
  {
    this->m_AnnotationText[i] = "";
    this->m_AnnotationMapper[i] = vtkTextMapper::New();
    this->m_AnnotationActor[i]  = vtkActor2D::New();
    this->m_AnnotationActor[i]->SetMapper(this->m_AnnotationMapper[i]);
    m_ActorToBeAdded = m_AnnotationActor[i];
    this->AddActorProcessing( );
  }
  
  igstkAddInputMacro( ValidAnnotationIndex );
  igstkAddInputMacro( InvalidAnnotationIndex );
  igstkAddInputMacro( ValidAnnotations );
  igstkAddInputMacro( InvalidAnnotations );
  
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AnnotationsAdded ); 

  igstkAddTransitionMacro ( Idle, ValidAnnotationIndex , Idle, AddAnnotationText );  
  igstkAddTransitionMacro ( Idle, InvalidAnnotationIndex , Idle, ReportInvalidAnnotationIndex );  
  igstkAddTransitionMacro ( Idle, ValidAnnotations , AnnotationsAdded , AddAnnotations );  

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
Annotation2D::~Annotation2D()  
{ 
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  this->DeleteActors();

  for (int i = 0; i < 4; i++)
  {
    this->m_AnnotationMapper[i]->Delete();
  }
}

/** Add actor */
void Annotation2D::AddActorProcessing( )
{
  igstkLogMacro( DEBUG, "AddActorProcessing called ...\n");
  m_Actors.push_back( m_ActorToBeAdded );
}

/** */
void Annotation2D::RequestAddAnnotationText( int i, const std::string  & text )
{
  igstkLogMacro( DEBUG, "RequestAddAnnotationText called ...\n");  

  m_IndexForAnnotationToBeAdded = i;
  m_AnnotationTextToBeAdded = text;
 
  if ( m_IndexForAnnotationToBeAdded < 0 && m_IndexForAnnotationToBeAdded > 3 ) 
    {
      igstkPushInputMacro( InvalidAnnotationIndex );
      m_StateMachine.ProcessInputs();
    }
  else
    {
      igstkPushInputMacro ( ValidAnnotationIndex );
      m_StateMachine.ProcessInputs();
    }
}



/** Add annotation text processing */
void Annotation2D::AddAnnotationTextProcessing( )
{
  igstkLogMacro( DEBUG, "AddAnnotationTextProcessing called ...\n");

  m_AnnotationText[m_IndexForAnnotationToBeAdded] = m_AnnotationTextToBeAdded;
  m_AnnotationMapper[m_IndexForAnnotationToBeAdded]->
                  SetInput( m_AnnotationText[m_IndexForAnnotationToBeAdded].c_str() );
  this->m_AnnotationActor[m_IndexForAnnotationToBeAdded]->
                  SetMapper(this->m_AnnotationMapper[m_IndexForAnnotationToBeAdded]);
}

/** */
void Annotation2D::RequestAddAnnotations( int horizontalSize, int verticalSize )
{
  igstkLogMacro( DEBUG, "RequestAddAnnotations called ....\n"  );
  
  this->m_ViewPortHorizontalSize = horizontalSize;
  this->m_ViewPortVerticalSize = verticalSize;
  igstkPushInputMacro( ValidAnnotations );
  m_StateMachine.ProcessInputs();
}


void Annotation2D::AddAnnotationsProcessing(  )
{
  igstkLogMacro( DEBUG, "AddAnnotationsProcessing called ...\n");

  this->m_AnnotationActor[0]->SetPosition(10,10);
  this->m_AnnotationActor[1]->SetPosition( this->m_ViewPortHorizontalSize-60 ,10);
  this->m_AnnotationActor[2]->SetPosition(10, this->m_ViewPortVerticalSize - 10);
  this->m_AnnotationActor[3]->SetPosition( this->m_ViewPortHorizontalSize - 60, this->m_ViewPortVerticalSize - 10);
}

/** Empty the list of actors */
void Annotation2D::DeleteActors()
{
  igstkLogMacro( DEBUG, "DeleteActors called....\n" );

  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  m_Actors.clear();
}

/** */
void Annotation2D::ReportInvalidAnnotationIndexProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidAnnotationIndexProcessing" );
}


/** Print Self tion */
void Annotation2D::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

