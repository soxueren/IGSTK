/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAuroraTracker.h"
#include "igstkTrackerPort.h"

namespace igstk
{

AuroraTracker::AuroraTracker(void) : Tracker()
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    {
    this->m_PortEnabled[i] = 0;
    this->m_PortHandle[i] = 0;
    }
}


AuroraTracker::~AuroraTracker(void)
{
}


void AuroraTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "AuroraTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }
  igstkLogMacro( DEBUG, "AuroraTracker:: Exiting SetCommunication ...\n"); 
}


AuroraTracker::ResultType AuroraTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalOpen called ...\n");

  // Initialize the device 
  m_CommandInterpreter->INIT();

  if (m_CommandInterpreter->GetError())
    {
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}


AuroraTracker::ResultType AuroraTracker::InternalClose( void )
{
  // return to default comm settings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
  {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
  }
  //ndiClose(this->Device); To be done by the application
  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalActivateTools called ...\n");

  // load any SROMS that are needed
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
      }
    }

  this->EnableToolPorts();

  this->ClearPorts();

  m_NumberOfTools = 0;

  for(int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      AuroraToolPointer tool = AuroraToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalDeactivateTools( void )
{
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->ClearVirtualSROM(i);
      }
    }

  this->DisableToolPorts();

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}


AuroraTracker::ResultType AuroraTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalReset( void )
{
  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalUpdateStatus called ...\n");

  int errnum, port, ph;
  int status[NDI_NUMBER_OF_PORTS];
  int absent[NDI_NUMBER_OF_PORTS];
  unsigned long frame[NDI_NUMBER_OF_PORTS];
  double transform8[NDI_NUMBER_OF_PORTS][8];
  long flags;
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  // initialize transformations to identity
  for (int port = 0; port < NDI_NUMBER_OF_PORTS; port++)
    {
    transform8[port][0] = 1.0;
    transform8[port][1] = 0.0;
    transform8[port][2] = 0.0;
    transform8[port][3] = 0.0;
    transform8[port][4] = 0.0;
    transform8[port][5] = 0.0;
    transform8[port][6] = 0.0;
    transform8[port][7] = 0.0;
    }

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
  errnum = m_CommandInterpreter->GetError();

  if (errnum)
    {
    if (errnum == CommandInterpreterType::NDI_BAD_CRC ||
        errnum == CommandInterpreterType::NDI_TIMEOUT) // common errors
      {
      igstkLogMacro( WARNING, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    else
      {
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    return FAILURE;
    }

  // default to incrementing frame count by one (in case there are
  // no transforms for any tools)
  unsigned long nextcount = 0;

  for (port = 0; port < NDI_NUMBER_OF_PORTS; port++)
    {
    ph = this->m_PortHandle[port];
    if (ph == 0)
      {
      continue;
      }

    absent[port] = m_CommandInterpreter->GetTXTransform(ph, transform8[port]);
    status[port] = m_CommandInterpreter->GetTXPortStatus(ph);
    frame[port] = m_CommandInterpreter->GetTXFrame(ph);
  }

  /*
  // check to see if any tools have been plugged in
  if (m_CommandInterpreter->GetTXSystemStatus() & 
      CommandInterpreterType::NDI_PORT_OCCUPIED)
    { // re-configure, a new tool has been plugged in
    this->m_CommandInterpreter->TSTOP();
    this->EnableToolPorts();
    this->m_CommandInterpreter->TSTART();
    }
  else
    {
    for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
      {
      this->m_PortEnabled[tool] = ((status[tool] & mflags) == mflags);
      }
    }
  */

  for (int port = 0; port < NDI_NUMBER_OF_PORTS; port++) 
    {
    // convert status flags from NDI to vtkTracker format
    int port_status = status[port];
    flags = 0;
    if ((port_status & mflags) != mflags) 
      {
      flags |= TR_MISSING;
      }
    else
      {
      if (absent[port]) { flags |= TR_OUT_OF_VIEW;  }
      if (port_status & CommandInterpreterType::NDI_OUT_OF_VOLUME)
        {
        flags |= TR_OUT_OF_VOLUME;
        }
//      if (port_status & CommandInterpreterType::NDI_SWITCH_1_ON)  { flags |= TR_SWITCH1_IS_ON; }
//      if (port_status & CommandInterpreterType::NDI_SWITCH_2_ON)  { flags |= TR_SWITCH2_IS_ON; }
//      if (port_status & CommandInterpreterType::NDI_SWITCH_3_ON)  { flags |= TR_SWITCH3_IS_ON; }
      }

    // send the transform to the tool

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;
    translation[0] = transform8[port][4];
    translation[1] = transform8[port][5];
    translation[2] = transform8[port][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    rotation.Set(transform8[port][0],transform8[port][1],transform8[port][2],transform8[port][3]);

    // report NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = transform8[port][7];

    typedef TransformType::TimePeriodType TimePeriodType;
    TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port,0,transform);
    }

  return SUCCESS;
}


void AuroraTracker::AttachSROMFileNameToPort( const int portNum, std::string fileName )
{
  if ((portNum>=0) && (portNum<=NDI_NUMBER_OF_PORTS))
    {
    m_SROMFileNames[portNum] = fileName;
    }
}


bool AuroraTracker::LoadVirtualSROM( const int tool, std::string SROMFileName) 
{
  FILE *file = fopen(SROMFileName.c_str(),"rb");
  if (file == NULL)
  {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: couldn't find srom file ...\n");
    return false;
  }

  unsigned char data[1024];
  memset(data,0,1024);
  fread(data,1,1024,file);
  fclose(file);

  int errnum;
  int ph;
  int n, i;
  char hexbuffer[128];
  char location[14];

  if (tool < 3) // wired tools
    {
    m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);
    n = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (i = 0; i < n; i++)
      {
      if (m_CommandInterpreter->GetPHSRInformation(i) &
          CommandInterpreterType::NDI_TOOL_IN_PORT)
        {
        ph = m_CommandInterpreter->GetPHSRHandle(i);
        m_CommandInterpreter->PHINF(ph,
                                    CommandInterpreterType::NDI_BASIC |
                                    CommandInterpreterType::NDI_PORT_LOCATION);
        m_CommandInterpreter->GetPHINFPortLocation(location);
        if (tool == (location[10]-'0')*10 + (location[11]-'0') - 1)
          {
          break;
          }
        }
      }
    if (i == n)
      {
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: can't load SROM: no tool found in port ...\n");
      return false;
      }
    }
  else // wireless tools
    {
    // This is actually code for the POLARIS:
    // all tools past the first three assumed to be passive tools
    // identified by the letters "A", "B", "C", etc.
    char portIdentifier[3] = "00";
    portIdentifier[1] = tool-3+'A';

    m_CommandInterpreter->PHRQ("********", // device number
                               "*",        // TIU or SCU
                               "*",        // wired or wireless
                               portIdentifier,      // port
                               "**");      // channel
 
    ph = m_CommandInterpreter->GetPHRQHandle();
    }
  errnum = m_CommandInterpreter->GetError();
  if (errnum)
    {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return false;
    }

  for ( i = 0; i < 1024; i += 64)
    {
    // convert data to hexidecimal and write to virtual SROM in
    // 64-byte chunks
    m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64);
    m_CommandInterpreter->PVWR(ph, i, hexbuffer);
    }  
  return true;
}


void AuroraTracker::EnableToolPorts()
{
  int errnum = 0;
  int tool;
  int ph;
  int port;
//  int info;
  int mode;
  int ntools;
  int status;
  char identity[34];
  char location[14];
//  char partNumber[24];

  // reset our information about the tool ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
  {
    this->m_PortHandle[tool] = 0;
    this->m_PortEnabled[tool] = 0;
  }

  // free ports that are waiting to be freed
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    port = this->GetToolFromHandle(ph);
    m_CommandInterpreter->PHF(ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    }
  }

  // initialize ports waiting to be initialized
  do // repeat as necessary (in case multi-channel tools are used) 
    {
    m_CommandInterpreter->PHSR(
      CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
    
    ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (tool = 0; tool < ntools; tool++)
      {
      ph = m_CommandInterpreter->GetPHSRHandle(tool);
      m_CommandInterpreter->PINIT(ph);
      errnum = m_CommandInterpreter->GetError();
      if (errnum)
        { 
        igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
        igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
        }
      }
    }
  while (ntools > 0 && errnum == 0);

  // enable initialized tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_BASIC);
    m_CommandInterpreter->GetPHINFToolInfo(identity);
    if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
      { // reference
      mode = CommandInterpreterType::NDI_STATIC;
      }
    else
      { // anything else
      mode = CommandInterpreterType::NDI_DYNAMIC;
      }

    // enable the tool
    m_CommandInterpreter->PENA(ph, mode);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      {
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }
    }

  // get information for all tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      continue;
      }    
    // get the physical port identifier
    m_CommandInterpreter->GetPHINFPortLocation(location);
    if (location[11] >= 'A')
      {
      port = location[11] - 'A' + 3;
      }
    else
      {
      port = (location[10]-'0')*10 + (location[11]-'0') - 1;
      }
    if (port < NDI_NUMBER_OF_PORTS)
      {
      this->m_PortHandle[port] = ph;
      }

    status = m_CommandInterpreter->GetPHINFPortStatus();
    this->m_PortEnabled[port] = 
      ((status & CommandInterpreterType::NDI_ENABLED) != 0);
    
    }
}


void AuroraTracker::DisableToolPorts( void )
{
  int errnum = 0;
  int ph;
  int tool;
  int ntools;

  // disable all enabled tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ENABLED_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PDIS(ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }    
    }

  // disable the enabled ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    this->m_PortEnabled[tool] = 0;
    }
}


int AuroraTracker::GetToolFromHandle(int handle)
{
  int tool;

  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    if (this->m_PortHandle[tool] == handle)
      {
      return tool;
      }
    }
  return -1;
}


void AuroraTracker::ClearVirtualSROM(int tool)
{
  int ph = this->m_PortHandle[tool];
  m_CommandInterpreter->PHF(ph);
  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = 0;
}


/** Print Self function */
void AuroraTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  int i;
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] << std::endl;
    }
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "Port " << i << " Handle: " << m_PortHandle[i] << std::endl;
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "SROM filename " << i << " : " << m_SROMFileNames[i] << std::endl;
    }
}


}
