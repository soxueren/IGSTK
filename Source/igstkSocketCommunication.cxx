/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSocketCommunication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// Turn off deprecation warnings in MSVC 8+
#if (_MSC_VER >= 1400)
#pragma warning( disable : 4996 ) 
#endif
#endif

#include <time.h>

#include "igstkBinaryData.h"
#include "igstkSocketCommunication.h"

namespace igstk
{ 

/** Constructor */
SocketCommunication::SocketCommunication() :  m_StateMachine( this )
{

  // Set the state descriptors 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToOpenCommunication );
  igstkAddStateMacro( CommunicationOpened );
  igstkAddStateMacro( AttemptingToServerOpenPort );
  igstkAddStateMacro( ServerPortOpened );
  igstkAddStateMacro( AttemptingToServerConnect );
  igstkAddStateMacro( ServerConnected );
  igstkAddStateMacro( AttemptingToClientConnect );
  igstkAddStateMacro( ClientConnected );

  // Set the input descriptors
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( OpenCommunication );
  igstkAddInputMacro( ServerOpenPort );
  igstkAddInputMacro( ServerWaitForConnection );
  igstkAddInputMacro( ServerQueryForConnection );
  igstkAddInputMacro( ClientConnect );
  igstkAddInputMacro( Read );
  igstkAddInputMacro( Write );
  igstkAddInputMacro( ServerDisconnectConnectionSocket );
  igstkAddInputMacro( CloseCommunication );


  // Idle
  igstkAddTransitionMacro( Idle,
                           OpenCommunication,
                           AttemptingToOpenCommunication,
                           AttemptToOpenCommunication);

  // AttemptingToOpenCommunication
  igstkAddTransitionMacro( AttemptingToOpenCommunication,
                           Success,
                           CommunicationOpened,
                           Success );

  igstkAddTransitionMacro( AttemptingToOpenCommunication,
                           Failure,
                           Idle,
                           Failure );

  // CommunicationOpened
  igstkAddTransitionMacro( CommunicationOpened,
                           ServerOpenPort,
                           AttemptingToServerOpenPort,
                           AttemptToOpenPort );

  igstkAddTransitionMacro( CommunicationOpened,
                           ClientConnect,
                           AttemptingToClientConnect,
                           AttemptToConnect );

  igstkAddTransitionMacro( CommunicationOpened,
                           CloseCommunication,
                           Idle,
                           CloseCommunication );

  // AttemptingToServerOpenPort
  igstkAddTransitionMacro( AttemptingToServerOpenPort,
                           Success,
                           ServerPortOpened,
                           Success );

  igstkAddTransitionMacro( AttemptingToServerOpenPort,
                           Failure,
                           CommunicationOpened,
                           Failure );

  // ServerPortOpened
  igstkAddTransitionMacro( ServerPortOpened,
                           ServerWaitForConnection,
                           AttemptingToServerConnect,
                           AttemptToWaitForConnection );

  igstkAddTransitionMacro( ServerPortOpened,
                           ServerQueryForConnection,
                           AttemptingToServerConnect,
                           AttemptToQueryForConnection );

  igstkAddTransitionMacro( ServerPortOpened,
                           CloseCommunication,
                           Idle,
                           CloseCommunication );

  // AttemptingToServerConnect
  igstkAddTransitionMacro( AttemptingToServerConnect,
                           Success,
                           ServerConnected,
                           Success );

  igstkAddTransitionMacro( AttemptingToServerConnect,
                           Failure,
                           ServerPortOpened,
                           Failure );

  // ServerConnected
  igstkAddTransitionMacro( ServerConnected,
                           Read,
                           ServerConnected,
                           Read );

  igstkAddTransitionMacro( ServerConnected,
                           Write,
                           ServerConnected,
                           Write );  

  igstkAddTransitionMacro( ServerConnected,
                           ServerDisconnectConnectionSocket,
                           ServerPortOpened,
                           DisconnectConnectionSocket );

  igstkAddTransitionMacro( ServerConnected,
                           CloseCommunication,
                           Idle,
                           CloseCommunication );

  // AttemptingToClientConnect
  igstkAddTransitionMacro( AttemptingToClientConnect,
                           Success,
                           ClientConnected,
                           Success );

  igstkAddTransitionMacro( AttemptingToClientConnect,
                           Failure,
                           CommunicationOpened,
                           Failure );

  // ClientConnected
  igstkAddTransitionMacro( ClientConnected,
                           Read,
                           ClientConnected,
                           Read );

  igstkAddTransitionMacro( ClientConnected,
                           Write,
                           ClientConnected,
                           Write );

  igstkAddTransitionMacro( ClientConnected,
                           CloseCommunication,
                           Idle,
                           CloseCommunication );  
 
  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  this->m_SocketType = NONE_SOCKET;

  this->m_Socket = INVALID_SOCKET;

  this->m_ConnectionSocket = INVALID_SOCKET;  

  this->m_CaptureFileName = "";
  
  this->m_Capture = false;
  
  this->m_CaptureMessageNumber = 0;

} 


// Destructor 
SocketCommunication::~SocketCommunication()  
{
  this->SetLogger(0); 
  this->CloseCommunication();

}

/** Printself function */
void SocketCommunication::PrintSelf( std::ostream& os,
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  switch ( this->GetSocketType())
  {
  case NONE_SOCKET:
    os << indent << "Socket Type: NONE_SOCKET";
    break;
  case SERVER_SOCKET:
    os << indent << "Socket Type: SERVERE_SOCKET";
    break;
  case CLIENT_SOCKET:
    os << indent << "Socket Type: CLIENT_SOCKET";
    break;
  }

  os << indent << "Socket: " << this->GetSocket() << std::endl;

  os << indent << "Connection Socket: " << this->GetConnectionSocket() << std::endl;

  os << indent << "Address: " << this->GetAddress() << std::endl;

  os << indent << "Port: " << this->GetPortNumber() << std::endl;

  os << indent << "Capture: " << m_Capture << std::endl;
  
  os << indent << "CaptureFileName: " << m_CaptureFileName << std::endl;
  
  os << indent << "CaptureFileStream: " << m_CaptureFileStream << std::endl;
  
  os << indent << "CaptureMessageNumber: " << m_CaptureMessageNumber
     << std::endl;

}

/** Internal function to open communication. */
SocketCommunication::ResultType 
SocketCommunication::InternalOpenCommunicationProcessing( void ) 
{ 
  igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenCommunicationProcessing called ...\n");

#ifdef WIN32
  WORD wVersionRequested;
  WSADATA wsaData;

  wVersionRequested = MAKEWORD(2,2);
  memset(&wsaData, 0, sizeof(WSADATA));

  if( WSAStartup(wVersionRequested, &wsaData) != 0 )
    {
    return FAILURE;
    }
#endif

  int on;

  on = 1;
  this->m_Socket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->m_Socket == INVALID_SOCKET )
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenCommunicationProcessing: create socket error!\n");
    return FAILURE;
    }

  if (setsockopt(this->m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)))
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenCommunicationProcessing: set socket option error!\n");
    return FAILURE;
    }
  else
    {
    // Open a file for writing data stream.
    if( m_Capture )
      {
      time_t ti;
      time(&ti);

      igstkLogMacro( DEBUG, "Capture is on. Filename: "
                     << m_CaptureFileName << "\n" );

      if( m_Recorder.IsNull() )
        {
        m_Recorder = itk::Logger::New();
        m_Recorder->SetPriorityLevel(itk::Logger::DEBUG);
        m_Recorder->SetLevelForFlushing(itk::Logger::DEBUG);
        }
      if( m_CaptureFileOutput.IsNull() )
        {
        m_CaptureFileOutput = itk::StdStreamLogOutput::New();
        }
      m_CaptureFileStream.open(m_CaptureFileName.c_str());
      if( !m_CaptureFileStream.is_open() )
        {
        igstkLogMacro( CRITICAL,
                       "failed to open a file for writing data stream.\n" );
        }
      else
        {
        m_CaptureFileOutput->SetStream( m_CaptureFileStream );
        m_Recorder->AddLogOutput( m_CaptureFileOutput );
        igstkLogMacro2( m_Recorder, DEBUG, "# recorded " << asctime(localtime(&ti))
                            << "\n" );
        }
    }
    return SUCCESS; 
    }

}

