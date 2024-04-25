#pragma once

#include "Arduino.h"
#include "USB/PluggableUSB.h"

#pragma pack(1)

struct InterfaceDescriptorAlternate: public InterfaceDescriptor {
  InterfaceDescriptorAlternate(uint8_t n, uint8_t alternate, uint8_t n_endpoints, uint8_t interface_class, uint8_t interface_subclass, uint8_t protocol):
    InterfaceDescriptor{9, 4, n, alternate, n_endpoints, interface_class, interface_subclass,  protocol, 0} {}
};

struct USBAudio_ACInterfaceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
  uint8_t bInCollection;
  uint8_t baInterfaceNr1;

  USBAudio_ACInterfaceDescriptor(uint16_t total_length, uint8_t streaming_interface):
    bLength(9),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x01), // HEADER
    bcdADC(0x0100),           // Audio class version 1.0
    wTotalLength(total_length),
    bInCollection(1),
    baInterfaceNr1(streaming_interface) {}
};

struct USBAudio_ACInputTerminalDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t bNrChannels;
  uint16_t wChannelConfig;
  uint8_t iChannelNames;
  uint8_t iTerminal;

  USBAudio_ACInputTerminalDescriptor(uint8_t terminal_id, uint16_t type, uint8_t n_channels, uint16_t channel_config):
    bLength(12),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x02), // INPUT_TERMINAL
    bTerminalID(terminal_id),
    wTerminalType(type),
    bAssocTerminal(0),
    bNrChannels(n_channels),
    wChannelConfig(channel_config),
    iChannelNames(0),
    iTerminal(0) {}
};

struct USBAudio_ACOutputTerminalDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t bSourceID;
  uint8_t iTerminal;

  USBAudio_ACOutputTerminalDescriptor(uint8_t terminal_id, uint16_t type, uint8_t source):
    bLength(9),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x03), // OUTPUT_TERMINAL
    bTerminalID(terminal_id),
    wTerminalType(type),
    bAssocTerminal(0),
    bSourceID(source),
    iTerminal(0) {}
};

struct USBAudio_ACFeatureDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bSourceID;
  uint8_t bControlSize;
  uint16_t bmaControls0;
  uint16_t bmaControls1;
  uint8_t iFeature;

  USBAudio_ACFeatureDescriptor(uint8_t unit_id, uint8_t source_id):
    bLength(11),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x06), // FEATURE_UNIT
    bUnitID(unit_id),
    bSourceID(source_id),
    bControlSize(1),
    bmaControls0(0),
    bmaControls1(0),
    iFeature(0) {}
};

struct USBAudio_ASInterfaceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalLink;
  uint8_t bDelay;
  uint16_t wFormatTag;

  USBAudio_ASInterfaceDescriptor(uint8_t terminal_link, uint8_t delay, uint16_t format):
    bLength(7),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x01), // AUDIO_STREAMING_GENERAL
    bTerminalLink(terminal_link),
    bDelay(delay),
    wFormatTag(format) {}
};

struct USBAudio_ASType1FormatDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatType;
  uint8_t bNrChannels;
  uint8_t bSubframeSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  uint32_t tSamFreq:24;

  USBAudio_ASType1FormatDescriptor(uint8_t n_channels, uint8_t subframe_size, uint8_t bit_resolution, uint32_t sample_frequency):
    bLength(11),
    bDescriptorType(0x24),    // CLASS_SPECIFIC_INTERFACE
    bDescriptorSubtype(0x02), // FORMAT_TYPE
    bFormatType(0x01),        // TYPE_I
    bNrChannels(n_channels),
    bSubframeSize(subframe_size),
    bBitResolution(bit_resolution),
    bSamFreqType(1),
    tSamFreq(sample_frequency) {}
};

struct USBAudio_ASIsocEndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
  uint8_t bRefresh;
  uint8_t bSynchAddress;

  USBAudio_ASIsocEndpointDescriptor(uint8_t addr, uint16_t max_packet_size):
    bLength(9),
    bDescriptorType(0x05), // ENDPOINT
    bEndpointAddress(addr),
    bmAttributes(0x05),    // Asynchronous
    wMaxPacketSize(max_packet_size),
    bInterval(4),
    bRefresh(0),
    bSynchAddress(0) {}
};

struct USBAudio_ASCSIsocEndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmAttributes;
  uint8_t bLockDelayUnits;
  uint16_t wLockDelay;

  USBAudio_ASCSIsocEndpointDescriptor():
    bLength(7),
    bDescriptorType(0x25),    // CLASS_SPECIFIC_ENDPOINT
    bDescriptorSubtype(0x01), // ENDPOINT_GENERAL
    bmAttributes(0),
    bLockDelayUnits(2),       // Decoded samples
    wLockDelay(0) {}
};

struct USBAudioDescriptor {
  IADDescriptor iad;

  InterfaceDescriptor ACInterface;
  USBAudio_ACInterfaceDescriptor ACInterfaceDescriptor;
  USBAudio_ACInputTerminalDescriptor ACInputTerminalDescriptor;
  // USBAudio_ACFeatureDescriptor ACFeatureDescriptor;
  USBAudio_ACOutputTerminalDescriptor ACOutputTerminalDescriptor;

  InterfaceDescriptorAlternate ASInterface_No_EP;
  InterfaceDescriptorAlternate ASInterface;
  USBAudio_ASInterfaceDescriptor ASInterfaceDescriptor;
  USBAudio_ASType1FormatDescriptor ASType1FormatDescriptor;
  USBAudio_ASIsocEndpointDescriptor ASIsocEndpointDescriptor;
  USBAudio_ASCSIsocEndpointDescriptor ASCSIsocEndpointDescriptor;
};

class USBAudio_: public PluggableUSBModule {
public:
  USBAudio_();

  uint32_t available();
  uint32_t read(void *buf, uint32_t max_size);

protected:
  virtual bool setup(USBSetup &setup) override;
  virtual int getInterface(uint8_t *interfaceCount) override;
  virtual int getDescriptor(USBSetup &setup) override;
  virtual uint8_t getShortName(char *name) override;

private:
  uint8_t audio_control_interface;
  uint8_t audio_stream_interface;

  uint32_t endpoint_type;
};

extern USBAudio_ USBAudio;
