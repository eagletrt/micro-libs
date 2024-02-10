#ifndef CAN_MANAGER_TYPE_MOCKING_H
#define CAN_MANAGER_TYPE_MOCKING_H

#include "can_manager_can_types.h"

#define CAN_RX_FIFO0 (0x00000000U) /*!< CAN receive FIFO 0 */
#define CAN_RX_FIFO1 (0x00000001U) /*!< CAN receive FIFO 1 */
#define CAN_RX_FIFO2 (0x00000002U) /*!< CAN receive FIFO 2 */

typedef enum {
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef struct {
  uint8_t dummy;
} CAN_HandleTypeDef;

typedef struct {
  uint32_t FilterIdHigh; /*!< Specifies the filter identification number (MSBs
                            for a 32-bit configuration, first one for a 16-bit
                            configuration). This parameter must be a number
                            between Min_Data = 0x0000 and Max_Data = 0xFFFF. */

  uint32_t FilterIdLow; /*!< Specifies the filter identification number (LSBs
                           for a 32-bit configuration, second one for a 16-bit
                           configuration). This parameter must be a number
                           between Min_Data = 0x0000 and Max_Data = 0xFFFF. */

  uint32_t
      FilterMaskIdHigh; /*!< Specifies the filter mask number or identification
                           number, according to the mode (MSBs for a 32-bit
                           configuration, first one for a 16-bit configuration).
                             This parameter must be a number between Min_Data =
                           0x0000 and Max_Data = 0xFFFF. */

  uint32_t
      FilterMaskIdLow; /*!< Specifies the filter mask number or identification
                          number, according to the mode (LSBs for a 32-bit
                          configuration, second one for a 16-bit configuration).
                            This parameter must be a number between Min_Data =
                          0x0000 and Max_Data = 0xFFFF. */

  uint32_t FilterFIFOAssignment; /*!< Specifies the FIFO (0 or 1U) which will be
                                    assigned to the filter. This parameter can
                                    be a value of @ref CAN_filter_FIFO */

  uint32_t FilterBank; /*!< Specifies the filter bank which will be initialized.
                            For single CAN instance(14 dedicated filter banks),
                            this parameter must be a number between Min_Data = 0
                          and Max_Data = 13. For dual CAN instances(28 filter
                          banks shared), this parameter must be a number between
                          Min_Data = 0 and Max_Data = 27. */

  uint32_t
      FilterMode; /*!< Specifies the filter mode to be initialized.
                       This parameter can be a value of @ref CAN_filter_mode */

  uint32_t FilterScale; /*!< Specifies the filter scale.
                             This parameter can be a value of @ref
                           CAN_filter_scale */

  uint32_t FilterActivation; /*!< Enable or disable the filter.
                                  This parameter can be a value of @ref
                                CAN_filter_activation */

  uint32_t
      SlaveStartFilterBank; /*!< Select the start filter bank for the slave CAN
                               instance. For single CAN instances, this
                               parameter is meaningless. For dual CAN instances,
                               all filter banks with lower index are assigned to
                               master CAN instance, whereas all filter banks
                               with greater index are assigned to slave CAN
                               instance. This parameter must be a number between
                               Min_Data = 0 and Max_Data = 27. */

} CAN_FilterTypeDef;

typedef struct {
  uint32_t StdId; /*!< Specifies the standard identifier.
                       This parameter must be a number between Min_Data = 0 and
                     Max_Data = 0x7FF. */

  uint32_t ExtId; /*!< Specifies the extended identifier.
                       This parameter must be a number between Min_Data = 0 and
                     Max_Data = 0x1FFFFFFF. */

  uint32_t IDE; /*!< Specifies the type of identifier for the message that will
                   be transmitted. This parameter can be a value of @ref
                   CAN_identifier_type */

  uint32_t RTR; /*!< Specifies the type of frame for the message that will be
                   transmitted. This parameter can be a value of @ref
                   CAN_remote_transmission_request */

  uint32_t DLC; /*!< Specifies the length of the frame that will be transmitted.
                     This parameter must be a number between Min_Data = 0 and
                   Max_Data = 8. */

  uint32_t Timestamp; /*!< Specifies the timestamp counter value captured on
                         start of frame reception.
                          @note: Time Triggered Communication Mode must be
                         enabled. This parameter must be a number between
                         Min_Data = 0 and Max_Data = 0xFFFF. */

  uint32_t FilterMatchIndex; /*!< Specifies the index of matching acceptance
                          filter element. This parameter must be a number
                          between Min_Data = 0 and Max_Data = 0xFF. */

} CAN_RxHeaderTypeDef;

typedef int FunctionalState;

typedef struct {
  uint32_t StdId; /*!< Specifies the standard identifier.
                       This parameter must be a number between Min_Data = 0 and
                     Max_Data = 0x7FF. */

  uint32_t ExtId; /*!< Specifies the extended identifier.
                       This parameter must be a number between Min_Data = 0 and
                     Max_Data = 0x1FFFFFFF. */

  uint32_t IDE; /*!< Specifies the type of identifier for the message that will
                   be transmitted. This parameter can be a value of @ref
                   CAN_identifier_type */

  uint32_t RTR; /*!< Specifies the type of frame for the message that will be
                   transmitted. This parameter can be a value of @ref
                   CAN_remote_transmission_request */

  uint32_t DLC; /*!< Specifies the length of the frame that will be transmitted.
                     This parameter must be a number between Min_Data = 0 and
                   Max_Data = 8. */

  FunctionalState
      TransmitGlobalTime; /*!< Specifies whether the timestamp counter value
              captured on start of frame transmission, is sent in DATA6 and
              DATA7 replacing pData[6] and pData[7].
              @note: Time Triggered Communication Mode must be enabled.
              @note: DLC must be programmed as 8 bytes, in order these 2 bytes
              are sent. This parameter can be set to ENABLE or DISABLE. */

} CAN_TxHeaderTypeDef;

#endif // CAN_MANAGER_TYPE_MOCKING_H
