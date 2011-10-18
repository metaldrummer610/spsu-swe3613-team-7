/**
* \file ICDNetwork
* \brief Global header file that includes all the necessary files to use the ICDNetwork library
* 
* \author robbie diaz
* \version 1.0
* \date 2011-09-11
*/

#ifndef _ICD_NETWORK_H
#define _ICD_NETWORK_H

#define LOG(x) std::cout << x << std::endl

#include "ICDCode.h"
#include "ICDPacket.h"
#include "ICDCommands.h"
#include "ICDNetworkHelpers.h"
#include "Utils.h"
#include "ICDCommandPacket.h"
#include "ICDResponsePacket.h"

/*#include <boost/serialization/export.hpp>

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDPacket)
BOOST_CLASS_EXPORT(ICDPacket)
BOOST_CLASS_EXPORT(ICDResponsePacket)
BOOST_CLASS_EXPORT(ICDCommandPacket)
BOOST_CLASS_EXPORT(ICDCommand)
BOOST_CLASS_EXPORT(ICDResponse)
BOOST_CLASS_EXPORT(ICDCommandConvert9To10)
BOOST_CLASS_EXPORT(ICDResponseConvert9To10)
*/
#endif
