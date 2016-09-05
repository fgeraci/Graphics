using UnityEngine;
using System.Collections;


/// <summary>
/// INPCMessage is a convenient interface for registering
/// listeners which need to be updated on demand, rather
/// than checking every cycle on update.
/// </summary>

public interface INPCMessage {

    void UpdateListener(INPCMessage message);

    void RegisterListener(INPCMessage listener);

    void ServeMessage();

}
