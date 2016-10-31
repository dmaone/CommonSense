/***************************************************************************//**
* \file USB_cdc.c
* \version 3.10
*
* \brief
*  This file contains the USB CDC class request handler.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* \copyright
* Copyright 2012-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_cdc.h"
#include "USB_pvt.h"
#include "cyapicallbacks.h"

#if defined(USB_ENABLE_CDC_CLASS)

/*******************************************************************************
*    CDC Variables
*******************************************************************************/

/*PUBLIC*/
/** Contains the current line coding structure. The host sets it using a
 * SET_LINE_CODING request and returns it to the user code using the
 * USB_GetDTERate(), USB_GetCharFormat(),
 * USB_GetParityType(), and USB_GetDataBits() APIs.
 * It is an array of 2 elements for COM port 1 and COM port 2 for MultiCOM port
 * support. In case of 1 COM port, data is in 0 element.*/
volatile uint8 USB_linesCoding[USB_MAX_MULTI_COM_NUM][USB_LINE_CODING_SIZE] =
{
    /*COM Port 1*/
    {
    0x00u, 0xC2u, 0x01u, 0x00u,     /* Data terminal rate 115200 */
    0x00u,                          /* 1 Stop bit */
    0x00u,                          /* None parity */
    0x08u                           /* 8 data bits */
    },
    /*COM Port 2*/
    {
    0x00u, 0xC2u, 0x01u, 0x00u,     /* Data terminal rate 115200 */
    0x00u,                          /* 1 Stop bit */
    0x00u,                          /* None parity */
    0x08u                           /* 8 data bits */
    }
};

/**Used as a flag for the USB_IsLineChanged() API, to inform it that the
 * host has been sent a request to change line coding or control bitmap. It is 
 * an array of 2 elements for COM port 1 and COM port 2 for MultiCOM port 
 * support. In case of 1 COM port, data is in 0 element.*/
volatile uint8  USB_linesChanged[USB_MAX_MULTI_COM_NUM];
/** Contains the current control-signal bitmap. The host sets it using a
 * SET_CONTROL_LINE request and returns it to the user code using the 
 * USB_GetLineControl() API. It is an array of 2 elements for COM 
 * port 1 and COM port 2 for MultiCOM port support. In case of 1 COM port, data 
 * is in 0 element.*/
volatile uint16 USB_linesControlBitmap[USB_MAX_MULTI_COM_NUM];
/** Contains the 16-bit serial state value that was sent using the 
 * \ref USB_SendSerialState() API. . It is an array of 2 elements 
 * for COM port 1 and COM port 2 for MultiCOM port support. In case of 1 COM 
 * port, data is in 0 element.*/
volatile uint16 USB_serialStateBitmap[USB_MAX_MULTI_COM_NUM];
/** Contains the data IN endpoint number. It is initialized after a 
 * SET_CONFIGURATION request based on a user descriptor. It is used in CDC APIs 
 * to send data to the PC. It is an array of 2 elements for COM port 1 and COM 
 * port 2 for MultiCOM port support. In case of 1 COM port, data is in 0 element.*/
volatile uint8  USB_cdcDataInEp[USB_MAX_MULTI_COM_NUM];
/** Contains the data OUT endpoint number. It is initialized after a 
 * SET_CONFIGURATION request based on user descriptor. It is used in CDC APIs to 
 * receive data from the PC. It is an array of 2 elements for COM port 1 and COM  
 * port 2 for MultiCOM port support. In case of 1 COM port, data is in 0 element.*/
volatile uint8  USB_cdcDataOutEp[USB_MAX_MULTI_COM_NUM];
/** Contains the data IN endpoint number for COMMUNICATION interface. It is 
 * initialized after a SET_CONFIGURATION request based on a user descriptor. It 
 * is used in CDC APIs to send data to the PC. It is an array of 2 elements for 
 * COM port 1 and COM port 2 for MultiCOM port support. In case of 1 COM port, 
 * data is in 0 element.*/
volatile uint8  USB_cdcCommInInterruptEp[USB_MAX_MULTI_COM_NUM];

/*PRIVATE*/

#define USB_CDC_IN_EP      (0u)
#define USB_CDC_OUT_EP     (1u)
#define USB_CDC_NOTE_EP    (2u)

#define USB_CDC_EP_MASK    (0x01u)

#define USB_GET_EP_COM_NUM(cdcComNums, epType) (((cdcComNums) >> (epType)) & USB_CDC_EP_MASK)


