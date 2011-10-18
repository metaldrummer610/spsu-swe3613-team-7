#ifndef _ICD_NETWORK_HELPERS_H
#define _ICD_NETWORK_HELPERS_H

#include <enet/enet.h>
class ICDPacket;

void sendPacket(ICDPacket* packet, ENetPeer* p);
void sendPacket(ENetPacket* packet, ENetPeer* p);

ICDPacket* getPacket(ENetPacket* p);
#endif