/** Internal function to open port. */
SocketCommunication::ResultType 
SocketCommunication::InternalOpenPortProcessing( SocketCommunication::PortNumberType port )
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenPortProcessing called ...\n");

  struct sockaddr_in server;
  int opt;
  
  opt = 1;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

#ifdef WIN32
  setsockopt( this->m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
#else
  setsockopt( this->m_Socket, SOL_SOCKET, SO_REUSEADDR, (void*) &opt, sizeof(int));
#endif

  if ( bind( this->m_Socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) )
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenPortProcessing: bind error!\n");
    return FAILURE;
    }

  if ( listen( this->m_Socket, 10))
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalOpenPortProcessing: listen error!\n");
    return FAILURE;
    }
  else
    {
    this->m_SocketType = SERVER_SOCKET;
    return SUCCESS; 
    }
}

/** Internal function to select socket. */
SocketCommunication::ResultType 
SocketCommunication::InternalSelectSocketProcessing( unsigned int msec )
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalSelectSocketProcessing called ...\n");

  fd_set rset;
  struct timeval tval;
  struct timeval* tvalptr;
  
  if ( msec > 0 )
    {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = (msec % 1000)*1000;
    tvalptr = &tval;
    }
  else
    {
    tvalptr = NULL;
    }

  FD_ZERO(&rset);
  FD_SET( this->m_Socket, &rset);
  int res = select( this->m_Socket + 1, &rset, NULL, NULL, tvalptr);
  if (res == 0)
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalSelectSocketProcessing: time out!\n");
    return TIMEOUT;    
    }
  if ( res < 0 || !(FD_ISSET(this->m_Socket, &rset)) )
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalSelectSocketProcessing: no active socket!\n");
    return FAILURE;
    } 

  igstkLogMacro( DEBUG, "SocketCommunication::InternalSelectSocketProcessing: active socket found!\n");

  return SUCCESS; 
}

