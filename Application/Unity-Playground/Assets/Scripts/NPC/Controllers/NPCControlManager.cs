using UnityEngine;
using System.Collections;

namespace NPC {

    public class NPCControlManager : MonoBehaviour {


        // TODO - change to NPCController
        public Transform NPCTarget = null;
        public bool EnableIO = false;

        bool EnableCamera = false;

        NPCCamController g_NPCCamera;
        NPCUIController g_UI;
        NPCIO g_IO;

	    // Use this for initialization
	    void Awake () {
            g_NPCCamera = Camera.main.GetComponent<NPCCamController>();
            g_UI = GetComponentInChildren<NPCUIController>();
            g_IO = GetComponentInChildren<NPCIO>();
            EnableCamera = g_NPCCamera != null;
            if(EnableCamera) {
                g_NPCCamera.SetCamera(Camera.main.transform);
                g_UI.NPCCamera = g_NPCCamera;
                if (NPCTarget != null) {
                    g_IO.SetTarget(NPCTarget);
                    g_NPCCamera.SetTarget(NPCTarget);
                }
            }
	    }

        void FixedUpdate() {
            if(EnableCamera) {
                if (Input.GetKey(KeyCode.F1)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.FREE);
                } else if (Input.GetKey(KeyCode.F2)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.THIRD_PERSON);
                } else if (Input.GetKey(KeyCode.F3)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.FIRST_PERSON);
                }
            }
            if(EnableIO) {
                g_IO.UpdateIO();
            }
        }
        
        void Update () {
            if (EnableCamera) {
                g_NPCCamera.UpdateCamera();
            }
            g_UI.UpdateUI();
	    }
    }

}
