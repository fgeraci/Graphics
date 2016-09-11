using UnityEngine;
using System.Collections;

namespace NPC {

    public class NPCControlManager : MonoBehaviour {


        // TODO - change to NPCController
        Transform NPCTarget = null;
        NPCController NPCController = null;
        public bool EnableIOController = false;
        public bool EnableCameraController = false;
        public bool EnableUIController = false;

        private static string mNPCIO = "NPC IO";
        private static string mNPCUI = "NPC UI";

        NPCCamController g_NPCCamera;
        NPCUIController g_UI;
        NPCIO g_IO;

        void Reset() {
            
            // Find Main NPC
            if (FindObjectsOfType<NPCControlManager>().Length > 1)
                throw new System.Exception("NPCControlManager --> ERROR - a NPCControlManager has already been added!");

            Debug.Log("NPCControlManager --> Adding components ...");

            // CAM
            Transform cam = Camera.main.transform;
            Camera.main.nearClipPlane = 0.001f;
            if (cam.gameObject.GetComponent<NPCCamController>() == null) { 
                cam.gameObject.AddComponent<NPCCamController>();
                cam.parent = transform;
            }
            cam.parent = this.transform;

            // UI
            Canvas canvas = FindObjectOfType<Canvas>();
            if (canvas != null) {
                GameObject io = null;
                if (GetComponent<NPCUIController>() == null) {
                    io = new GameObject();
                    io.AddComponent<NPCUIController>();
                }
                io.transform.parent = this.transform;
                canvas.transform.SetParent(io.transform);
                io.name = mNPCUI;
            }

            // IO
            GameObject npcIO = new GameObject();
            npcIO.name = mNPCIO;
            npcIO.AddComponent<NPCIO>();
            npcIO.transform.parent = transform;

        }

	    // Use this for initialization
	    void Awake () {
            try {
                transform.position = Vector3.zero;
                transform.rotation = Quaternion.identity;
                FindMainNPC();
                g_NPCCamera = Camera.main.GetComponent<NPCCamController>();
                g_UI = GetComponentInChildren<NPCUIController>();
                g_IO = GetComponentInChildren<NPCIO>();
                g_NPCCamera.SetCamera(Camera.main.transform);
                g_UI.NPCCamera = g_NPCCamera;
                if (NPCTarget != null) {
                    g_IO.SetTarget(NPCTarget);
                    g_NPCCamera.SetTarget(NPCController);
                }
            } catch(System.Exception e) {
                Debug.Log("NPCControlManager --> Components missing from the controller, please add them. Disabling controller: " + e.Message);
                this.enabled = false;
            }
	    }

        public void SetTarget(NPCController c) {
            if(NPCTarget == null) {
                NPCTarget = c.transform;
                NPCController = c;
            }
        }

        void FixedUpdate() {
            if(EnableCameraController) {
                if (Input.GetKey(KeyCode.F1)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.FREE);
                } else if (Input.GetKey(KeyCode.F2)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.THIRD_PERSON);
                } else if (Input.GetKey(KeyCode.F3)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.FIRST_PERSON);
                } else if (Input.GetKey(KeyCode.F4)) {
                    g_NPCCamera.UpdateCameraMode(NPCCamController.CAMERA_MODE.ISOMETRIC);
                }
            }
            if (EnableIOController 
                && g_NPCCamera.Targetting) {
                g_IO.UpdateIO();
            }
        }
        
        void Update () {
            if (EnableCameraController) {
                g_NPCCamera.UpdateCamera();
            }
            if(EnableUIController) {
                g_UI.UpdateUI();
            }
        }

        void FindMainNPC() {
            foreach (NPCController npc in FindObjectsOfType<NPCController>()) {
                if (npc.MainAgent) {
                    if (NPCTarget == null) {
                        NPCTarget = npc.transform;
                        NPCController = npc;
                    } else if (npc != NPCTarget) {
                        NPCController = null;
                        NPCTarget = null;
                        Debug.Log("NPCControlManager --> Many NPCs marked as MainAgents, Target defaults to empty");
                    }
                }
            }
        }
    }

}
