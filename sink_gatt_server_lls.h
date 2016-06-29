/*******************************************************************************
Copyright Cambridge Silicon Radio Limited 2014 - 2015.
Part of ADK 4.0

FILE NAME
    sink_gatt_server_lls.h

DESCRIPTION
    Routines to handle messages sent from the GATT Link Loss Server Task.
    
NOTES

*/


#ifndef _SINK_GATT_SERVER_LLS_H_
#define _SINK_GATT_SERVER_LLS_H_


#ifndef GATT_ENABLED
#undef GATT_LLS_SERVER
#endif


/* Firmware headers */
#include <csrtypes.h>
#include <message.h>

/* Library headers */
#include <gatt_link_loss_server.h>

#ifdef GATT_LLS_SERVER
#define sinkGattLinkLossServerGetSize() sizeof(GLLSS_T)
#else
#define sinkGattLinkLossServerGetSize() 0
#endif

#ifdef GATT_LLS_SERVER
#define sinkGattLinkLossAlertHigh(delay_s) MessageSendLater(&theSink.task, EventSysLlsAlertHigh, 0, D_SEC(delay_s))
#define sinkGattLinkLossAlertMild(delay_s) MessageSendLater(&theSink.task, EventSysLlsAlertMild, 0, D_SEC(delay_s))
#else
#define sinkGattLinkLossAlertHigh(delay_s) ((void)0)
#define sinkGattLinkLossAlertMild(delay_s) ((void)0)
#endif

/*******************************************************************************
NAME
    sinkGattLinkLossAlertStop
    
DESCRIPTION
    This function stops alerting link loss
    
PARAMETERS
    void
    
RETURNS
    void
*/
#ifdef GATT_LLS_SERVER
void sinkGattLinkLossAlertStop(void);
#else
#define sinkGattLinkLossAlertStop() ((void)(0))
#endif

/*******************************************************************************
NAME
    sinkGattLinkLossServerInitialiseTask
    
DESCRIPTION
    Initialise the Link Loss server task.
    NOTE: This function will modify *ptr.
    
PARAMETERS
    ptr - pointer to allocated memory to store server tasks rundata.

    
RETURNS
    TRUE if the Link Loss server task was initialised, FALSE otherwise.
*/
#ifdef GATT_LLS_SERVER
bool sinkGattLinkLossServerInitialiseTask(uint16 **ptr);
#else
#define sinkGattLinkLossServerInitialiseTask(ptr) (TRUE)
#endif

/*******************************************************************************
NAME
    sinkHandleLinkLossInd
    
DESCRIPTION
    Handle Link Loss indication to alter if required
    
PARAMETERS
    cid      Connection Id for the link
    
RETURNS
    void
*/
#ifdef GATT_LLS_SERVER
void sinkGattHandleLinkLossInd(uint16 cid);
#else
#define sinkGattHandleLinkLossInd(cid) ((void)(0))
#endif

/*******************************************************************************
NAME
    sinkHandleLinkUpInd
    
DESCRIPTION
    Handle Link Up indication to stop alter
    
PARAMETERS
    cid      Connection Id for the link
    
RETURNS
    void
*/
#ifdef GATT_LLS_SERVER
void sinkGattHandleLinkUpInd(uint16 cid);
#else
#define sinkGattHandleLinkUpInd(cid) ((void)(0))
#endif

/*******************************************************************************
NAME
    sinkGattLinkLossServerMsgHandler
    
DESCRIPTION
    Handle messages from the GATT Link Loss Server library
    
PARAMETERS
    task    The task the message is delivered
    id      The ID for the GATT message
    payload The message payload
    
RETURNS
    void
*/
#ifdef GATT_LLS_SERVER
void sinkGattLinkLossServerMsgHandler(Task task, MessageId id, Message message);
#else
#define sinkGattLinkLossServerMsgHandler(task, id, message) ((void)(0))
#endif

#endif /* _SINK_GATT_SERVER_LLS_H_ */