/***************************************
*     Static Function Prototypes
***************************************/
#if (USB_ENABLE_CDC_CLASS_API != 0u)
    static uint16 USB_StrLen(const char8 string[]) ;
    static t_USB_cdc_notification USB_serialStateNotification =
    {

        USB_SERIAL_STATE_REQUEST_TYPE, /* bRequestType    */
        USB_SERIAL_STATE,              /* bNotification   */
        0u,                                         /* wValue          */
        0u,                                         /* wValueMSB       */
        0u,                                         /* wIndex          */
        0u,                                         /* wIndexMSB       */
        USB_SERIAL_STATE_LENGTH,       /* wLength         */
        0u,                                         /* wLengthMSB      */
        0u,                                         /* wSerialState    */
        0u,                                         /* wSerialStateMSB */
    };
    static uint8 USB_activeCom = 0u;
#endif /* (USB_ENABLE_CDC_CLASS_API != 0u) */


/***************************************
* Custom Declarations
***************************************/

/* `#START CDC_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USB_DispatchCDCClassRqst
****************************************************************************//**
*
*  This routine dispatches CDC class requests.
*
* \return
*  requestHandled
*
* \globalvars
*   USB_linesCoding: Contains the current line coding structure.
*     It is set by the Host using SET_LINE_CODING request and returned to the
*     user code by the USBFS_GetDTERate(), USBFS_GetCharFormat(),
*     USBFS_GetParityType(), USBFS_GetDataBits() APIs.
*   USB_linesControlBitmap: Contains the current control signal
*     bitmap. It is set by the Host using SET_CONTROL_LINE request and returned
*     to the user code by the USBFS_GetLineControl() API.
*   USB_linesChanged: This variable is used as a flag for the
*     USBFS_IsLineChanged() API, to be aware that Host has been sent request
*     for changing Line Coding or Control Bitmap.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_DispatchCDCClassRqst(void) 
{
    uint8 requestHandled = USB_FALSE;
    uint8 comPort;

    comPort = USB_GetInterfaceComPort((uint8)USB_wIndexLoReg);

    /* Check request direction: D2H or H2D. */
    if (0u != (USB_bmRequestTypeReg & USB_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */

        switch (USB_bRequestReg)
        {
            case USB_CDC_GET_LINE_CODING:
                USB_currentTD.count = USB_LINE_CODING_SIZE;
                USB_currentTD.pData = USB_linesCoding[comPort];
                requestHandled  = USB_InitControlRead();
                break;

            /* `#START CDC_READ_REQUESTS` Place other request handler here */

            /* `#END` */

            default:
            /* Do not handle this request unless callback is defined. */
            #ifdef USB_DISPATCH_CDC_CLASS_CDC_READ_REQUESTS_CALLBACK
                requestHandled = USB_DispatchCDCClass_CDC_READ_REQUESTS_Callback();
            #endif /* (USB_DISPATCH_CDC_CLASS_CDC_READ_REQUESTS_CALLBACK) */
                break;
        }
    }
    else
    {
        /* Handle direction from host to device. */

        switch (USB_bRequestReg)
        {
            case USB_CDC_SET_LINE_CODING:
                USB_currentTD.count  = USB_LINE_CODING_SIZE;
                USB_currentTD.pData  = USB_linesCoding[comPort];
                USB_linesChanged[comPort] |= USB_LINE_CODING_CHANGED;

                requestHandled = USB_InitControlWrite();
                break;

            case USB_CDC_SET_CONTROL_LINE_STATE:
                USB_linesControlBitmap[comPort] = (uint8) USB_wValueLoReg;
                USB_linesChanged[comPort]      |= USB_LINE_CONTROL_CHANGED;

                requestHandled = USB_InitNoDataControlTransfer();
                break;

            /* `#START CDC_WRITE_REQUESTS` Place other request handler here */

            /* `#END` */

            default:
                /* Do not handle this request unless callback is defined. */
            #ifdef USB_DISPATCH_CDC_CLASS_CDC_WRITE_REQUESTS_CALLBACK
                requestHandled = USB_DispatchCDCClass_CDC_WRITE_REQUESTS_Callback();
            #endif /* (USB_DISPATCH_CDC_CLASS_CDC_WRITE_REQUESTS_CALLBACK) */
                break;
        }
    }

    return(requestHandled);
}


