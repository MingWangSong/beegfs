FileEvent Documentation
-----------------------

When `sysFileEventLogTarget` is configured, the metadata server tries to
connect to the specified UNIX socket path which must be listened on by a client
known as a "listener". Note that because the metadata server here is the one
that is attempting connect to a known peer, technically, it is a "client" and
the listener is a "server". Internally, the metadata server logs messages about
file system operations (i.e., "events") to an on-disk event queue located at
`sysFileEventPersistDirectory`. Listeners interact with this event queue using
a low level API defined in `FileEventLogger.cpp` by sending and receiving
packets defined by the `PacketType` enum.

Starting in BeeGFS 8, the file event protocol was overhauled (referred to as v2) to support a
sequence-based stream of 2.x event packets addressable by message sequence number (MSN). The
protocol introduces new control packets to manage negotiation and streaming.

# Connection Sequence

```
+----------+                                     +-----------+
| Listener |                                     | Metadata  |
|          |                                     |  Server   |
+----------+                                     +-----------+
     |                                                 |
     | ----------- Handshake_Request ----------------> |
     | <---------- Handshake_Response ---------------- |
     |                                                 |
     | --- Request_Message_Newest or _Range ---------> |
     | <--- Send_Message_Newest or _Range ------------ |
     | --- Request_Message_Stream_Start -------------> |
     | <--------- Send_Message (streamed) ------------ |
     |        ... (repeated messages)                  |
     | --- Close_Request (optional graceful) --------> |
     | <---------------- Send_Close ------------------ |
     |                                                 |
     |           [or connection is closed]             |
```


Connections may be terminated by either party. Breaking the connection causes the metadata server to
close the socket and return to its listening state. Alternatively, either side can initiate a
graceful shutdown using `Close_Request` (listener) or `Send_Close` (server).

# Event Messages

Each `Send_Message` packet contains a serialized BeeGFS file event `packet`, defined in
`beegfs_file_event_log.hpp`. See the user documentation for detailed field descriptions:
https://doc.beegfs.io/latest/advanced_topics/filesystem_modification_events.html.

# Getting Started

[BeeGFS Watch](https://github.com/ThinkParQ/beegfs-go/tree/main/watch) is a production-ready
listener that supports fanning out events to multiple subscribers over high-level protocols such as
protocol buffers and gRPC. Its internal
[implementation](https://github.com/ThinkParQ/beegfs-go/blob/d2e70aee5396151a9b553526fa562b1948ee7105/watch/internal/metadata/manager.go#L264-L279)
for managing v2 connections and serializing/deserializing event packets serves as a useful reference
for developers implementing custom listeners.

A simpler example listener is also included in this repository. It can be built by running `make` in
the `event_listener/build` directory, and the binary is packaged with `beegfs-utils`.
