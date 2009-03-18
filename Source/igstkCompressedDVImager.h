/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCompressedDVImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCompressedDVImager_h
#define __igstkCompressedDVImager_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkImager.h"
#include "igstkCompressedDVImagerTool.h"

#include <sys/types.h>
#include <linux/types.h>
#include <stdio.h>
#include <map>
#include <signal.h>

#include <libdv/dv.h>
#include <libiec61883/iec61883.h>

class vtkImageData;

namespace igstk {

/** \class CompressedDVImager
 * \brief This derivation of the Imager class provides communication
 * to the CompressedDV frame grabber
 *
 * This class controlls the communication with the video device.
 * The communication with the frame grabber is established with the unicap
 * library over firewire
 *
 * \ingroup Imager
 */

class CompressedDVImager : public Imager
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CompressedDVImager, Imager )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  CompressedDVImager(void);

  virtual ~CompressedDVImager(void);

  /** Typedef for internal boolean return type. */
  typedef Imager::ResultType   ResultType;

  /** Open communication with the imaging device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the imaging device. */
  virtual ResultType InternalClose( void );

  /** Put the imaging device into imaging mode. */
  virtual ResultType InternalStartImaging( void );

  /** Take the imaging device out of imaging mode. */
  virtual ResultType InternalStopImaging( void );

  /** Update the status and the transforms for all ImagerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the frames.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the imaging device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Verify imager tool information */
  virtual ResultType VerifyImagerToolInformation( const ImagerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified
   * frequency is valid for the imaging device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Create an associative container that maps error code to error
    * descritpion */
  static void CreateErrorCodeList();

  /** Get Error description given the error code */
  static std::string GetErrorDescription( unsigned int );

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveImagerToolFromInternalDataContainers( const
                                     ImagerToolType * imagerTool );

  /** Add imager tool entry to internal containers */
  virtual ResultType AddImagerToolToInternalDataContainers( const
                                     ImagerToolType * imagerTool );

private:

  CompressedDVImager(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Initialize camera */
  bool Initialize();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** A buffer to hold frames */
  typedef std::map< std::string, igstk::Frame >
                                ImagerToolFrameContainerType;

  typedef igstk::Frame   FrameType;
  ImagerToolFrameContainerType           m_ToolFrameBuffer;

  /** Error map container */
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  static ErrorCodeContainerType   m_ErrorCodeContainer;

  /** boolean to indicate if error code list is created */
  static bool m_ErrorCodeListCreated;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

  /** Members and functions for communication with DV library */

  public:
  FILE *f;// = NULL;//
  int node_specified;// = 0;//
  int channel;//
  int bandwidth;// = -1;//
  nodeid_t node;// = 0xffc0;//
  raw1394handle_t handle;//
  iec61883_dv_fb_t frame;//
  void dv_receive( raw1394handle_t handle, FILE *f, int channel);
  static unsigned char  *pixels[1];// = NULL;
  static unsigned char* frameBuffer;
  /** A mutex for multithreaded access to frameBuffer */
  static itk::MutexLock::Pointer  m_FrameBufferLock;

};

}  // namespace igstk

#endif //__igstk_CompressedDVImager_h_