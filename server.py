import random
import string
import json
import sys

import paho.mqtt.client as mqtt

sys.path.append('lib/fleet-protocol/protobuf/compiled/python')

import ExternalProtocol_pb2 as external_protocol
import InternalProtocol_pb2 as internal_protocol

# Status of initial connection sequence with module_gateway -
# (0 - not connected, 1 - after connect sequence, 2 - after status and command sequence)
connection_status = 0
sessionId = ""
command_message_counter = 0
last_device_status = 0

default_json_command = [
    {"outputNumber": 1, "actionType": 1},
    {"outputNumber": 2, "actionType": 1}
]

empty_json_command = {}


def create_connect_response(
    connect_response_type: external_protocol.ConnectResponse.Type,
) -> external_protocol.ExternalServer:
    connect_response = external_protocol.ConnectResponse()
    global sessionId
    connect_response.sessionId = sessionId
    connect_response.type = connect_response_type
    sent_msg = external_protocol.ExternalServer()
    sent_msg.connectResponse.sessionId = sessionId
    sent_msg.connectResponse.type = connect_response_type
    return sent_msg


def create_status_response(message_counter: int) -> external_protocol.ExternalServer:
    status_response = external_protocol.StatusResponse()
    global sessionId
    status_response.sessionId = sessionId
    status_response.type = external_protocol.StatusResponse.Type.OK
    status_response.messageCounter = message_counter
    sent_msg = external_protocol.ExternalServer()
    sent_msg.statusResponse.CopyFrom(status_response)
    return sent_msg


def create_external_command(counter: int, json_command) -> external_protocol.ExternalServer:
    command = external_protocol.Command()
    global sessionId
    command.sessionId = sessionId
    command.messageCounter = counter
    command.deviceCommand.CopyFrom(_create_internal_command(json_command))
    sent_msg = external_protocol.ExternalServer()
    sent_msg.command.CopyFrom(command)
    return sent_msg


def _create_internal_command(json_command) -> internal_protocol.DeviceCommand:
    device_command = internal_protocol.DeviceCommand()
    global last_device_status
    device_command.device.CopyFrom(last_device_status.device)
    device_command.commandData = json.dumps(json_command).encode()
    return device_command


def handle_connect(connect_message, client: mqtt.Client) -> None:
    global sessionId
    sessionId = connect_message.sessionId
    connect_response = create_connect_response(external_protocol.ConnectResponse.Type.OK)
    client.publish("BringAuto/VirtualVehicle/external_server", connect_response.SerializeToString(), qos=0)
    print("Connection with module gateway successfully established, sessionId:", sessionId)


def print_status(status_message) -> None:
    print("deviceState:", status_message.deviceState)
    print("messageCounter:", status_message.messageCounter)
    print(
        "Device: {",
        status_message.deviceStatus.device.module,
        status_message.deviceStatus.device.deviceType,
        status_message.deviceStatus.device.deviceRole,
        status_message.deviceStatus.device.deviceName,
        status_message.deviceStatus.device.priority,
        "}",
    )
    json_status_bytes = json.loads(status_message.deviceStatus.statusData)
    print("Status data:")
    print(json.dumps(json_status_bytes, indent=2))
    if status_message.deviceState == 2:
        json_error_bytes = json.loads(status_message.errorMessage)
        print("Error data:")
        print(json.dumps(json_error_bytes, indent=2))

    print("")


def handle_status(status_message, client: mqtt.Client) -> None:
    print_status(status_message)
    global last_device_status
    last_device_status = status_message.deviceStatus
    message_counter = status_message.messageCounter
    status_response = create_status_response(message_counter)
    client.publish("BringAuto/VirtualVehicle/external_server", status_response.SerializeToString(), qos=0)


def send_command(json_command) -> None:
    global command_message_counter
    command = create_external_command(command_message_counter, json_command)
    command_message_counter += 1
    global last_device_status
    client.publish("BringAuto/VirtualVehicle/external_server", command.SerializeToString(), qos=0)


def on_message(client, userdata, message):
    external_client_message = external_protocol.ExternalClient().FromString(message.payload)
    global connection_status

    # Handling of initial sequence of external connection (simplified with only one device)
    if connection_status == 0:
        if not external_client_message.HasField("connect"):
            print("Connect message expected")
            return

        handle_connect(external_client_message.connect, client)
        connection_status = 1
    elif connection_status == 1:
        if not external_client_message.HasField("status"):
            print("Status message expected")
            return

        handle_status(external_client_message.status, client)
        send_command(default_json_command)
        connection_status = 2

    # Handling 4th step of external connection
    else:
        if external_client_message.HasField("status"):
            handle_status(external_client_message.status, client)
            send_command(empty_json_command)


client = mqtt.Client("".join(random.choices(string.ascii_uppercase + string.digits, k=20)), protocol=mqtt.MQTTv5)
client.on_message = on_message
client.connect(host="127.0.0.1", port=1883, keepalive=60, clean_start=True)
client.subscribe("BringAuto/VirtualVehicle/module_gateway", qos=0)

try:
    client.loop_forever()
except KeyboardInterrupt:
    client.loop_stop()
