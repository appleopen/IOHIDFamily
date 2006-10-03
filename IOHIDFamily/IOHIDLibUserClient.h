/*
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 * Changes to this API are expected.
 */

#ifndef _IOKIT_IOHIDLibUserClient_H_
#define _IOKIT_IOHIDLibUserClient_H_

#include "IOHIDKeys.h"

// evil hack alert: we are using type in one of two ways:
// if type == 0, then we want to get the element values
// otherwise type is an object pointer (in kernel space).
// this was deemed better for now than duplicating
// the code from IOUserClient.cpp: is_io_connect_map_memory,
// mapClientMemory and is_io_connect_unmap_memory
enum IOHIDLibUserClientMemoryTypes {
    IOHIDLibUserClientElementValuesType = 0
};


enum IOHIDLibUserClientAsyncCommandCodes {
    kIOHIDLibUserClientSetAsyncPort,   		// kIOUCScalarIScalarO, 0, 0
    kIOHIDLibUserClientSetQueueAsyncPort,	// kIOUCScalarIScalarO, 1, 0
    kIOHIDLibUserClientAsyncGetReport,		// kIOUCScalarIScalarO, 5, 0
    kIOHIDLibUserClientAsyncSetReport,		// kIOUCScalarIScalarO, 5, 0

    kIOHIDLibUserClientNumAsyncCommands
};

enum IOHIDLibUserClientCommandCodes {
    kIOHIDLibUserClientOpen,			// kIOUCScalarIScalarO, 0, 0
    kIOHIDLibUserClientClose,			// kIOUCScalarIScalarO, 0, 0
    kIOHIDLibUserClientCreateQueue,		// kIOUCScalarIScalarO, 2, 1
    kIOHIDLibUserClientDisposeQueue,		// kIOUCScalarIScalarO, 1, 0
    kIOHIDLibUserClientAddElementToQueue,	// kIOUCScalarIScalarO, 3, 0
    kIOHIDLibUserClientRemoveElementFromQueue,	// kIOUCScalarIScalarO, 2, 0
    kIOHIDLibUserClientQueueHasElement, 	// kIOUCScalarIScalarO, 2, 1
    kIOHIDLibUserClientStartQueue, 		// kIOUCScalarIScalarO, 1, 0
    kIOHIDLibUserClientStopQueue, 		// kIOUCScalarIScalarO, 1, 0
    kIOHIDLibUserClientUpdateElementValue, 	// kIOUCScalarIScalarO, 1, 0
    kIOHIDLibUserClientPostElementValue,	// kIOUCStructIStructO, 0xffffffff, 0
    kIOHIDLibUserClientGetReport,		// kIOUCScalarIStructO, 2, 0xffffffff
    kIOHIDLibUserClientGetReportOOL,		// kIOUCStructIStructO, 
    kIOHIDLibUserClientSetReport,		// kIOUCScalarIScalarO, 2, 0xffffffff
    kIOHIDLibUserClientSetReportOOL,		// kIOUCStructIStructO,

    kIOHIDLibUserClientNumCommands
};

#if 0
struct IOHIDCommandExecuteData {
    HIDInfo HIDInfo;
    HIDResults *HIDResults;
	int kernelHandle;
	int sgEntries;
	UInt32 timeoutMS;
	IOVirtualRange sgList[0];
};

#define kIOHIDCommandExecuteDataMaxSize 1024

#endif

struct IOHIDElementValue
{
    IOHIDElementCookie cookie;
    UInt32             totalSize;
    AbsoluteTime       timestamp;
    UInt32             generation;
    UInt32             value[1];
};

struct IOHIDReportReq
{
    UInt32      reportType;
    UInt32		reportID;
    void 		*reportBuffer;
    UInt32		reportBufferSize;
};

#if KERNEL

#include <mach/mach_types.h>
#include <IOKit/IOUserClient.h>

class IOHIDDevice;
class IOSyncer;
#if 0
class IOCommandGate;

struct HIDResults;
#endif

class IOHIDLibUserClient : public IOUserClient 
{
    OSDeclareDefaultStructors(IOHIDLibUserClient)

protected:
    static const IOExternalMethod
		sMethods[kIOHIDLibUserClientNumCommands];
    static const IOExternalAsyncMethod
		sAsyncMethods[kIOHIDLibUserClientNumAsyncCommands];

    IOHIDDevice *fNub;
    IOCommandGate *fGate;
    
    OSSet * fQueueSet;

    task_t fClient;
    mach_port_t fWakePort;
    mach_port_t fQueuePort;
    
    bool fNubIsTerminated;
    
    IOOptionBits fCachedOptionBits;
    
    // Methods
    virtual bool initWithTask(task_t owningTask, void *security_id, UInt32 type);
    
    virtual IOReturn clientClose(void);
    IOReturn clientCloseGated();

    virtual bool start(IOService *provider);

    virtual IOExternalMethod * getTargetAndMethodForIndex(IOService **target, UInt32 index);

    virtual IOExternalAsyncMethod * getAsyncTargetAndMethodForIndex(IOService **target, UInt32 index);

