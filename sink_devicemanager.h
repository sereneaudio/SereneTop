/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004 - 2015.

FILE NAME
    sink_devicemanager.h
    
DESCRIPTION
    
*/

#ifndef _SINK_DEVICEMANAGER_H_
#define _SINK_DEVICEMANAGER_H_

#include <connection.h>

#include "sink_configmanager.h"
#include "sink_gatt_client.h"


/* Link types the sink device supports */
typedef enum
{
    sink_none  = 0x00,
    sink_hfp   = 0x01,
    sink_a2dp  = 0x02,
    sink_avrcp = 0x04,
    sink_swat  = 0x08
    
}sink_link_type;

/* Type of devices known by the device manager */
typedef enum
{
    dev_type_none   = 0x0,
    dev_type_ag     = 0x1,  /* Audio Gateway devices */
    dev_type_sub    = 0x2   /* Subwoofer devices */
    
}sink_remote_device_type;

/* HFP attributes */
typedef struct
{
    uint8 volume;
} hfp_attributes;

/* A2DP attributes */
typedef struct
{
    uint8  volume;
    uint16 clock_mismatch;
    uint16 audio_enhancements;
} a2dp_attributes;

/* sub woofer attributes */
typedef struct
{
    uint8 sub_trim_idx;    
} sub_attributes;


/* All device attributes stored in PS */
typedef struct
{
    remote_features             peer_features:6;    /* ENABLE_PEER use only */
    remote_device               peer_device:2;      /* ENABLE_PEER use only */    
    sink_link_type              profiles:8;
    hfp_attributes              hfp;
    a2dp_attributes             a2dp;
    sub_attributes              sub;                /* ENABLE_SUBWOOFER use only */
    gatt_client_attributes_t    gatt_client;        /* GATT_ENABLED use only */
} sink_attributes;


typedef struct
{
    bdaddr                      bd_addr;
    sink_attributes             attributes;
    sink_remote_device_type     device_type;
} EVENT_UPDATE_ATTRIBUTES_T;


/* Connection mask */
typedef enum
{
    conn_hfp_pri  = 1 << 0,
    conn_hfp_sec  = 1 << 1,
    conn_a2dp_pri = 1 << 2,
    conn_a2dp_sec = 1 << 3
} conn_mask;

#define conn_hfp  (conn_hfp_pri | conn_hfp_sec)
#define conn_a2dp (conn_a2dp_pri | conn_a2dp_sec)


/****************************************************************************
NAME
    deviceManagerUpdateAttributesWithDeviceDefaults
    
DESCRIPTION
    Updates the attributes with the defaults for a specific device type.

RETURNS
    void
*/
void deviceManagerUpdateAttributesWithDeviceDefaults(sink_attributes* attributes, sink_remote_device_type device_type);


/****************************************************************************
NAME    
    deviceManagerClearAttributes
    
DESCRIPTION
    Clear sink_attributes struct

RETURNS
    void
*/
void deviceManagerClearAttributes(sink_attributes* attributes);


/****************************************************************************
NAME    
    deviceManagerGetDefaultAttributes
    
DESCRIPTION
    Get the default attributes for a specific device type. 
    All attributes not related to the device type will be cleared.

RETURNS
    void
*/
void deviceManagerGetDefaultAttributes(sink_attributes* attributes, sink_remote_device_type device_type);


/****************************************************************************
NAME    
    deviceManagerGetAttributes
    
DESCRIPTION
    Get the attributes for a given device, or check device is in PDL if 
    attributes is NULL.

RETURNS
    TRUE if device in PDL, otherwise FALSE
*/
bool deviceManagerGetAttributes(sink_attributes* attributes, const bdaddr* dev_addr);


/****************************************************************************
NAME    
    deviceManagerGetIndexedAttributes
    
DESCRIPTION
    Get the attributes for a given index of device

RETURNS
    TRUE if successful, otherwise FALSE
*/
bool deviceManagerGetIndexedAttributes(uint8 index, sink_attributes* attributes, typed_bdaddr* dev_addr);

/****************************************************************************
NAME
    deviceManagerGetAddrFromProfile

DESCRIPTION
    Get the attributes for a specific device profile

RETURNS
    TRUE if device profile is in PDL, otherwise FALSE
*/
bool deviceManagerGetAddrFromProfile(sink_link_type profile, typed_bdaddr* dev_addr);
        
/****************************************************************************
NAME
    deviceManagerStoreAttributes
    
DESCRIPTION
    Stores given attribute values against a given device in PS.

RETURNS
    void
*/
void deviceManagerStoreAttributes(sink_attributes* attributes, const bdaddr* dev_addr);