/***************************************************************************
* Function Name: USB_GetInterfaceComPort
************************************************************************//**
*   \internal
*  Internal function which gets number of COM port by specified interface
*   number.
*
* \param uint8 interface
*  Interface number
*
* \return
*  COM port number (0 or 1) or error 0xFF
*
***************************************************************************/
uint8 USB_GetInterfaceComPort(uint8 interface) 
{
    uint8 comPort = 0u;
    uint8 res = 0xFFu;
    uint8 notEp;

    while (comPort < USB_MAX_MULTI_COM_NUM)
    {
        notEp = USB_cdcCommInInterruptEp[comPort];

        if (USB_EP[notEp].interface == interface)
        {
            res = comPort;
            comPort = USB_MAX_MULTI_COM_NUM;
        }

        comPort++;
    }
    return (res);
}


/***************************************
* Optional CDC APIs
***************************************/
#if (USB_ENABLE_CDC_CLASS_API != 0u)
/***************************************************************************
* Function Name: USB_CDC_Init
************************************************************************//**
*
*  This function initializes the CDC interface to be ready to receive data
*  from the PC. The API set active communication port to 0 in the case of 
*  multiple communication port support.This API should be called after the 
*  device has been started and configured using USBUART_Start() API to 
*  initialize and start the USBFS component operation. Then call the 
*  USBUART_GetConfiguration() API to wait until the host has enumerated and 
*  configured the device. For example:
*
*  \snippet /USBFS_sut_02.cydsn/main.c wait for enumeration
*
* \return
*  cystatus:
*   Return Value    Description
*   USB_SUCCESS   CDC interface was initialized correctly
*   USB_FAILURE   CDC interface was not initialized
*
* \globalvars
*   USB_linesChanged: Initialized to zero.
*   USB_cdcDataOutEp: Used as an OUT endpoint number.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_CDC_Init(void) 
{
    uint8 comPort;
    uint8 outEp;
    uint8 ret = USB_SUCCESS;

    USB_activeCom = 0u;
    USB_linesChanged[USB_COM_PORT1] = 0u;
    USB_linesChanged[USB_COM_PORT2] = 0u;

    for(comPort = 0u; comPort<USB_MAX_MULTI_COM_NUM; comPort++)
    {
        outEp = USB_cdcDataOutEp[comPort];
        if((0u != outEp) && (USB_MAX_EP > outEp))
        {
            USB_EnableOutEP(outEp);
        }

    }

    /* COM Port 1 should be correct to proceed. */
    if ((0u == USB_cdcDataInEp[USB_COM_PORT1]) \
            || (0u == USB_cdcDataOutEp[USB_COM_PORT1]) \
            || (0u ==  USB_cdcCommInInterruptEp[USB_COM_PORT1])
            || (USB_cdcDataInEp[USB_COM_PORT1] >= USB_MAX_EP)
            || (USB_cdcDataOutEp[USB_COM_PORT1] >= USB_MAX_EP)
            || (USB_cdcCommInInterruptEp[USB_COM_PORT1] >= USB_MAX_EP))
    {
        ret = USB_FAILURE;
    }

    return (ret);
}


/*******************************************************************************
* Function Name: USB_PutData
****************************************************************************//**
*
*  This function sends a specified number of bytes from the location specified
*  by a pointer to the PC. The USB_CDCIsReady() function should be
*  called before sending new data, to be sure that the previous data has
*  finished sending.
*  If the last sent packet is less than maximum packet size the USB transfer
*  of this short packet will identify the end of the segment. If the last sent
*  packet is exactly maximum packet size, it shall be followed by a zero-length
*  packet (which is a short packet) to assure the end of segment is properly
*  identified. To send zero-length packet, use USB_PutData() API
*  with length parameter set to zero.
*
*  \param pData: pointer to the buffer containing data to be sent.
*  \param length: Specifies the number of bytes to send from the pData
*  buffer. Maximum length will be limited by the maximum packet
*  size for the endpoint. Data will be lost if length is greater than Max
*  Packet Size.
*
* \globalvars
*
*   USB_cdcDataInEp: CDC IN endpoint number used for sending
*     data.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_PutData(const uint8* pData, uint16 length) 
{
    uint8 epNumber = USB_cdcDataInEp[USB_activeCom];

    /* Limit length to maximum packet size for endpoint. */
    if (length > USB_EP[epNumber].bufferSize)
    {
        /* Caution: Data will be lost if length is greater than Max Packet Size. */
        length = USB_EP[epNumber].bufferSize;

        /* Halt CPU in debug mode */
        CYASSERT(0u != 0u);
    }

    USB_LoadInEP(epNumber, pData, length);
}


