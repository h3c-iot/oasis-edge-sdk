INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external)

ADD_LIBRARY (edge_mqtt OBJECT
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Base64.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Clients.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Heap.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/LinkedList.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Log.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Messages.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTAsync.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTClient.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTPacket.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTPacketOut.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTPersistence.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTPersistenceDefault.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTProperties.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTProtocolClient.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTProtocolOut.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTReasonCodes.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/MQTTVersion.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/OsWrapper.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/SHA1.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Socket.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/SocketBuffer.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/SSLSocket.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/StackTrace.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Thread.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/Tree.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/utf-8.c
        ${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external/WebSocket.c
        )

