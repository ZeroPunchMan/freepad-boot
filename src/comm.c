#include "comm.h"
#include "cl_event_system.h"
#include "usb_agent.h"
#include "sgp_protocol.h"

CL_Result_t SgpAcmSendFunc(const uint8_t *buff, uint16_t count)
{
    if(SendData(buff,count)==count)
        return CL_ResSuccess;

    return CL_ResFailed;
}

void Comm_Init(void)
{
    SgpProtocol_AddChannel(SpgChannelHandle_Acm, SgpAcmSendFunc);
}

void Comm_Process(void)
{
    uint8_t buff[64];
    uint32_t recvLen = RecvData(buff, sizeof(buff));
    if(recvLen)
    {
        SgpProtocol_RecvData(SpgChannelHandle_Acm, buff, recvLen);
    }
}