/*******************************************************************************
* Function Name: USB_StrLen
****************************************************************************//**
*
*  Calculates length of a null terminated string.
*
*  \param string: pointer to the string.
*
* \return
*  Length of the string
*
*******************************************************************************/
static uint16 USB_StrLen(const char8 string[]) 
{
    uint16 len = 0u;

    while (string[len] != (char8)0)
    {
        len++;
    }

    return ((uint16) len);
}


/***************************************************************************
* Function Name: USB_PutString
************************************************************************//**
*
*  This function sends a null terminated string to the PC. This function will
*  block if there is not enough memory to place the whole string. It will block
*  until the entire string has been written to the transmit buffer.
*  The USB_CDCIsReady() function should be called before
*  sending data with a new call to USB_PutString(), to be sure
*  that the previous data has finished sending. This function sends
*  zero-length packet automatically, if the length of the last packet, sent
*  by this API, is equal to Max Packet Size
*
*  \param string: pointer to the string to be sent to the PC.
*
* \globalvars
*
*   USB_cdcDataInEp: CDC IN endpoint number used for sending
*     data.
*
* \reentrant
*  No.
*
***************************************************************************/
void USB_PutString(const char8 string[]) 
{
    uint16 strLength;
    uint16 sendLength;
    uint16 bufIndex = 0u;

    uint8  epNumber = USB_cdcDataInEp[USB_activeCom];

    /* Get length string length (it is terminated with zero). */
    strLength = USB_StrLen(string);

    do
    {
        /* Limit length to maximum packet size of endpoint. */
        sendLength = (strLength > USB_EP[epNumber].bufferSize) ?
                                  USB_EP[epNumber].bufferSize : strLength;

        /* Load IN endpoint and expose it to host. */
        USB_LoadInEP(epNumber, (const uint8 *)&string[bufIndex], sendLength);
        strLength -= sendLength;

        /* If more data are present to send or full packet was sent */
        if ((strLength > 0u) || (sendLength == USB_EP[epNumber].bufferSize))
        {
            bufIndex += sendLength;

            /* Wait until host read data from IN endpoint buffer. */
            while (USB_IN_BUFFER_FULL == USB_EP[epNumber].apiEpState)
            {
            }

            /* If last packet is exactly maximum packet size, it shall be followed
            * by a zero-length packet to assure the end of segment is properly
            * identified by the terminal.
            */
            if (0u == strLength)
            {
                USB_LoadInEP(epNumber, NULL, 0u);
            }
        }
    }
    while (strLength > 0u);
}


/***************************************************************************
* Function Name: USB_PutChar
************************************************************************//**
*
*  This function writes a single character to the PC at a time. This is an
*  inefficient way to send large amounts of data.
*
*  \param txDataByte: Character to be sent to the PC.
*
*  \globalvars
*
*   USB_cdcDataInEp: CDC IN endpoint number used for sending
*     data.
*
*  \reentrant
*  No.
*
***************************************************************************/
void USB_PutChar(char8 txDataByte) 
{
    uint8 dataByte;
    dataByte = (uint8) txDataByte;

    USB_LoadInEP(USB_cdcDataInEp[USB_activeCom], &dataByte, 1u);
}


/*******************************************************************************
* Function Name: USB_PutCRLF
****************************************************************************//**
*
*  This function sends a carriage return (0x0D) and line feed (0x0A) to the
*  PC. This APIis provided to mimic API provided by our other UART components
*
* \globalvars
*
*   USB_cdcDataInEp: CDC IN endpoint number used for sending
*     data.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_PutCRLF(void) 
{
    const uint8 CYCODE txData[] = {0x0Du, 0x0Au};

    USB_LoadInEP(USB_cdcDataInEp[USB_activeCom], (const uint8 *)txData, 2u);
}


/*******************************************************************************
* Function Name: USB_GetCount
****************************************************************************//**
*
*  This function returns the number of bytes that were received from the PC.
*  The returned length value should be passed to USB_GetData() as
*  a parameter to read all received data. If all of the received data is not
*  read at one time by the USB_GetData() API, the unread data will
*  be lost.
*
* \return
*  Returns the number of received bytes. The maximum amount of received data at
*  a time is limited by the maximum packet size for the endpoint.
*
* \globalvars
*   USB_cdcDataOutEp: CDC OUT endpoint number used.
*
*******************************************************************************/
uint16 USB_GetCount(void) 
{
    uint16 bytesCount;

    uint8  epNumber = USB_cdcDataOutEp[USB_activeCom];

    if (USB_OUT_BUFFER_FULL == USB_EP[epNumber].apiEpState)
    {
        bytesCount = USB_GetEPCount(epNumber);
    }
    else
    {
        bytesCount = 0u;
    }

    return (bytesCount);
}


