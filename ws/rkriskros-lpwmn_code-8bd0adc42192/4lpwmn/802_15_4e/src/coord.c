#if 0

#include <mlme.h>

#ifdef __linux__
#include <assert.h>
#endif



/*
 **************************************************************
 *
 *
 *
 *
 *
 **************************************************************
 */
void NodeMgr_evtHndlr(  )
{
   MAC_sts_t macSts;

   switch (nodeMgrCntxt.state)
   {
       case NODE_MGR_STATE_WAIT_MAC_INIT_DONE:
            {
                switch (evtType)
                {
                     case NODE_MGR_EVENT_MAC_INIT_DONE:
                          {
                              macSts = MLME_setSlotFrameRequest(  );
                              if (macSts != MAC_STS_SUCCESS)
                              {
                                  proj_assert();
                              }
                              else
                              {
                                  nodeMgrCntxt.state = NODE_MGR_STATE_WAIT_SET_SLOT_FRAME_RESP;
                              }
                          }
              break;

                     default:
                          {

                          }
              break;
                }
            }
            break;

       default:
            {

            }
            break;
   }

   return;
}

#endif