/****************************************************************************
NAME
    deviceManagerStoreDefaultAttributes
    
DESCRIPTION
    Stores the default attributes against a given device in PS.

RETURNS
    void
*/
void deviceManagerStoreDefaultAttributes(const bdaddr* dev_addr, sink_remote_device_type device_type);


/****************************************************************************
NAME
    deviceManagerUpdateAttributes
    
DESCRIPTION
    Stores the current attribute values for a given HFP/A2DP connection in
    PS.

RETURNS
    void
*/
void deviceManagerUpdateAttributes(const bdaddr* bd_addr, sink_link_type link_type, hfp_link_priority hfp_priority, a2dp_link_priority a2dp_priority);

/****************************************************************************
NAME
    deviceManagerMarkTrusted
    
DESCRIPTION
     Sets a device as trusted

RETURNS
    void
*/
void deviceManagerMarkTrusted(const bdaddr* dev_addr);

/****************************************************************************
NAME
    deviceManagerDelayedUpdateAttributes
    
DESCRIPTION
    Store attributes contained in EVENT_UPDATE_ATTRIBUTES_T in PS

RETURNS
    void
*/
void deviceManagerDelayedUpdateAttributes(EVENT_UPDATE_ATTRIBUTES_T* update);


/****************************************************************************
NAME
    deviceManagerSetPriority
    
DESCRIPTION
    Set a device's priority in the PDL

RETURNS
    void
*/
uint8 deviceManagerSetPriority(const bdaddr* dev_addr);

/****************************************************************************
 NAME
 deviceManagerUpdatePriorityDevices

 DESCRIPTION
 Forces a type of device on top of the PDL

RETURNS
    void
*/
void deviceManagerUpdatePriorityDevices(void);

/****************************************************************************
NAME    
    deviceManagerRemoveDevice
    
DESCRIPTION
    Remove given device from the PDL

RETURNS
    void
*/
#define deviceManagerRemoveDevice(dev_addr) ConnectionSmDeleteAuthDevice(dev_addr)


/****************************************************************************
NAME    
    deviceManagerRemoveAllDevices
    
DESCRIPTION
    Remove all devices from the PDL

RETURNS
    void
*/
void deviceManagerRemoveAllDevices(void);


/****************************************************************************
NAME    
    deviceManagerProfilesConnected
    
DESCRIPTION
    compare passed in bdaddr against those of the current connected devices,
    if a match is found returns true

RETURNS
    TRUE or FALSE
*/
conn_mask deviceManagerProfilesConnected(const bdaddr * bd_addr);


/****************************************************************************
NAME    
    deviceManagerNumConnectedDevs
    
DESCRIPTION
   determines the number of different connected devices, a device may connected
   both hfp and a2dp or only one of each
RETURNS
    number of connected devices
*/
uint8 deviceManagerNumConnectedDevs(void);


/****************************************************************************
NAME    
    deviceManagerNumConnectedPeerDevs
    
DESCRIPTION
   determines the number of different connected peer devices, a device will only
   have connected a2dp
RETURNS
    number of connected peer devices
*/
uint8 deviceManagerNumConnectedPeerDevs(void);


/****************************************************************************
NAME    
    deviceManagerCanConnect
    
DESCRIPTION
   Determines if max number of possible connections have been made
RETURNS
   TRUE if connection can be made, FALSE otherwise
*/
bool deviceManagerCanConnect(void);

/****************************************************************************
NAME    
    deviceManagerIsSameDevice
    
DESCRIPTION
    Determines if the supplied HF and AV devices are actually one and the same

RETURNS
    TRUE if the devices are the same, FALSE otherwise
*/
bool deviceManagerIsSameDevice(a2dp_link_priority a2dp_link, hfp_link_priority hfp_link);


/****************************************************************************
NAME    
    deviceManagerDeviceDisconnectedInd
    
DESCRIPTION
    Finds which device has been disconnected(primary or secondary) and sends the appropriate event to notify the same

RETURNS
    void
*/
void deviceManagerDeviceDisconnectedInd(const bdaddr* dev_addr);


/****************************************************************************
NAME    
    deviceManagerGetAttributeHfpVolume
    
DESCRIPTION
    Gets the HFP volume attribute associated with a paired device.
    The address of the paired device must be supplied as ag_addr.
    If the function returns TRUE, hfp_vol will contain the attribute value.

RETURNS
    TRUE if the HFP volume attribute was found, FALSE otherwise.
*/
bool deviceManagerGetAttributeHfpVolume(const bdaddr *ag_addr, uint16 *hfp_vol);


#endif /* _SINK_DEVICEMANAGER_H_ */