/*******************************************************************************
* Function Name: USB_DataIsReady
****************************************************************************//**
*
*  This function returns a non-zero value if the component received data or
*  received zero-length packet. The USB_GetAll() or
*  USB_GetData() API should be called to read data from the buffer
*  and reinitialize the OUT endpoint even when a zero-length packet is
*  received. These APIs will return zero value when zero-length packet is
*  received.
*
* \return
*  If the OUT packet is received, this function returns a non-zero value.
*  Otherwise, it returns zero.
*
* \globalvars
*   USB_cdcDataOutEp: CDC OUT endpoint number used.
*
*******************************************************************************/
uint8 USB_DataIsReady(void) 
{
    return (USB_GetEPState(USB_cdcDataOutEp[USB_activeCom]));
}


/*******************************************************************************
* Function Name: USB_CDCIsReady
****************************************************************************//**
*
*  This function returns a non-zero value if the component is ready to send more
*  data to the PC; otherwise, it returns zero. The function should be called
*  before sending new data when using any of the following APIs:
*  USB_PutData(),USB_PutString(),
*  USB_PutChar or USB_PutCRLF(),
*  to be sure that the previous data has finished sending.
*
* \return
*  If the buffer can accept new data, this function returns a non-zero value.
*  Otherwise, it returns zero.
*
* \globalvars
*   USB_cdcDataInEp: CDC IN endpoint number used.
*
*******************************************************************************/
uint8 USB_CDCIsReady(void) 
{
    return (USB_GetEPState(USB_cdcDataInEp[USB_activeCom]));
}


/***************************************************************************
* Function Name: USB_GetData
************************************************************************//**
*
*  This function gets a specified number of bytes from the input buffer and
*  places them in a data array specified by the passed pointer.
*  The USB_DataIsReady() API should be called first, to be sure
*  that data is received from the host. If all received data will not be read at
*  once, the unread data will be lost. The USB_GetData() API should
*  be called to get the number of bytes that were received.
*
*  \param pData: Pointer to the data array where data will be placed.
*  \param Length: Number of bytes to read into the data array from the RX buffer.
*          Maximum length is limited by the the number of received bytes
*          or 64 bytes.
*
* \return
*         Number of bytes which function moves from endpoint RAM into the
*         data array. The function moves fewer than the requested number
*         of bytes if the host sends fewer bytes than requested or sends
*         zero-length packet.
*
* \globalvars
*   USB_cdcDataOutEp: CDC OUT endpoint number used.
*
* \reentrant
*  No.
*
***************************************************************************/
uint16 USB_GetData(uint8* pData, uint16 length) 
{
    uint8 epNumber = USB_cdcDataOutEp[USB_activeCom];

    /* Read data from OUT endpoint buffer. */
    length = USB_ReadOutEP(epNumber, pData, length);

#if (USB_EP_MANAGEMENT_DMA_MANUAL)
    /* Wait until DMA complete transferring data from OUT endpoint buffer. */
    while (USB_OUT_BUFFER_FULL == USB_GetEPState(epNumber))
    {
    }

    /* Enable OUT endpoint to communicate with host. */
    USB_EnableOutEP(epNumber);
#endif /* (USB_EP_MANAGEMENT_DMA_MANUAL) */

    return (length);
}


/*******************************************************************************
* Function Name: USB_GetAll
****************************************************************************//**
*
*  This function gets all bytes of received data from the input buffer and
*  places them into a specified data array. The
*  USB_DataIsReady() API should be called first, to be sure
*  that data is received from the host.
*
*  \param pData: Pointer to the data array where data will be placed.
*
* \return
*  Number of bytes received. The maximum amount of the received at a time
*  data is 64 bytes.
*
* \globalvars
*   - \ref USB_cdcDataOutEp: CDC OUT endpoint number used.
*   - \ref USB_EP[].bufferSize: EP max packet size is used as a
*     length to read all data from the EP buffer.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint16 USB_GetAll(uint8* pData) 
{
    uint8 epNumber = USB_cdcDataOutEp[USB_activeCom];
    uint16 dataLength;

    /* Read data from OUT endpoint buffer. */
    dataLength = USB_ReadOutEP(epNumber, pData, USB_EP[epNumber].bufferSize);

#if (USB_EP_MANAGEMENT_DMA_MANUAL)
    /* Wait until DMA complete transferring data from OUT endpoint buffer. */
    while (USB_OUT_BUFFER_FULL == USB_GetEPState(epNumber))
    {
    }

    /* Enable OUT endpoint to communicate with host. */
    USB_EnableOutEP(epNumber);