/** Internal function to wait for connection. */
SocketCommunication::ResultType 
SocketCommunication::InternalWaitForConnectionProcessing( )
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalWaitForConnectionProcessing called ...\n");

  SOCKET newsocket;

  newsocket = accept( this->m_Socket, 0, 0);
  if ( newsocket == INVALID_SOCKET )
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalWaitForConnectionProcessing: accept socket error!\n");
    return FAILURE;
    }
 
  this->m_ConnectionSocket = newsocket;

  igstkLogMacro( DEBUG, "SocketCommunication::InternalWaitForConnectionProcessing: accept socket!\n");

  return SUCCESS; 
}

/** Internal function to query for connection. */
SocketCommunication::ResultType 
SocketCommunication::InternalQueryForConnectionProcessing( unsigned int msec )
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalQueryForConnectionProcessing called ...\n");

  ResultType result;

  result = this->InternalSelectSocketProcessing( msec );
  if ( result != SUCCESS)
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalQueryForConnectionProcessing: no active socket found!\n");
    return result;
    }

  return this->InternalWaitForConnectionProcessing();
}

/** Internal function to connect. */  
SocketCommunication::ResultType 
SocketCommunication::InternalConnectProcessing( AddressType address, PortNumberType port ) 
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalConnectProcessing called ...\n");

  struct hostent* hp;
  struct sockaddr_in name;
  unsigned long addr;

  hp = gethostbyname(address.c_str());
  if (!hp)
    {
    addr = inet_addr(address.c_str());
    hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    }
  
  name.sin_family = AF_INET;
  memcpy( &name.sin_addr, hp->h_addr, hp->h_length);
  name.sin_port = htons(port);

  if( connect(this->m_Socket, reinterpret_cast<sockaddr*>(&name), sizeof(name)) < 0)
    {
    igstkLogMacro( DEBUG, "SocketCommunication::InternalConnectProcessing: socket connection error!\n");
    return FAILURE;
    }

  this->m_SocketType = CLIENT_SOCKET;
  igstkLogMacro( DEBUG, "SocketCommunication::InternalConnectProcessing: socket connected!\n");

  return SUCCESS; 

};

/** Internal function to close communication. */
SocketCommunication::ResultType 
SocketCommunication::InternalCloseCommunicationProcessing( void ) 
{ 
  igstkLogMacro( DEBUG, "SocketCommunication::InternalCloseCommunicationProcessing called ...\n");

  if (this->m_SocketType == SERVER_SOCKET)
    {
    this->InternalDisconnectConnectionSocketProcessing();
    }

  if( this->m_Socket != INVALID_SOCKET )
  {
    closesocket( this->m_Socket);
    this->m_Socket = INVALID_SOCKET;
  }

  this->m_CaptureFileStream.close();

  return SUCCESS; 

};

