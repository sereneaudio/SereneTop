/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2005 - 2015.

FILE NAME
    sink_gatt_client_battery.c

DESCRIPTION
    Routines to handle the GATT Battery Service.
*/

#include "sink_gatt_client_battery.h"

#include "sink_debug.h"
#include "sink_development.h"
#include "sink_gatt_client.h"
#include "sink_private.h"
#include "sink_gap_ad_types.h"


#include <stdlib.h>
#include <connection.h>
#include <gatt.h>


#ifdef GATT_BATTERY_CLIENT


#ifdef DEBUG_GATT_BATTERY_CLIENT
#define GATT_BATTERY_CLIENT_INFO(x) DEBUG(x)
#define GATT_BATTERY_CLIENT_ERROR(x) DEBUG(x) TOLERATED_ERROR(x)
#else
#define GATT_BATTERY_CLIENT_INFO(x)
#define GATT_BATTERY_CLIENT_ERROR(x)
#endif

static const uint8 bas_ble_advertising_filter[] = {GATT_SERVICE_UUID_BATTERY_SERVICE & 0xFF, GATT_SERVICE_UUID_BATTERY_SERVICE >> 8};

/*******************************************************************************
NAME
    gattBatteryClientSetCachedLevel
    
DESCRIPTION
    Sets the cached remote battery level.
    
PARAMETERS
    level    The battery level to cache
    
*/
static void gattBatteryClientSetCachedLevel(uint8 level)
{
    GATT_BATTERY_CLIENT_INFO(("GATT Battery RC level cache=[%u]\n", level));
    GATT_CLIENT.cache.basc_remote_level = level;
}

/*******************************************************************************
NAME
    gattBatteryClientFindConnection
    
DESCRIPTION
    Finds a client connection associated with a battery instance.
    
PARAMETERS
    gbasc    The battery client instance pointer
    
RETURNS    
    The client connection pointer associated with the battery instance. NULL if not found.    
    
*/
static gatt_client_connection_t *gattBatteryClientFindConnection(const GBASC *gbasc)
{
    uint16 index = 0;
    gatt_client_services_t *data = NULL;
    
    if (gbasc == NULL)
    {
        return NULL;
    }
    
    for (index = 0; index < GATT_CLIENT.number_connections; index++)
    {
        data = gattClientGetServiceData(&GATT_CLIENT.connection[index]);
        if (data && (data->basc == gbasc))
        {
            return &GATT_CLIENT.connection[index];
        }
    }
    
    return NULL;
}

/*******************************************************************************
NAME
    batteryServiceInitialised
    
DESCRIPTION
    Called when the battery service is initialised.
    
PARAMETERS
    gbasc    The battery client instance pointer
    
*/
static void batteryServiceInitialised(const GBASC *gbasc)
{    
    gatt_client_connection_t *conn = gattBatteryClientFindConnection(gbasc);
    
    if (conn != NULL)
    {
        gattClientDiscoveredServiceInitialised(conn);
    }
    else
    {
        gattClientDiscoveryError(NULL);
    }
}

/*******************************************************************************
NAME
    gattBatteryInitCfm
    
DESCRIPTION
    Handle the GATT_BATTERY_CLIENT_INIT_CFM message.
    
PARAMETERS
    cfm    The GATT_BATTERY_CLIENT_INIT_CFM message
    
*/
static void gattBatteryInitCfm(const GATT_BATTERY_CLIENT_INIT_CFM_T *cfm)
{
    GATT_BATTERY_CLIENT_INFO(("GATT_BATTERY_CLIENT_INIT_CFM status[%u]\n",
                                       cfm->status));
    
    if ((cfm->status == gatt_battery_client_status_success) &&
        (gattBatteryClientFindConnection(cfm->battery_client) != NULL))
    {
        /* Read battery level */
        GattBatteryClientReadLevelRequest(cfm->battery_client);
        /* Set notifications */
        GattBatteryClientSetNotificationEnableRequest(cfm->battery_client, TRUE);
    }
    else
    {
        /* The service initialisation is complete */
        batteryServiceInitialised(cfm->battery_client);
    }
}

/*******************************************************************************
NAME
    gattBatteryReadLevelCfm
    
DESCRIPTION
    Handle the GATT_BATTERY_CLIENT_READ_LEVEL_CFM message.
    
PARAMETERS
    cfm    The GATT_BATTERY_CLIENT_READ_LEVEL_CFM message
    
*/
static void gattBatteryReadLevelCfm(const GATT_BATTERY_CLIENT_READ_LEVEL_CFM_T *cfm)
{
    GATT_BATTERY_CLIENT_INFO(("GATT_BATTERY_CLIENT_READ_LEVEL_CFM status[%u] level[%u]\n", 
                   cfm->status,
                   cfm->battery_level));
    
    if (cfm->status == gatt_battery_client_status_success)
    {
        gattBatteryClientSetCachedLevel(cfm->battery_level);
    }
}

