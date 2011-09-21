#ifndef _ICD_NETWORK_HELPERS_H
#define _ICD_NETWORK_HELPERS_H

#include <enet/enet.h>
#include "ICDPacket.h"

void sendPacket(ICDPacket* packet, ENetPeer* p);
void sendPacket(ENetPacket* packet, ENetPeer* p);

#endif
