#include "USBAudio.h"

USBAudio_ USBAudio;

bool USBAudio_::setup(USBSetup &setup) {
  (void)setup;
  return false;
}

int USBAudio_::getDescriptor(USBSetup &setup) {
  (void)setup;
  return 0;
}

uint8_t USBAudio_::getShortName(char *name) {
  memcpy(name, "Audio", 5);
  return 5;
}

int USBAudio_::getInterface(uint8_t *interfaceCount) {
  // Uses 2 interfaces
  *interfaceCount += 2;

  // Send descriptor
  USBAudioDescriptor descriptor = {
    // Interface association
    D_IAD(audio_control_interface, 2, 1 /* AUDIO */, 1 /* AUDIO_CONTROL */, 0),

    // AudioControl interface
    D_INTERFACE(audio_control_interface, 0, 1 /* AUDIO */, 1 /* AUDIO_CONTROL */, 0),
    USBAudio_ACInterfaceDescriptor(sizeof(USBAudio_ACInterfaceDescriptor) + sizeof(USBAudio_ACInputTerminalDescriptor) + sizeof(USBAudio_ACOutputTerminalDescriptor), audio_stream_interface),
    USBAudio_ACInputTerminalDescriptor(1, 0x101 /* USB Stream */, 1, 0),
    // USBAudio_ACFeatureDescriptor(2, 1),
    USBAudio_ACOutputTerminalDescriptor(2, 0x301 /* SPEAKER */, 1),

    // AudioStream interface
    InterfaceDescriptorAlternate(audio_stream_interface, 0, 0, 1 /* AUDIO */, 2 /* AUDIO_STREAMING */, 0),
    InterfaceDescriptorAlternate(audio_stream_interface, 1, 1, 1 /* AUDIO */, 2 /* AUDIO_STREAMING */, 0),
    USBAudio_ASInterfaceDescriptor(1, 0, 0x0001 /* PCM Format */),
    USBAudio_ASType1FormatDescriptor(1, 2, 16, 48000),
    USBAudio_ASIsocEndpointDescriptor(pluggedEndpoint, 512),
    USBAudio_ASCSIsocEndpointDescriptor()
  };

  return USBD_SendControl(0, &descriptor, sizeof(descriptor));
}

uint32_t USBAudio_::available() {
  return USBD_Available(pluggedEndpoint);
}

uint32_t USBAudio_::read(void *buf, uint32_t max_size) {
  return USBD_Recv(pluggedEndpoint, buf, max_size);
}

USBAudio_::USBAudio_():
  PluggableUSBModule(1, 2, &endpoint_type) {

  endpoint_type = UOTGHS_DEVEPTCFG_EPSIZE_512_BYTE |
                  UOTGHS_DEVEPTCFG_EPTYPE_ISO |
                  UOTGHS_DEVEPTCFG_EPBK_1_BANK |
                  UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS |
                  UOTGHS_DEVEPTCFG_ALLOC;

  PluggableUSB().plug(this);

  audio_control_interface = pluggedInterface;
  audio_stream_interface = pluggedInterface + 1;
}
