using UnityEngine;
using System.Collections;
using System;

public class NPCIO : MonoBehaviour, INPCMessage {

    public bool EnableIO = true;
    
    public enum INPUT_KEY {
        FORWARD = KeyCode.W,
        BACKWARD = KeyCode.S,
        LEFT = KeyCode.A,
        RIGHT = KeyCode.D,
        CROUCH = KeyCode.LeftControl,
        JUMP = KeyCode.Space,
        RUN = KeyCode.LeftShift,
        INVENTORY = KeyCode.I,
        MAP = KeyCode.M,
        FREE_CAM = KeyCode.F1,
        THIRD_CAM = KeyCode.F2,
        FIRST_CAM = KeyCode.F3
    }

    
    void Awake() {
        GeneralCamController MainCamera = Camera.main.GetComponent<GeneralCamController>();
        if(MainCamera == null) {
            Debug.Log("No GeneralCamController found, defaulting to characters controlls");
        } else {
            ((INPCMessage) MainCamera).RegisterListener(this);
            if (MainCamera.CurrentMode == GeneralCamController.CAMERA_MODE.FREE) {
                EnableIO = false;
            }
        }
    }
	
	// Update is called once per frame
	void Update () {
        if(EnableIO) {

        }
	}

    #region INPCMessage

    public void UpdateListener(INPCMessage message) {
        if(message is GeneralCamController) {
            GeneralCamController c = message as GeneralCamController;
            if(c.CurrentMode == GeneralCamController.CAMERA_MODE.FREE) {
                Debug.Log("NPCIO --> Camera updated, disabling IO controller for character");
                EnableIO = false;
            } else {
                Debug.Log("NPCIO --> Enabling character IO controllers");
            }
        }
    }

    public void RegisterListener(INPCMessage listener) { throw new NotImplementedException(); }
    public void ServeMessage() { throw new NotImplementedException(); }

    #endregion
}
