/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VascularNetworkObject.cxx
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
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
//
// \index{igstk::VascularNetworkObject}
// This example describes how to create a \doxygen{VesselObject} and 
// how to use the \doxygen{VascularNetworkObject} to group VesselObjects 
// together to represent a vascular tree.
//
// We first include the header files:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkVascularNetworkObject.h"
#include "igstkVesselObject.h"
// Software Guide : EndCodeSnippet

igstkObserverObjectMacro(Vessel,
  ::igstk::VesselObjectModifiedEvent,
  ::igstk::VesselObject)

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// Next we declare \doxygen{VascularNetworkObject}:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::VascularNetworkObject VascularNetworkType;
  VascularNetworkType::Pointer vasculature = VascularNetworkType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then we create \doxygen{VesselObject}:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::VesselObject VesselType;
  VesselType::Pointer vessel = VesselType::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Like the \doxygen{TubeObject}, a \doxygen{VesselObject} is defined as a collection
// of centerline points with an associated radius.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef VesselType::PointType PointType;
  PointType pt;
  pt.SetPosition(0,1,2);
  pt.SetRadius(10);
  vessel->AddPoint(pt);

  pt.SetPosition(1,2,3);
  pt.SetRadius(20);
  vessel->AddPoint(pt);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We then add the newly created vessel to the vascular network. We use the
// \code{RequestAddVessel()} function. This function requires to provide
// the \doxygen{Transform} that relates the vessel to the vascular tree.
// In this example we set the identity transform with the longest possible
// expiration time.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  vasculature->RequestAddVessel( transform, vessel );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// In some cases, it might interesting to get a vessel of interest from a
// \code{VascularNetworkObject}. To retrieve the vessel, we first need to create
// an observer.
// \begin{verbatim}
// igstkObserverObjectMacro(Vessel,
//  ::igstk::VascularNetworkObject::VesselObjectModifiedEvent,
//  ::igstk::VesselObject)
// \end{verbatim}
// Note that the declaration of the observer should be done outside 
// of the class. As we have seen in the previous section this macro will create two functions 
// depending on the name of the first argument:
// \begin{enumerate}
// \item \code{GotVessel()} which returns true if the vessel exists.
// \item \code{GetVessel()} which returns the actual pointer to the vessel.
// \end{enumerate}
// Once the observer is declared we add it to the VascularNetworkProject using
// the \code{AddObserver()} function.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef VesselObserver VesselObserver;
  VesselObserver::Pointer vesselObserver = VesselObserver::New();
 
  vasculature->AddObserver(
            igstk::VesselObjectModifiedEvent(),
            vesselObserver);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We then request a vessel given its position in the list
// using the \code{RequestGetVessel(unsigned long position)} function.
// We also check to see if the observer got the vessel, i.e if the vessel exists.
// 
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  vasculature->RequestGetVessel(0);
  if(!vesselObserver->GotVessel())
    {
    std::cout << "No Vessel!" << std::endl;
    return EXIT_FAILURE;
    }
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// The vessel is retrieved using the \code{GetVessel()} function from the
// observer, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet 
  VesselType::Pointer outputVessel = vesselObserver->GetVessel();

  outputVessel->Print(std::cout);
  std::cout << "Number of points in the vessel = "
            << outputVessel->GetNumberOfPoints() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