#endif /* (USB_EP_MANAGEMENT_DMA_MANUAL) */

    return (dataLength);
}


/***************************************************************************
* Function Name: USB_GetChar
************************************************************************//**
*
*  This function reads one byte of received data from the buffer. If more than
*  one byte has been received from the host, the rest of the data will be lost.
*
* \return
*  Received one character.
*
* \globalvars
*   USB_cdcDataOutEp: CDC OUT endpoint number used.
*
* \reentrant
*  No.
*
***************************************************************************/
uint8 USB_GetChar(void) 
{
     uint8 rxData;
     uint8 epNumber = USB_cdcDataOutEp[USB_activeCom];

    (void) USB_ReadOutEP(epNumber, &rxData, 1u);

#if (USB_EP_MANAGEMENT_DMA_MANUAL)
    /* Wait until DMA complete transferring data from OUT endpoint buffer. */
    while (USB_OUT_BUFFER_FULL == USB_GetEPState(epNumber))
    {
    }

    /* Enable OUT endpoint to communicate with host. */
    USB_EnableOutEP(epNumber);
#endif /* (USB_EP_MANAGEMENT_DMA_MANUAL) */

    return (rxData);
}


/*******************************************************************************
* Function Name: USB_IsLineChanged
****************************************************************************//**
*
*  This function returns clear on read status of the line. It returns not zero
*  value when the host sends updated coding or control information to the
*  device. The USB_GetDTERate(), USB_GetCharFormat()
*  or USB_GetParityType() or USB_GetDataBits() API
*  should be called to read data coding information.
*  The USB_GetLineControl() API should be called to read line
*  control information.
*
* \return
*  If SET_LINE_CODING or CDC_SET_CONTROL_LINE_STATE requests are received, it
*  returns a non-zero value. Otherwise, it returns zero.
*  Return Value                 | Description
*  -----------------------------|--------------------------
*  USBUART_LINE_CODING_CHANGED  | Line coding changed
*  USBUART_LINE_CONTROL_CHANGED |   Line control changed
*
* \globalvars
*  - \ref USB_transferState: it is checked to be sure then OUT
*    data phase has been complete, and data written to the lineCoding or
*    Control Bitmap buffer.
*  - \ref USB_linesChanged: used as a flag to be aware that
*    Host has been sent request for changing Line Coding or Control Bitmap.
*
*******************************************************************************/
uint8 USB_IsLineChanged(void) 
{
    uint8 state = 0u;

    /* transferState is checked to be sure then OUT data phase has been complete */
    if (USB_transferState == USB_TRANS_STATE_IDLE)
    {
        if (USB_linesChanged[USB_activeCom] != 0u)
        {
            state = USB_linesChanged[USB_activeCom];
            USB_linesChanged[USB_activeCom] = 0u;
        }
    }

    return (state);
}


/***************************************************************************
* Function Name: USB_GetDTERate
************************************************************************//**
*
*  This function returns the data terminal rate set for this port in bits
*  per second.
*
* \return
*  Returns a uint32 value of the data rate in bits per second.
*
* \globalvars
*  USB_linesCoding: First four bytes converted to uint32
*    depend on compiler, and returned as a data rate.
*
*******************************************************************************/
uint32 USB_GetDTERate(void) 
{
    uint32 rate;

    rate = USB_linesCoding[USB_activeCom][USB_LINE_CODING_RATE + 3u];
    rate = (rate << 8u) | USB_linesCoding[USB_activeCom][USB_LINE_CODING_RATE + 2u];
    rate = (rate << 8u) | USB_linesCoding[USB_activeCom][USB_LINE_CODING_RATE + 1u];
    rate = (rate << 8u) | USB_linesCoding[USB_activeCom][USB_LINE_CODING_RATE];

    return (rate);
}


/*******************************************************************************
* Function Name: USB_GetCharFormat
****************************************************************************//**
*
*  Returns the number of stop bits.
*
* \return
*  Returns the number of stop bits.
*  Return               |Value Description
*  ---------------------|-------------------
*  USBUART_1_STOPBIT    | 1 stop bit
*  USBUART_1_5_STOPBITS | 1,5 stop bits
*  USBUART_2_STOPBITS   | 2 stop bits
*
*
* \globalvars
*  USB_linesCoding: used to get a parameter.
*
*******************************************************************************/
uint8 USB_GetCharFormat(void) 
{
    return (USB_linesCoding[USB_activeCom][USB_LINE_CODING_STOP_BITS]);
}


