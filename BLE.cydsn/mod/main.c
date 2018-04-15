/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  BLE HID keyboard example project that supports both input and output reports
*  in boot and protocol mode. The example also demonstrates handling suspend 
*  event from the central device and enters low power mode when suspended.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*  HID Usage Tables spec ver 1.12
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/



#include "common.h"
#include "hids.h"
#include "bas.h"
#include "scps.h"
#include "../c2/c2_protocol.h"

volatile uint32 mainTimer = 0;


void AppCallBack(uint32 event, void* eventParam)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_BD_ADDR_T localAddr;
    CYBLE_GAP_AUTH_INFO_T *authInfo;
    uint8 i;
    
    switch (event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
		case CYBLE_EVT_STACK_ON: /* This event is received when the component is Started */
            /* Enter into discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
            DBG_PRINTF("Bluetooth On, StartAdvertisement with addr: ");
            localAddr.type = 0u;
            CyBle_GetDeviceAddress(&localAddr);
            for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
            {
                DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
            }
            DBG_PRINTF("\r\n");
            break;
		case CYBLE_EVT_TIMEOUT: 
            break;
		case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR \r\n");
			break;
            
    	/* This event will be triggered by host stack if BLE stack is busy or not busy.
    	 *  Parameter corresponding to this event will be the state of BLE stack.
    	 *  BLE stack busy = CYBLE_STACK_STATE_BUSY,
    	 *  BLE stack not busy = CYBLE_STACK_STATE_FREE 
         */
    	case CYBLE_EVT_STACK_BUSY_STATUS:
            DBG_PRINTF("CYBLE_EVT_STACK_BUSY_STATUS: %x\r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_HCI_STATUS:
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS: %x \r\n", *(uint8 *)eventParam);
			break;
            
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAP_AUTH_REQ:
            DBG_PRINTF("CYBLE_EVT_AUTH_REQ: security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr);
            break;
        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_PASSKEY_ENTRY_REQUEST press 'p' to enter passkey \r\n");
            break;
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_PASSKEY_DISPLAY_REQUEST %6.6ld \r\n", *(uint32 *)eventParam);
            break;
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT \r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;
            (void)authInfo;
            DBG_PRINTF("AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n", 
                                    authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINTF("CYBLE_EVT_AUTH_FAILED: %x \r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_ADVERTISING, state: %x \r\n", CyBle_GetState());
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                DBG_PRINTF("Hibernate \r\n");
                LED_RED_Write(LED_OFF);
                LED_GRN_Write(LED_ON);
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
            #if (DEBUG_UART_ENABLED == ENABLED)
                /* Wait until debug info is sent */
                while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                CySysPmHibernate();
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED \r\n");
            LED_RED_Write(LED_OFF);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
            break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            { 
                uint16 mtu;
                CyBle_GattGetMtuSize(&mtu);
                DBG_PRINTF("CYBLE_EVT_GATTS_XCNHG_MTU_REQ, final mtu= %d \r\n", mtu);
            }
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATT_WRITE_REQ: %x = ",((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle);
            ShowValue(&((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value);
            (void)CyBle_GattsWriteRsp(((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->connHandle);
            break;
        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_CONNECTION_UPDATE_COMPLETE: %x \r\n", *(uint8 *)eventParam);
            break;
            
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: %x, %x \r\n", cyBle_connHandle.attId, cyBle_connHandle.bdHandle);
            /* Register service specific callback functions */
            HidsInit();
            BasInit();
            ScpsInit();
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND \r\n");
            break;
        case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            /* Triggered on server side when client sends read request and when
            * characteristic has CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
            * This event could be ignored by application unless it need to response
            * by error response which needs to be set in gattErrorCode field of
            * event parameter. */
            DBG_PRINTF("CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: handle: %x \r\n", 
                ((CYBLE_GATTS_CHAR_VAL_READ_REQ_T *)eventParam)->attrHandle);
            break;
            
        /**********************************************************
        *                       Other Events
        ***********************************************************/
		case CYBLE_EVT_PENDING_FLASH_WRITE:
            /* Inform application that flash write is pending. Stack internal data 
            * structures are modified and require to be stored in Flash using 
            * CyBle_StoreBondingData() */
            DBG_PRINTF("CYBLE_EVT_PENDING_FLASH_WRITE\r\n");
            break;

        default:
            DBG_PRINTF("OTHER event: %lx \r\n", event);
			break;
	}

}


/*******************************************************************************
* Function Name: LowPowerImplementation()
********************************************************************************
* Summary:
* Implements low power in the project.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* The function tries to enter deep sleep as much as possible - whenever the 
* BLE is idle and the UART transmission/reception is not happening. At all other
* times, the function tries to enter CPU sleep.
*
*******************************************************************************/
static void LowPowerImplementation(void)
{
    CYBLE_LP_MODE_T bleMode;
    uint8 interruptStatus;
    
    /* For advertising and connected states, implement deep sleep 
     * functionality to achieve low power in the system. For more details
     * on the low power implementation, refer to the Low Power Application 
     * Note.
     */
    if((CyBle_GetState() == CYBLE_STATE_ADVERTISING) || 
       (CyBle_GetState() == CYBLE_STATE_CONNECTED))
    {
        /* Request BLE subsystem to enter into Deep-Sleep mode between connection and advertising intervals */
        bleMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        /* Disable global interrupts */
        interruptStatus = CyEnterCriticalSection();
        /* When BLE subsystem has been put into Deep-Sleep mode */
        if(bleMode == CYBLE_BLESS_DEEPSLEEP)
        {
            /* And it is still there or ECO is on */
            if((CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_ON) || 
               (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_DEEPSLEEP))
            {
            #if (DEBUG_UART_ENABLED == ENABLED)
                /* Put the CPU into the Deep-Sleep mode when all debug information has been sent */
                if((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u)
                {
                    CySysPmDeepSleep();
                }
                else /* Put the CPU into Sleep mode and let SCB to continue sending debug data */
                {
                    CySysPmSleep();
                }
            #else
                CySysPmDeepSleep();
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            }
        }
        else /* When BLE subsystem has been put into Sleep mode or is active */
        {
            /* And hardware doesn't finish Tx/Rx opeation - put the CPU into Sleep mode */
            if(CyBle_GetBleSsState() != CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                CySysPmSleep();
            }
        }
        /* Enable global interrupt */
        CyExitCriticalSection(interruptStatus);
    }
}

void keyboard_press(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_report.mods |= (1 << (keycode & 0x07));
    return;
  }
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_report.keys; cur_pos++) {
    if (keyboard_report.keys[cur_pos] == keycode) {
      DBG_PRINTF("Existing %d pos %d", keycode, cur_pos);
      return;
    } else if (keyboard_report.keys[cur_pos] == 0) {
      keyboard_report.keys[cur_pos] = keycode;
      keyboard_report_usage = cur_pos + 1;
      return;
    }
  }
}

// Very similar to consumer_release, but keycode there is uint16_t :(
void keyboard_release(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_report.mods &= ~(1 << (keycode & 0x07));
    return;
  }
  bool move = false;
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_report.keys; cur_pos++) {
    if (move) {
      keyboard_report.keys[cur_pos - 1] = keyboard_report.keys[cur_pos];
    } else if (keyboard_report.keys[cur_pos] == keycode) {
      move = true;
    }
  }
  if (move) {
    // Key was, in fact, pressed.
    keyboard_report.keys[sizeof keyboard_report.keys - 1] = 0;
    keyboard_report_usage--;
    // xprintf("Released %d, usage %d", keycode, keyboard_report_usage);
  }
}


void process_inbox(Sup_Pdu_t *inbox) {
  switch (inbox->command) {
    case SUP_CMD_KEYDOWN:
      keyboard_press(inbox->data);
    case SUP_CMD_KEYUP:
      keyboard_release(inbox->data);
    case SUP_CMD_NOOP:
      DBG_PRINTF("Received NOOP");
    default:
      DBG_PRINTF("Unknown command: %d", inbox->command);
  }
  return;
  uint8_t apiResult;
  if (CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_FREE) {
      
      apiResult = CyBle_HidssGetCharacteristicValue(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
          CYBLE_HIDS_PROTOCOL_MODE, sizeof(protocol), &protocol);
      if(apiResult == CYBLE_ERROR_OK)
      {
          
          if(protocol == CYBLE_HIDS_PROTOCOL_MODE_BOOT)
          {
              apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX,
                  CYBLE_HIDS_BOOT_KYBRD_IN_REP, KEYBOARD_DATA_SIZE, keyboard_report.raw);
          }
          else
          {
              apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
                  CYBLE_HUMAN_INTERFACE_DEVICE_REPORT_IN, KEYBOARD_DATA_SIZE, keyboard_report.raw);
          }
          
          if(apiResult != CYBLE_ERROR_OK)
          {
              DBG_PRINTF("HID notification API Error: %x \r\n", apiResult);
          }
      }
  }


}


int main()
{
    CyGlobalIntEnable;  
    SCB_Start();

    UART_DEB_Start();
    DBG_PRINTF("BLE HID Keyboard Example Project \r\n");
    LED_RED_Write(LED_OFF);
    LED_GRN_Write(LED_OFF);
    LED_BLU_Write(LED_OFF);

    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);
    Sup_Pdu_t i2c_inbox;
    while(1) 
    {           
      SCB_I2CMasterClearStatus();
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();
/*
      uint8_t buf;
      uint32_t result = SCB_I2CMasterReadBuf(8, &buf, 1, SCB_I2C_MODE_COMPLETE_XFER);
      if (0 == (result & SCB_I2C_MSTR_NOT_READY)) {
        uint32_t cnt = 100;
        while (--cnt && 0 == (SCB_I2CMasterStatus() & SCB_I2C_MSTAT_RD_CMPLT)) {}
        if (!cnt) {
          uint32_t tmp = SCB_I2CMasterStatus();
          DBG_PRINTF("stuck in I2C RX: %d\r\n", tmp);
        } else {
          if (0 == (SCB_I2CMasterStatus() & SCB_I2C_MSTAT_ERR_MASK) && buf != 0xff) {
            i2c_inbox.command = buf;
            result = SCB_I2C_MSTR_NOT_READY;
            while (result & SCB_I2C_MSTR_NOT_READY) {
              result = SCB_I2CMasterReadBuf(8, &buf, 1, SCB_I2C_MODE_COMPLETE_XFER);
            }
            while (0 == (SCB_I2CMasterStatus() & SCB_I2C_MSTAT_RD_CMPLT)) {}
            if (0 == (SCB_I2CMasterStatus() & SCB_I2C_MSTAT_ERR_MASK)) {
              i2c_inbox.data = buf;
              process_inbox(&i2c_inbox);
            }
          }
        }
      }
*/
        /* To achieve low power in the device */
        LowPowerImplementation();

        if((CyBle_GetState() == CYBLE_STATE_CONNECTED) && (suspend != CYBLE_HIDS_CP_SUSPEND))
        {
            if(mainTimer != 0u)
            {
                mainTimer = 0u;                
            }
            /* Store bonding data to flash only when all debug information has been sent */
        #if(CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
        #if (DEBUG_UART_ENABLED == ENABLED)
            if((cyBle_pendingFlashWrite != 0u) &&
               ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
            
        #else
            if(cyBle_pendingFlashWrite != 0u)
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            {
                CYBLE_API_RESULT_T apiResult;
                
                apiResult = CyBle_StoreBondingData(0u);
                (void)apiResult;
                DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
            }
        #endif /* CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES */
        
        }
	}   
}  


/* [] END OF FILE */