/*******************************************************************************
NAME
    gattBatteryReadDescriptorCfm
    
DESCRIPTION
    Handle the GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM message.
    
PARAMETERS
    cfm    The GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM message
    
*/
static void gattBatteryReadDescriptorCfm(const GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM_T *cfm)
{
#ifdef DEBUG_GATT_BATTERY_CLIENT            
    uint16 i = 0;
    GATT_BATTERY_CLIENT_INFO(("GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM status[%u] uuid[%u] size[%u]\n", 
           cfm->status,
           cfm->descriptor_uuid,
           cfm->size_value));
    for (i = 0; i < cfm->size_value; i++)
    {
        GATT_BATTERY_CLIENT_INFO(("[0x%x]", cfm->value[i]));
    }
    GATT_BATTERY_CLIENT_INFO(("\n]"));
#endif    
}

/*******************************************************************************
NAME
    gattBatterySetNotificationEnableCfm
    
DESCRIPTION
    Handle the GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM message.
    
PARAMETERS
    cfm    The GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM message
    
*/
static void gattBatterySetNotificationEnableCfm(const GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM_T *cfm)
{
    GATT_BATTERY_CLIENT_INFO(("GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM status[%u]\n", cfm->status));
    
    if (gattBatteryClientFindConnection(cfm->battery_client) != NULL)
    {
        /* Read CCD for Battery level */
        GattBatteryClientReadDescriptorRequest(cfm->battery_client, GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_UUID);
    }
    /* The service is initialised once notifications have been set */
    batteryServiceInitialised(cfm->battery_client);
}

/*******************************************************************************
NAME
    gattBatteryLevelInd
    
DESCRIPTION
    Handle the GATT_BATTERY_CLIENT_LEVEL_IND message.
    
PARAMETERS
    ind    The GATT_BATTERY_CLIENT_LEVEL_IND message
    
*/
static void gattBatteryLevelInd(const GATT_BATTERY_CLIENT_LEVEL_IND_T *ind)
{
    GATT_BATTERY_CLIENT_INFO(("GATT_BATTERY_CLIENT_LEVEL_IND level[%u]\n", ind->battery_level));
    
    gattBatteryClientSetCachedLevel(ind->battery_level);
}


/****************************************************************************/
void gattBatteryClientSetupAdvertisingFilter(void)
{
    GATT_BATTERY_CLIENT_INFO(("GattBas: Add BAS advertising filter\n"));
    ConnectionBleAddAdvertisingReportFilter(AD_TYPE_SERVICE_UUID_16BIT_LIST, sizeof(bas_ble_advertising_filter), sizeof(bas_ble_advertising_filter), bas_ble_advertising_filter);
}

/****************************************************************************/
void gattBatteryClientInit(void)
{
    /* Initialise cached values */
    GATT_CLIENT.cache.basc_remote_level = BATTERY_LEVEL_INVALID;
}

    
/****************************************************************************/
bool gattBatteryClientAddService(uint16 cid, uint16 start, uint16 end)
{
    GBASC *gbasc = NULL;
    gatt_client_services_t *client_services = NULL;
    gatt_client_connection_t *connection = gattClientFindByCid(cid);
    uint16 *service = gattClientAddService(connection, sizeof(GBASC));
    
    if (service)
    {
        client_services = gattClientGetServiceData(connection);
        client_services->basc = (GBASC *)service;
        gbasc = client_services->basc;
        if (GattBatteryClientInit(gbasc, sinkGetBleTask(), cid, start, end, NULL) == gatt_battery_client_status_success)
        {            
            return TRUE;
        }
    }
    
    return FALSE;
}


/****************************************************************************/
void gattBatteryClientRemoveService(GBASC *gbasc, uint16 cid)
{
    GattBatteryClientDestroy(gbasc, cid);
}


/******************************************************************************/
void gattBatteryClientMsgHandler(Task task, MessageId id, Message message)
{
    switch(id)
    {
        case GATT_BATTERY_CLIENT_INIT_CFM:
        {
            gattBatteryInitCfm((GATT_BATTERY_CLIENT_INIT_CFM_T*)message);
        }
        break;
        case GATT_BATTERY_CLIENT_READ_LEVEL_CFM:
        {
            gattBatteryReadLevelCfm((GATT_BATTERY_CLIENT_READ_LEVEL_CFM_T*)message);
        }
        break;
        case GATT_BATTERY_CLIENT_LEVEL_IND:
        {
            gattBatteryLevelInd((GATT_BATTERY_CLIENT_LEVEL_IND_T*)message);
        }
        break;
        case GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM:
        {
            gattBatteryReadDescriptorCfm((GATT_BATTERY_CLIENT_READ_DESCRIPTOR_CFM_T*)message);        
        }
        break;
        case GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM:
        {            
            gattBatterySetNotificationEnableCfm((GATT_BATTERY_CLIENT_SET_NOTIFICATION_ENABLE_CFM_T*)message);
        }
        break;
        default:
        {
            GATT_BATTERY_CLIENT_ERROR(("Unhandled BASC msg[%x]\n", id));
        }
        break;
    }
}


/****************************************************************************/
uint8 gattBatteryClientGetCachedLevel(void)
{
    return GATT_CLIENT.cache.basc_remote_level;
}



#endif /* GATT_BATTERY_CLIENT */