/*******************************************************************************
* Function Name: USB_GetParityType
****************************************************************************//**
*
*  This function returns the parity type for the CDC port.
*
* \return
*  Returns the parity type.
*   Return               | Value Description
*  ----------------------|-------------------
*  USBUART_PARITY_NONE   | 1 stop bit
*  USBUART_PARITY_ODD    | 1,5 stop bits
*  USBUART_PARITY_EVEN   | 2 stop bits
*  USBUART_PARITY_MARK   | Mark
*  USBUART_PARITY_SPACE  | Space
*
* \globalvars
*  USB_linesCoding: used to get a parameter.
*
*******************************************************************************/
uint8 USB_GetParityType(void) 
{
    return (USB_linesCoding[USB_activeCom][USB_LINE_CODING_PARITY]);
}


/***************************************************************************
* Function Name: USB_GetDataBits
************************************************************************//**
*
*  This function returns the number of data bits for the CDC port.
*
* \return
*  Returns the number of data bits.
*  The number of data bits can be 5, 6, 7, 8 or 16.
*
* \globalvars
*  USB_linesCoding: used to get a parameter.
*
*******************************************************************************/
uint8 USB_GetDataBits(void) 
{
    return (USB_linesCoding[USB_activeCom][USB_LINE_CODING_DATA_BITS]);
}


/***************************************************************************
* Function Name: USB_GetLineControl
************************************************************************//**
*
*  This function returns Line control bitmap that the host sends to the
*  device.
*
* \return
*  Returns Line control bitmap.
*  Return                   |Value Notes
*  -------------------------|-----------------------------------------------
*  USBUART_LINE_CONTROL_DTR |Indicates that a DTR signal is present. This signal corresponds to V.24 signal 108/2 and RS232 signal DTR.
*  USBUART_LINE_CONTROL_RTS |Carrier control for half-duplex modems. This signal corresponds to V.24 signal 105 and RS232 signal RTS.
*  RESERVED                 |The rest of the bits are reserved.
*
*  *Note* Some terminal emulation programs do not properly handle these
*  control signals. They update information about DTR and RTS state only
*  when the RTS signal changes the state.
*
* \globalvars
*  USB_linesControlBitmap: used to get a parameter.
*
*******************************************************************************/
uint16 USB_GetLineControl(void) 
{
    return (USB_linesControlBitmap[USB_activeCom]);
}


/*******************************************************************************
* Function Name: USB_SendSerialState
****************************************************************************//**
*
*  Sends the serial state notification to the host using the interrupt
*  endpoint for the COM port selected using the API SetComPort().The
*  USB_NotificationIsReady() API must be called to check if the
*  Component is ready to send more serial state to the host. The API will
*  not send the notification data if the interrupt endpoint Max Packet Size
*  is less than the required 10 bytes.
*
* \param uint16 serialState
*  16-bit value that will be sent from the device to the
*  host as SERIAL_STATE notification using the IN interrupt endpoint. Refer
*  to revision 1.2 of the CDC PSTN Subclass specification for bit field
*  definitions of the 16-bit serial state value.
*
*******************************************************************************/
void USB_SendSerialState (uint16 serialState) 
{
    uint8 epNumber = USB_cdcCommInInterruptEp[USB_activeCom];

    if(USB_SERIAL_STATE_SIZE <= USB_EP[epNumber].bufferSize)
    {
        /* Save current SERIAL_STATE bitmap. */
        USB_serialStateBitmap[USB_activeCom] = serialState;

        /* Add interface number */
        USB_serialStateNotification.wIndex = USB_EP[epNumber].interface;

        /*Form SERIAL_STATE data*/
        USB_serialStateNotification.wSerialState =    LO8(USB_serialStateBitmap[USB_activeCom]);
        USB_serialStateNotification.wSerialStateMSB = HI8(USB_serialStateBitmap[USB_activeCom]);

        USB_LoadInEP(epNumber, (uint8 *) &USB_serialStateNotification, sizeof(USB_serialStateNotification));
    }
}


/*******************************************************************************
* Function Name: USB_GetSerialState
****************************************************************************//**
*
*  This function returns the current serial state value for the COM port
*  selected using the API SetComPort().
*
* \return
*  16-bit serial state value. Refer to revision 1.2 of the CDC PSTN Subclass
*  specification for bit field definitions of the 16-bit serial state value.
*
*******************************************************************************/
uint16 USB_GetSerialState(void) 
{
    return USB_serialStateBitmap[USB_activeCom];
}