/** Internal function to write. */
SocketCommunication::ResultType 
SocketCommunication::InternalWriteProcessing( const char *data, unsigned int length ) 
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalWriteProcessing called ...\n");

  int total;
  int n;
  std::string encodedString;
  
  total = 0;
  do
    {
    switch (this->m_SocketType)
      {
      case SERVER_SOCKET:
        n = send( this->m_ConnectionSocket, data + total, length - total, 0);
        break;
      case CLIENT_SOCKET:
        n = send( this->m_Socket, data + total, length - total, 0);
        break;
      }
    
    if(n < 1)
      {
      igstkLogMacro( DEBUG, "SocketCommunication::InternalWriteProcessing: time out!\n");
      return TIMEOUT;
      }
    total += n;
    } while(total < length);

  igstk::BinaryData::Encode(encodedString, (unsigned char *)data,
                            length);

  // Recording for data sent
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    m_CaptureMessageNumber++;

    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                    << ". command[" << length << "] "
                    << encodedString << std::endl );
    }

  igstkLogMacro( DEBUG, "SocketCommunication::InternalWriteProcessing: write succeed! ...\n");
  return SUCCESS; 

};

/** Internal function to read. */
SocketCommunication::ResultType 
SocketCommunication::InternalReadProcessing( char * data, unsigned int length, unsigned int &total) 
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalReadProcessing called ...\n");

  int n;
  std::string encodedString;
  
  total = 0;
  do
    {
      switch (this->m_SocketType)
      {
      case SERVER_SOCKET:
        n = recv( this->m_ConnectionSocket, data + total, length - total, 0);
        break;
      case CLIENT_SOCKET:
        n = recv( this->m_Socket, data + total, length - total, 0);
        break;    
      }
    if (n < 1)
      {
      if (total == 0)
        {
        igstkLogMacro( DEBUG, "SocketCommunication::InternalReadProcessing: read time out!\n");
        return TIMEOUT;
        }
      else
        {
        igstkLogMacro( DEBUG, "SocketCommunication::InternalReadProcessing: read failured!\n");
        return FAILURE; 
        }
      }
    total += n;
    } while (total < length);

  // In case the data contains nulls or non-graphical characters,
  // encode it before logging it
  BinaryData::Encode(encodedString, (unsigned char*)data, total);

  // Recording for data received
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                    << ". receive[" << total << "] "
                    << encodedString << std::endl );
    }

  igstkLogMacro( DEBUG, "SocketCommunication::InternalReadProcessing: read succeed!\n");
  return SUCCESS; 

};

/** Internal function to disconnect connection socket. */
SocketCommunication::ResultType 
SocketCommunication::InternalDisconnectConnectionSocketProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::InternalDisconnectConnectionSocketProcessing called ...\n");

  if( this->m_ConnectionSocket != INVALID_SOCKET )
  {
    closesocket( this->m_ConnectionSocket);
    this->m_ConnectionSocket = INVALID_SOCKET;
  }

  return SUCCESS;
}