    virtual IOReturn setAsyncPort(OSAsyncReference asyncRef,
                                  void *, void *, void *,
                                  void *, void *, void *);
                                  
    virtual IOReturn setQueueAsyncPort(OSAsyncReference asyncRef,
                                  void *vInQueue, void *, void *,
                                  void *, void *, void *);

    // Open the IOHIDDevice
    virtual IOReturn open(void * flags);
    IOReturn openGated(void * flags);

    // Close the IOHIDDevice
    virtual IOReturn close();
    IOReturn closeGated();
                   
    virtual bool didTerminate(IOService *provider, IOOptionBits options, bool *defer);
        
    virtual void free();

    // return the shared memory for type (called indirectly)
    virtual IOReturn clientMemoryForType(
                           UInt32                type,
                           IOOptionBits *        options,
                           IOMemoryDescriptor ** memory );
    IOReturn clientMemoryForTypeGated(
                           UInt32                type,
                           IOOptionBits *        options,
                           IOMemoryDescriptor ** memory );

    // Create a queue
    virtual IOReturn createQueue(void * vInFlags, void * vInDepth, void * vOutQueue);
    IOReturn createQueueGated(void * vInFlags, void * vInDepth, void * vOutQueue);

    // Dispose a queue
    virtual IOReturn disposeQueue(void * vInQueue);
    IOReturn disposeQueueGated(void * vInQueue);

    // Add an element to a queue
    virtual IOReturn addElementToQueue(void * vInQueue, void * vInElementCookie, void * vInFlags, void * vSizeChange);
    IOReturn addElementToQueueGated(void * vInQueue, void * vInElementCookie, void * vInFlags, void * vSizeChange);
   
    // remove an element from a queue
    virtual IOReturn removeElementFromQueue (void * vInQueue, void * vInElementCookie, void * vSizeChange);
    IOReturn removeElementFromQueueGated (void * vInQueue, void * vInElementCookie, void * vSizeChange);
    
    // Check to see if a queue has an element
    virtual IOReturn queueHasElement (void * vInQueue, void * vInElementCookie, void * vOutHasElement);
    IOReturn queueHasElementGated (void * vInQueue, void * vInElementCookie, void * vOutHasElement);
    
    // start a queue
    virtual IOReturn startQueue (void * vInQueue);
    IOReturn startQueueGated (void * vInQueue);
    
    // stop a queue
    virtual IOReturn stopQueue (void * vInQueue);
    IOReturn stopQueueGated (void * vInQueue);
                            
    // Update Feature element value
    virtual IOReturn updateElementValue (void *cookie);
    IOReturn updateElementValueGated (void *cookie);
                                                
    // Post element value
    virtual IOReturn postElementValue (void *cookie, void * cookieBytes);
    IOReturn postElementValueGated (void *cookie, void * cookieBytes);
                                                
    // Get report
    virtual IOReturn getReport(void *vReportType, void *vReportID, void *vReportBuffer, void *vReportBufferSize);
    IOReturn getReportGated (IOHIDReportType reportType, 
                                    UInt32 reportID, 
                                    void *reportBuffer, 
                                    UInt32 *reportBufferSize);
                                
    // Get report OOL
    virtual IOReturn getReportOOL ( void *vReqIn, void *vSizeOut, void * vInCount, void *vOutCount);
    IOReturn getReportOOLGated (IOHIDReportReq *reqIn, 
                                        UInt32 *sizeOut, 
                                        IOByteCount inCount, 
                                        IOByteCount *outCount);

    // Set report
    virtual IOReturn setReport (void *vReportType, void *vReportID, void *vReportBuffer, void *vReportBufferSize);
    IOReturn setReportGated (IOHIDReportType reportType, 
                                UInt32 reportID, 
                                void *reportBuffer, 
                                UInt32 reportBufferSize);
                                
    // Set report OOL
    virtual IOReturn setReportOOL (void *vReq, void *vInCount);
    IOReturn setReportOOLGated (IOHIDReportReq *req, IOByteCount inCount);


    // Asyn get report
    virtual IOReturn asyncGetReport (OSAsyncReference asyncRef, 
                                    IOHIDReportType reportType, 
                                    UInt32 reportID, 
                                    void *reportBuffer,
                                    UInt32 reportBufferSize, 
                                    UInt32 completionTimeOutMS);
    IOReturn asyncGetReportGated (void * param);
                                    
    // Asyn set report
    virtual IOReturn asyncSetReport (OSAsyncReference asyncRef, 
                                    IOHIDReportType reportType, 
                                    UInt32 reportID, 
                                    void *reportBuffer,
                                    UInt32 reportBufferSize, 
                                    UInt32 completionTimeOutMS);
    IOReturn asyncSetReportGated (void * param);

    void ReqComplete(void *param, IOReturn status, UInt32 remaining);
    void ReqCompleteGated(void *param, IOReturn status, UInt32 remaining);
};

#endif /* KERNEL */

#endif /* ! _IOKIT_IOHIDLibUserClient_H_ */