/*******************************************************************************
* Function Name: USB_NotificationIsReady
****************************************************************************//**
*
*  This function returns a non-zero value if the Component is ready to send
*  more notifications to the host; otherwise, it returns zero. The function
*  should be called before sending new notifications when using
*  USB_SendSerialState() to ensure that any previous
*  notification data has been already sent to the host.
*
* \return
*  If the buffer can accept new data(endpoint buffer not full), this
*  function returns a non-zero value. Otherwise, it returns zero.
*
* \globalvars
*   USB_cdcDataInEp: CDC IN endpoint number used.
*
*******************************************************************************/
uint8 USB_NotificationIsReady(void) 
{
    return (USB_EP[USB_cdcCommInInterruptEp[USB_activeCom]].apiEpState);
}


/*******************************************************************************
* Function Name: USB_SetComPort
****************************************************************************//**
*
*  This function allows the user to select from one of the two COM ports
*  they wish to address in the instance of having multiple COM ports
*  instantiated though the use of a composite device. Once set, all future
*  function calls related to the USBUART will be affected. This addressed
*  COM port can be changed during run time.
*
* \param comNumber
*  Contains the COM interface the user wishes to address. Value can either
*  be 0 or 1 since a maximum of only 2 COM ports can be supported. Note that
*  this COM port number is not the COM port number assigned on the PC side
*  for the UART communication. If a value greater than 1 is passed, the
*  function returns without performing any action.
*
*******************************************************************************/
void USB_SetComPort(uint8 comNumber) 
{
    if ((USB_activeCom != comNumber) && \
            (comNumber < USB_MAX_MULTI_COM_NUM ))
    {
        USB_activeCom = comNumber;
    }
}


/*******************************************************************************
* Function Name: USB_GetComPort
****************************************************************************//**
*
*  This function returns the current selected COM port that the user is
*  currently addressing in the instance of having multiple COM ports
*  instantiated though the use of a composite device.
*
* \return
*  Returns the currently selected COM port. Value can either be 0 or 1 since
*  a maximum of only 2 COM ports can be supported. . Note that this COM port
*  number is not the COM port number assigned on the PC side for the UART
*  communication.
*
*******************************************************************************/
uint8 USB_GetComPort(void) 
{
    return (USB_activeCom);
}


#endif  /* (USB_ENABLE_CDC_CLASS_API) */


/***************************************************************************
* Function Name: USB_Cdc_EpInit
************************************************************************//**
*
*  \internal
*  This routine decide type of endpoint (IN, OUT, Notification) and same to
*   appropriate global variables  according to COM port number.
*   USB_cdcDataInEp[], USB_cdcCommInInterruptEp[],
*   USB_cdcDataOutEp[]
*
* \param pEP: Pointer to structure with current EP description.
* \param epNum: EP number
* \param cdcComNums: Bit array of current COM ports for CDC IN, OUT,
*        and notification EPs(0 - COM port 1, 1- COM port 2)
*
* \return
*  Updated cdcComNums
*
* \reentrant
*  No.
*
***************************************************************************/
uint8 USB_Cdc_EpInit(const T_USB_EP_SETTINGS_BLOCK CYCODE *pEP, uint8 epNum, uint8 cdcComNums) 
{
    uint8 epType;

    epType = pEP->attributes & USB_EP_TYPE_MASK;

    if (0u != (pEP->addr & USB_DIR_IN))
    {
        if (epType != USB_EP_TYPE_INT)
        {
            USB_cdcDataInEp[USB_GET_EP_COM_NUM(cdcComNums, USB_CDC_IN_EP)] = epNum;
            cdcComNums |= (uint8)(USB_COM_PORT2 << USB_CDC_IN_EP);
        }
        else
        {

            USB_cdcCommInInterruptEp[USB_GET_EP_COM_NUM(cdcComNums, USB_CDC_NOTE_EP)] = epNum;
            cdcComNums |= (uint8)(USB_COM_PORT2 << USB_CDC_NOTE_EP);
        }
    }
    else
    {
        if (epType != USB_EP_TYPE_INT)
        {
            USB_cdcDataOutEp[USB_GET_EP_COM_NUM(cdcComNums, USB_CDC_OUT_EP)] = epNum;
            cdcComNums |= (uint8)(USB_COM_PORT2 << USB_CDC_OUT_EP);
        }
    }
    return (cdcComNums);
}


/*******************************************************************************
* Additional user functions supporting CDC Requests
********************************************************************************/

/* `#START CDC_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /* (USB_ENABLE_CDC_CLASS) */


/* [] END OF FILE */
