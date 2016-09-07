using UnityEngine;
using System.Collections;
using System;

namespace NPC {

    public class NPCIO : MonoBehaviour {

        bool EnableIO = true;
        NPCController g_NPCController;

        public void SetTarget(Transform target) {
            g_NPCController = target.gameObject.GetComponent<NPCController>();
        }
    
        public enum INPUT_KEY {
            FORWARD = KeyCode.W,
            BACKWARD = KeyCode.S,
            LEFT = KeyCode.A,
            RIGHT = KeyCode.D,
            DUCK = KeyCode.LeftControl,
            JUMP = KeyCode.Space,
            RUN = KeyCode.LeftShift,
            INVENTORY = KeyCode.I,
            MAP = KeyCode.M,
            FREE_CAM = KeyCode.F1,
            THIRD_CAM = KeyCode.F2,
            FIRST_CAM = KeyCode.F3

        }
	
	    // Update is called once per frame
	    public void UpdateIO () {

            // forward motion
            if (Input.GetKey((KeyCode)INPUT_KEY.RUN)) {
                g_NPCController.Body.Move(LOCO_STATE.RUN);
            } else if (Input.GetKey((KeyCode)INPUT_KEY.DUCK)) {
                g_NPCController.Body.Move(LOCO_STATE.DUCK);
            } else g_NPCController.Body.Move(LOCO_STATE.WALK);

            // forward motion
            if (Input.GetKey((KeyCode)INPUT_KEY.FORWARD)) {
                g_NPCController.Body.Move(LOCO_STATE.FORWARD);
            } else if (Input.GetKey((KeyCode)INPUT_KEY.BACKWARD)) {
                g_NPCController.Body.Move(LOCO_STATE.BACKWARDS);
            } else g_NPCController.Body.Move(LOCO_STATE.IDLE);

            // forward motion
            if (Input.GetKey((KeyCode)INPUT_KEY.LEFT)) {
                g_NPCController.Body.Move(LOCO_STATE.LEFT);
            } else if (Input.GetKey((KeyCode)INPUT_KEY.RIGHT)) {
                g_NPCController.Body.Move(LOCO_STATE.RIGHT);
            } else g_NPCController.Body.Move(LOCO_STATE.FRONT);
        }

    }

}