/** State function to attempt to open communication. */
void SocketCommunication::AttemptToOpenCommunicationProcessing( void )
{
  igstkLogMacro( DEBUG, "SocketCommunication::AttemptToOpenCommunicationProcessing called ...\n");

  this->m_StateMachine.PushInputBoolean( (bool)this->InternalOpenCommunicationProcessing(), 
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** State function to attempt to open port. */
void SocketCommunication::AttemptToOpenPortProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::AttemptToOpenPortProcessing called ...\n");

  this->m_StateMachine.PushInputBoolean( (bool)this->InternalOpenPortProcessing( this->m_PortToSend ), 
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** State function to attempt to connect. */
void SocketCommunication::AttemptToConnectProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::AttemptToConnectProcessing called ...\n");

  this->m_StateMachine.PushInputBoolean( (bool)this->InternalConnectProcessing( this->m_AddressToSend, this->m_PortToSend ), 
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** State function to close communication. */
void SocketCommunication::CloseCommunicationProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::CloseCommunicationProcessing called ...\n");

  this->m_ReturnValue = this->InternalCloseCommunicationProcessing();
}

/** State function to attempt to wait for connection. */
void SocketCommunication::AttemptToWaitForConnectionProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::AttemptToWaitForConnectionProcessing called ...\n");

  this->m_StateMachine.PushInputBoolean( (bool)this->InternalWaitForConnectionProcessing(), 
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** State function to attempt to query for connection. */
void SocketCommunication::AttemptToQueryForConnectionProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::AttemptToQueryForConnectionProcessing called ...\n");

  this->m_StateMachine.PushInputBoolean( (bool)(this->InternalQueryForConnectionProcessing( this->m_MillisecondToSend ) == SUCCESS), 
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** State function to write. */
void SocketCommunication::WriteProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::WriteProcessing called ...\n");

  this->m_ReturnValue = this->InternalWriteProcessing( this->m_BufferPointerToSend, this->m_NumberToSend );
}

/** State function to read. */
void SocketCommunication::ReadProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::ReadProcessing called ...\n");

  this->m_ReturnValue = this->InternalReadProcessing( this->m_BufferPointerToReceive, this->m_NumberToSend, this->m_NumberToReceive );
}

/** State function to disconnect connection socket. */
void SocketCommunication::DisconnectConnectionSocketProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::DisconnectConnectionSocketProcessing called ...\n");

  this->m_ReturnValue = this->InternalDisconnectConnectionSocketProcessing();
}

/** State function for no processing. */
void SocketCommunication::NoProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::NoProcessing called ...\n");
}

/** State function for success processing. */
void SocketCommunication::SuccessProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::SuccessProcessing called ...\n");

  this->m_ReturnValue = SUCCESS;
}

/** State function for failure processing. */
void SocketCommunication::FailureProcessing()
{
  igstkLogMacro( DEBUG, "SocketCommunication::FailureProcessing called ...\n");

  this->m_ReturnValue = FAILURE;
}

/** Function to open communication. */
SocketCommunication::ResultType
SocketCommunication::RequestOpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestOpenCommunication called ...\n");

  igstkPushInputMacro( OpenCommunication );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to close communication. */
SocketCommunication::ResultType
SocketCommunication::RequestCloseCommunication( void )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestCloseCommunication"
                 " called ...\n" );

  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to write. */
SocketCommunication::ResultType 
SocketCommunication::RequestWrite( const char *data, unsigned int numberOfBytes )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestWrite called ...\n");

  this->m_BufferPointerToSend = data;
  this->m_NumberToSend = numberOfBytes;

  igstkPushInputMacro( Write );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to read. */
SocketCommunication::ResultType 
SocketCommunication::RequestRead( char *data, unsigned int numberOfBytes,
                           unsigned int &bytesRead )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestRead called ...\n");

  this->m_BufferPointerToReceive = data;
  this->m_NumberToSend = numberOfBytes;

  igstkPushInputMacro( Read );
  m_StateMachine.ProcessInputs();

  bytesRead = this->m_NumberToReceive;

  return m_ReturnValue;

}

/** Function to connect. */
SocketCommunication::ResultType 
SocketCommunication::RequestConnect( SocketCommunication::AddressType address, 
                              SocketCommunication::PortNumberType port )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestConnect called ...\n");

  this->m_AddressToSend = address;
  this->m_PortToSend = port;

  igstkPushInputMacro( ClientConnect );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to open port. */
SocketCommunication::ResultType 
SocketCommunication::RequestOpenPort( SocketCommunication::PortNumberType port )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestOpenPort called ...\n");

  this->m_PortToSend = port;

  igstkPushInputMacro( ServerOpenPort );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to wait for connection. */
SocketCommunication::ResultType 
SocketCommunication::RequestWaitForConnection( )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestWaitForConnection called ...\n");

  igstkPushInputMacro( ServerWaitForConnection );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to query for connection. */
SocketCommunication::ResultType 
SocketCommunication::RequestQueryForConnection( unsigned int msec )
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestQueryForConnection called ...\n");

  this->m_MillisecondToSend = msec;

  igstkPushInputMacro( ServerQueryForConnection );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;

}

/** Function to disconnect connection socket. */
SocketCommunication::ResultType
SocketCommunication::RequestDisconnectConnectionSocket()
{
  igstkLogMacro( DEBUG, "SocketCommunication::RequestDisconnectConnectionSocket called ...\n");

  igstkPushInputMacro( ServerDisconnectConnectionSocket );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
}



} // end namespace igstk

