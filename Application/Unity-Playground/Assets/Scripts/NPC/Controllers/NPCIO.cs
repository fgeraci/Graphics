using UnityEngine;
using System.Collections;
using System;

namespace NPC {

    public class NPCIO : MonoBehaviour {

        bool EnableIO = true;
        NPCController g_NPCController;

        public void SetTarget(Transform target) {
            g_NPCController = target.gameObject.GetComponent<NPCController>();
            if (g_NPCController == null) {
                Debug.Log("NPCIO --> No NPCController found in target character for IO - disabling IO");
                EnableIO = false;
            } else EnableIO = true;
        }
    
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
	
	    // Update is called once per frame
	    public void UpdateIO () {
            if(EnableIO) {
                foreach (INPUT_KEY key in Enum.GetValues(typeof(INPUT_KEY))) {
                    if(Input.GetKeyDown((KeyCode)key)) {
                        print(key + "is down");
                    }
                }
            }
	    }
    }

}
